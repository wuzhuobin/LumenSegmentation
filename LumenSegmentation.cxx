#include "LumenSegmentation.h"

#include <vtkObjectFactory.h>
#include <vtkExtractVOI.h>
#include <vtkContourFilter.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataConnectivityFilter.h>
#include <vtkGeometryFilter.h>
#include <vtkThreshold.h>
#include <vtkMath.h>
#include <vtkPolygon.h>

#include <vtkXMLPolyDataWriter.h>

vtkStandardNewMacro(LumenSegmentaiton)

void LumenSegmentaiton::PrintSelf(ostream & os, vtkIndent indent)
{
	vtkObject::PrintSelf(os, indent);
}

void LumenSegmentaiton::SetInputData(vtkImageData * input)
{
	this->input = input;
	memcpy(this->VOI, input->GetExtent(), sizeof(VOI));
	//this->input->GetExtent(this->VOI);
}

vtkOrientedGlyphContourRepresentation* LumenSegmentaiton::GetOutput()
{
	return this->lumenWallContourRepresentation;
}

void LumenSegmentaiton::SetVOI(int * VOI)
{
	memcpy(this->VOI, VOI, sizeof(this->VOI));
}

void LumenSegmentaiton::SetVOI(int extent0, int extent1, int extent2, int extent3, int extent4, int extent5)
{
	int VOI[6] = { extent0, extent1, extent2, extent3, extent4, extent5 };
	this->SetVOI(VOI);
}

void LumenSegmentaiton::SetSlice(int slice)
{
	this->SetVOI(VOI[0], VOI[1], VOI[2], VOI[3], slice, slice);
}

void LumenSegmentaiton::SetGenerateValues(double generateValues0, double generateValues1, double generateValues2)
{
	double generateValues[3] = { generateValues0, generateValues1, generateValues2 };
	this->SetGenerateValues(generateValues);
}

void LumenSegmentaiton::SetGenerateValues(double * generateValues)
{
	memcpy(this->generateValues, generateValues, sizeof(this->generateValues));
}

void LumenSegmentaiton::SetContourRepresentation(vtkOrientedGlyphContourRepresentation * contourRepresentation)
{
	this->vesselWallContourRepresentation = contourRepresentation;
}

void LumenSegmentaiton::Update()
{
	vtkSmartPointer<vtkPolyData> vesselWallPolyData = this->vesselWallContourRepresentation->GetContourRepresentationAsPolyData();
	vtkSmartPointer<vtkPolygon> vesselWallPolygon = vtkSmartPointer<vtkPolygon>::New();

	int numOfPoints = vesselWallPolyData->GetNumberOfPoints();
	double origin[3], spacing[3];
	double lastPoint[3] = { VTK_DOUBLE_MAX, VTK_DOUBLE_MAX, VTK_DOUBLE_MAX };
	this->input->GetOrigin(origin);
	this->input->GetSpacing(spacing);

	for (vtkIdType i = 0; i < numOfPoints; i++)
	{
		double displayCoordinate[3];

		vesselWallPolyData->GetPoint(i, displayCoordinate);

		//Take one image data 1 to be reference
		displayCoordinate[0] = (displayCoordinate[0] - origin[0]) / spacing[0];
		displayCoordinate[1] = (displayCoordinate[1] - origin[1]) / spacing[1];
		displayCoordinate[2] = (displayCoordinate[2] - origin[2]) / spacing[2];
		//cout << s[0] << " " << s[1] << " " << s[2] << endl;
		//Test whether the points are inside the polygon or not
		// if the points is too close to the previous point, skip it to avoid error in PointInPolygon algorithm
		double d = vtkMath::Distance2BetweenPoints(lastPoint, displayCoordinate);
		if (d < 1E-5)
			continue;
		memcpy(lastPoint, displayCoordinate, sizeof(double) * 3);
		vesselWallPolygon->GetPoints()->InsertNextPoint(displayCoordinate[0], displayCoordinate[1], displayCoordinate[2]);
	}
	double vesselWallPolygonNormal[3];
	vesselWallPolygon->ComputeNormal(vesselWallPolygon->GetPoints()->GetNumberOfPoints(),
		static_cast<double*>(vesselWallPolygon->GetPoints()->GetData()->GetVoidPointer(0)), vesselWallPolygonNormal);

	vtkSmartPointer<vtkExtractVOI> extractVOI =
		vtkSmartPointer<vtkExtractVOI>::New();
	extractVOI->SetInputData(input);
	extractVOI->SetVOI(this->VOI);
	extractVOI->Update();
	extract = extractVOI->GetOutput();

	vtkSmartPointer<vtkContourFilter> contourFilter =
		vtkSmartPointer<vtkContourFilter>::New();
	contourFilter->SetInputConnection(extractVOI->GetOutputPort());
	contourFilter->GenerateValues(generateValues[0], generateValues[1], generateValues[2]); // (numContours, rangeStart, rangeEnd)
	contourFilter->Update();

	vtkSmartPointer<vtkPolyDataConnectivityFilter> connectivityFilter =
		vtkSmartPointer<vtkPolyDataConnectivityFilter>::New();
	connectivityFilter->SetInputConnection(contourFilter->GetOutputPort());
	connectivityFilter->SetExtractionModeToAllRegions();
	connectivityFilter->ColorRegionsOn();
	connectivityFilter->Update();
	cerr << "number of extracted regions:" << connectivityFilter->GetNumberOfExtractedRegions() << endl;

	bool foundFlag = false;
	// Generate the threshold
	for (int i = 0; i < connectivityFilter->GetNumberOfExtractedRegions(); i++)
	{
		if (foundFlag == true)
			break;
		cout << "finding in loop " << i << endl;
		vtkSmartPointer<vtkThreshold> thres = vtkSmartPointer<vtkThreshold>::New();
		thres->SetInputConnection(connectivityFilter->GetOutputPort());
		thres->SetInputArrayToProcess(1, 0, 0, vtkDataObject::FIELD_ASSOCIATION_POINTS, "RegionID");
		thres->ThresholdBetween((double)i - 0.1, (double)i + 0.1);
		thres->Update();

		vtkSmartPointer<vtkGeometryFilter> gfilter = vtkSmartPointer<vtkGeometryFilter>::New();
		gfilter->SetInputConnection(thres->GetOutputPort());
		gfilter->Update();

		// check each loop if it is inside the region specified by contour widget
		vtkSmartPointer<vtkPolyData>lumenWallPolyData = gfilter->GetOutput();
		cout << "lumenWallPolyData has number of points" << gfilter->GetOutput()->GetNumberOfPoints() << endl;
		for (vtkIdType j = 0; j < lumenWallPolyData->GetNumberOfPoints(); ++j) {
			cout << "finding in " << j << endl;
			if (!vesselWallPolygon->PointInPolygon(lumenWallPolyData->GetPoint(j),
				vesselWallPolygon->GetPoints()->GetNumberOfPoints(),
				static_cast<double*>(vesselWallPolygon->GetPoints()->GetData()->GetVoidPointer(0)),
				vesselWallPolygon->GetPoints()->GetBounds(), vesselWallPolygonNormal)) {
				break;
			}
			if (j == (lumenWallPolyData->GetNumberOfPoints() - 1)) {
				foundFlag = true;
				cout << "foundFlat" << endl;
				vtkSmartPointer<vtkPolyDataConnectivityFilter> connectivityFilter =
					vtkSmartPointer<vtkPolyDataConnectivityFilter>::New();
				connectivityFilter->SetInputConnection(contourFilter->GetOutputPort());
				connectivityFilter->SetExtractionModeToSpecifiedRegions();
				connectivityFilter->AddSpecifiedRegion(i);
				connectivityFilter->Update();
				this->m_contour = connectivityFilter->GetOutput();
			}
		}
		//this->m_contour = 
		
	}


	//int num = connectivityFilter->GetNumberOfExtractedRegions();

	//for (int i = 0; i < num; ++i) {
	//	vtkSmartPointer<vtkPolyDataConnectivityFilter> connectivityFilter =
	//		vtkSmartPointer<vtkPolyDataConnectivityFilter>::New();
	//	connectivityFilter->SetInputConnection(contourFilter->GetOutputPort());
	//	connectivityFilter->SetExtractionModeToSpecifiedRegions();
	//	connectivityFilter->InitializeSpecifiedRegionList();
	//	connectivityFilter->AddSpecifiedRegion(i);
	//	connectivityFilter->Update();
	//	regions->AddItem(connectivityFilter->GetOutput());
	//}
	//connectivityFilter->SetExtractionModeToAllRegions();
	//connectivityFilter->SetExtractionModeToLargestRegion();
	//connectivityFilter->SetExtractionModeToSpecifiedRegions();
	//connectivityFilter->AddSpecifiedRegion(5);
	//connectivityFilter->ColorRegionsOn();
	//connectivityFilter->Update();
	//regions->InitTraversal();
/*
	this->m_contour = regions->GetNextItem();
	this->m_contour = regions->GetNextItem();
	this->m_contour = regions->GetNextItem();
	this->m_contour = regions->GetNextItem();
	this->m_contour = regions->GetNextItem();
*/
	//vtkSmartPointer<vtkXMLPolyDataWriter> writer = vtkSmartPointer<vtkXMLPolyDataWriter>::New();
	//writer->SetFileName("E:/Test.vtp");
	//writer->SetInputData(regions->GetNextItem());
	//writer->Update();
	//writer->Write();










}

LumenSegmentaiton::LumenSegmentaiton()
{
	//this->input = vtkSmartPointer<vtkImageData>::New();
	//this->regions = vtkSmartPointer<vtkPolyDataCollection>::New();
	this->lumenWallContourRepresentation = NULL;
}
