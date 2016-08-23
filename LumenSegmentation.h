#ifndef __LUMENSEGMENTATION_H
#define __LUMENSEGMENTATION_H

#include "vtkImageAlgorithm.h"
#include <vtkSmartPointer.h>


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

	void Update();

protected:
	LumenSegmentaiton();
	~LumenSegmentaiton() {};

	//int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);



	vtkSmartPointer<vtkImageData> input;
	vtkSmartPointer<vtkImageData> output;
	int VOI[6] = { 0 };

private:
	LumenSegmentaiton(const LumenSegmentaiton&);  // Not implemented.
	void operator=(const LumenSegmentaiton&);  // Not implemented.

};

#endif