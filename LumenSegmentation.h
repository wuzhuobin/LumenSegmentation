#ifndef __LUMENSEGMENTATION_H
#define __LUMENSEGMENTATION_H

#include <vtkObject.h>
#include <vtkImageData.h>
#include <vtkSmartPointer.h>
#include <vtkPolyData.h>


class LumenSegmentaiton : public vtkObject
{
public:
	vtkTypeMacro(LumenSegmentaiton, vtkObject);
	void PrintSelf(ostream& os, vtkIndent indent);

	static LumenSegmentaiton *New();

	void SetInputData(vtkImageData* input);

	vtkImageData* GetOutput();

	void SetVOI(int* VOI);
	void SetVOI(int extent0, int extent1, int extent2, 
		int extent3, int extent4, int extent5);
	void SetSlice(int slice);

	void SetGenerateValues(double generateValues0, double generateValues1, double generateValues2);
	void SetGenerateValues(double* generateValues);

	void Update();

//protected:
	LumenSegmentaiton();
	~LumenSegmentaiton() {};

	//int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);


	vtkSmartPointer<vtkPolyData> contour;
	vtkSmartPointer<vtkImageData> input;
	vtkSmartPointer<vtkImageData> output;

	double generateValues[3] = { 0 };
	int VOI[6] = { 0 };

private:
	LumenSegmentaiton(const LumenSegmentaiton&);  // Not implemented.
	void operator=(const LumenSegmentaiton&);  // Not implemented.

};

#endif