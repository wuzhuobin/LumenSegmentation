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

//#include <vtkXMLPolyDataWriter.h>

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

vtkPolyData* LumenSegmentaiton::GetOutput()
{
	return this->m_contour;
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

void LumenSegmentaiton::SetVesselWallContourRepresentation(vtkOrientedGlyphContourRepresentation * vesselWallContourRepresentation)
{
	this->vesselWallContourRepresentation = vesselWallContourRepresentation;
}

void LumenSegmentaiton::SetLumenWallContourRepresentation(vtkOrientedGlyphContourRepresentation * lumenWallContourRepresentation)
{
	this->lumenWallContourRepresentation = lumenWallContourRepresentation;
}

vtkOrientedGlyphContourRepresentation * LumenSegmentaiton::GetVesselWallContourRepresentation()
{
	return this->vesselWallContourRepresentation;
}

vtkOrientedGlyphContourRepresentation * LumenSegmentaiton::GetLumenWallContourRepresentation()
{
	return this->lumenWallContourRepresentation;
}

void LumenSegmentaiton::Update()
{
	vtkSmartPointer<vtkPolyData> vesselWallPolyData = this->vesselWallContourRepresentation->GetContourRepresentationAsPolyData();
	vtkSmartPointer<vtkPolygon> vesselWallPolygon = vtkSmartPointer<vtkPolygon>::New();


	double lastPoint[3] = { VTK_DOUBLE_MAX, VTK_DOUBLE_MAX, VTK_DOUBLE_MAX };

	// add all vesselWallPolyData into vesselWallPolygon
	for (vtkIdType i = 0; i < vesselWallPolyData->GetNumberOfPoints(); i++)
	{
		double worldCoordinate[3];

		vesselWallPolyData->GetPoint(i, worldCoordinate);

		// if the points is too close to the previous point, skip it to avoid error in PointInPolygon algorithm
		double d = vtkMath::Distance2BetweenPoints(lastPoint, worldCoordinate);
		if (d < 1E-5)
			continue;
		memcpy(lastPoint, worldCoordinate, sizeof(double) * 3);
		vesselWallPolygon->GetPoints()->InsertNextPoint(worldCoordinate[0], worldCoordinate[1], worldCoordinate[2]);
	}
	double vesselWallPolygonNormal[3];
	vesselWallPolygon->ComputeNormal(vesselWallPolygon->GetPoints()->GetNumberOfPoints(),
		static_cast<double*>(vesselWallPolygon->GetPoints()->GetData()->GetVoidPointer(0)), vesselWallPolygonNormal);

	const double* spacing = input->GetSpacing();
	const double* origin = input->GetOrigin();
	const double* bounds = vesselWallPolygon->GetPoints()->GetBounds();
	const int* extent = input->GetExtent();
	int newVOI[4];
	// uising the vesselWallPolygon to find a smaller VOI to extract
	//newVOI[0] = (bounds[0] - origin[0]) / spacing[0] - 1;
	//newVOI[1] = (bounds[1] - origin[0]) / spacing[0] + 1;
	//newVOI[2] = (bounds[2] - origin[1]) / spacing[1] - 1;
	//newVOI[3] = (bounds[3] - origin[1]) / spacing[1] + 1;

	//this->VOI[0] = newVOI[0] >= extent[0] ? newVOI[0] : extent[0];
	//this->VOI[1] = newVOI[1] <= extent[1] ? newVOI[1] : extent[1];
	//this->VOI[2] = newVOI[2] >= extent[2] ? newVOI[2] : extent[2];
	//this->VOI[3] = newVOI[3] <= extent[3] ? newVOI[3] : extent[3];

	vtkSmartPointer<vtkExtractVOI> extractVOI =
		vtkSmartPointer<vtkExtractVOI>::New();
	extractVOI->SetInputData(input);
	extractVOI->SetVOI(this->VOI);
	extractVOI->Update();
	extract = extractVOI->GetOutput();

	// ouput contour as PolyData
	vtkSmartPointer<vtkContourFilter> contourFilter =
		vtkSmartPointer<vtkContourFilter>::New();
	contourFilter->SetInputConnection(extractVOI->GetOutputPort());
	contourFilter->GenerateValues(generateValues[0], generateValues[1], generateValues[2]); // (numContours, rangeStart, rangeEnd)
	contourFilter->Update();

	// output connected contour as PolyData
	vtkSmartPointer<vtkPolyDataConnectivityFilter> connectivityFilter =
		vtkSmartPointer<vtkPolyDataConnectivityFilter>::New();
	connectivityFilter->SetInputConnection(contourFilter->GetOutputPort());
	connectivityFilter->SetExtractionModeToAllRegions();
	connectivityFilter->ColorRegionsOn();
	connectivityFilter->Update();
	cerr << "number of extracted regions:" << connectivityFilter->GetNumberOfExtractedRegions() << endl;

	bool foundFlag = false;
	// Generate the threshold
	for (int i = 0; i < connectivityFilter->GetNumberOfExtractedRegions() && !foundFlag; i++)
	{
		//if (foundFlag == true)
			//break;
		cout << "finding in loop " << i << endl;
		vtkSmartPointer<vtkThreshold> thres = vtkSmartPointer<vtkThreshold>::New();
		thres->SetInputConnection(connectivityFilter->GetOutputPort());
		// using RegionID as the threshold of Threshold Filter
		thres->SetInputArrayToProcess(1, 0, 0, vtkDataObject::FIELD_ASSOCIATION_POINTS, "RegionID");
		thres->ThresholdBetween((double)i - 0.1, (double)i + 0.1);
		thres->Update();

		// convert UnstructedGrid data to PolyData
		vtkSmartPointer<vtkGeometryFilter> gfilter = vtkSmartPointer<vtkGeometryFilter>::New();
		gfilter->SetInputConnection(thres->GetOutputPort());
		gfilter->Update();
		vtkSmartPointer<vtkPolyData>lumenWallPolyData = gfilter->GetOutput();
		cout << "lumenWallPolyData has number of points" << gfilter->GetOutput()->GetNumberOfPoints() << endl;

		// clear all nodes from the last polyData
		if (this->lumenWallContourRepresentation != NULL) {
			this->lumenWallContourRepresentation->ClearAllNodes();
		}

		// check each loop if it is inside the region specified by contour widget
		for (vtkIdType j = 0; j < lumenWallPolyData->GetNumberOfPoints(); ++j) {

			cout << lumenWallPolyData->GetPoint(j)[0] << ' ' << lumenWallPolyData->GetPoint(j)[1] << ' '
				<< lumenWallPolyData->GetPoint(j)[2] << endl;
			if (this->lumenWallContourRepresentation != NULL) {
				this->lumenWallContourRepresentation->AddNodeAtWorldPosition(lumenWallPolyData->GetPoint(j));
			}
			cout << "finding in " << j << endl;
			// Test whether the points are inside the vesselWallPolygon or not
			// if one of the point is not inside break and goto next  
			if (!vesselWallPolygon->PointInPolygon(lumenWallPolyData->GetPoint(j),
				vesselWallPolygon->GetPoints()->GetNumberOfPoints(),
				static_cast<double*>(vesselWallPolygon->GetPoints()->GetData()->GetVoidPointer(0)),
				vesselWallPolygon->GetPoints()->GetBounds(), vesselWallPolygonNormal)) {
				break;
			}
			// if the last point is still in the vesselWallPolygon 
			// set foundFlag to true and save its polyData
			if (j == (lumenWallPolyData->GetNumberOfPoints() - 1)) {
				foundFlag = true;
				cout << "foundFlat" << endl;

				//vtkSmartPointer<vtkPolyDataConnectivityFilter> connectivityFilter =
				//	vtkSmartPointer<vtkPolyDataConnectivityFilter>::New();
				//connectivityFilter->SetInputConnection(contourFilter->GetOutputPort());
				//connectivityFilter->SetExtractionModeToSpecifiedRegions();
				//connectivityFilter->AddSpecifiedRegion(i);
				//connectivityFilter->Update();
				this->m_contour = lumenWallPolyData;
			}
		}
		
	}


}
