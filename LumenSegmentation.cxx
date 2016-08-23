#include "LumenSegmentation.h"

#include <vtkObjectFactory.h>
#include <vtkExtractVOI.h>
#include <vtkImageData.h>
#include <vtkContourFilter.h>
#include <vtkPolyDataMapper.h>

vtkStandardNewMacro(LumenSegmentaiton)

void LumenSegmentaiton::PrintSelf(ostream & os, vtkIndent indent)
{
	vtkObject::PrintSelf(os, indent);
}

void LumenSegmentaiton::SetInputData(vtkImageData * input)
{
	this->input = input;
	this->input->GetExtent(this->VOI);
}

vtkImageData * LumenSegmentaiton::GetOutput()
{
	return this->output;
}

void LumenSegmentaiton::SetVOI(int * VOI)
{
	memcpy(this->VOI, VOI, sizeof(VOI));
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

void LumenSegmentaiton::Update()
{
	vtkSmartPointer<vtkExtractVOI> extractVOI =
		vtkSmartPointer<vtkExtractVOI>::New();
	extractVOI->SetVOI(this->VOI);
	extractVOI->SetInputData(input);

	vtkSmartPointer<vtkContourFilter> contourFilter =
		vtkSmartPointer<vtkContourFilter>::New();
	contourFilter->SetInputConnection(extractVOI->GetOutputPort());
	contourFilter->GenerateValues(1, 10, 10); // (numContours, rangeStart, rangeEnd)

	// Map the contours to graphical primitives
	vtkSmartPointer<vtkPolyDataMapper> contourMapper =
		vtkSmartPointer<vtkPolyDataMapper>::New();
	contourMapper->SetInputConnection(contourFilter->GetOutputPort());
	contourMapper->Update();

	//vtkSmartPointer<vtkActor>

}

LumenSegmentaiton::LumenSegmentaiton()
{
	this->input = vtkSmartPointer<vtkImageData>::New();
	this->output = vtkSmartPointer<vtkImageData>::New();
}
