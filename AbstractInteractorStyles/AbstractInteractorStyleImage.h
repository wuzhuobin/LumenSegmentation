/*
Author:		Wong, Matthew Lun
Date:		16th, June 2016
Occupation:	Chinese University of Hong Kong,
Department of Imaging and Inteventional Radiology,
Junior Research Assistant


The abstract interactor class used in medical viewers.
This class contains methods of image related processes, including changing slice position,
zoomming, dragging...etc.


Wong Matthew Lun
Copyright (C) 2016
*/

#ifndef ABSTRACT_INTERACTOR_STYLE_IMAGE_H
#define ABSTRACT_INTERACTOR_STYLE_IMAGE_H

#include <QSpinBox>

/// VTK
#include <vtkInteractorStyleImage.h>
#include <vtkObjectFactory.h>
#include <vtkImageViewer2.h>

#include "AbstractInteractorStyle.h"


class AbstractInteractorStyleImage : public vtkInteractorStyleImage
{
public:
	vtkTypeMacro(AbstractInteractorStyleImage, vtkInteractorStyleImage);
	static AbstractInteractorStyleImage *New();
	
	virtual void SetImageViewer(vtkImageViewer2* imageViewer);
	virtual void SetSliceSpinBox(QSpinBox* x, QSpinBox* y, QSpinBox* z);
	virtual void SetOrientation(int orientation);
	virtual int GetOrientation();
	virtual vtkActor* PickActor(int x, int y);
	void SetCurrentSlice(int slice);


//protected:
	AbstractInteractorStyleImage();
	~AbstractInteractorStyleImage();
	
	virtual void OnMouseWheelForward();
	virtual void OnMouseWheelBackward();
	virtual void OnLeftButtonDown();
	virtual void OnLeftButtonUp();
	virtual void OnRightButtonDown();
	virtual void OnRightButtonUp();
	virtual void OnMiddleButtonDown();
	virtual void OnMiddleButtonUp();
	virtual void OnMouseMove();
	virtual void OnChar();
	virtual void OnKeyPressed();

	void MoveSliceForward();
	void MoveSliceBackward();

	QSpinBox* m_sliceSplinBox[3];

	vtkImageViewer2* imageViewer;

	int m_slice;
	int m_minSlice;
	int m_maxSlice;
	int orientation;
	double origin[3];
	double spacing[3];
	int extent[6];

	bool m_rightFunctioning;
	bool m_leftFunctioning;
	bool m_middleFunctioning;
};

#endif //ABSTRACT_INTERACTOR_STYLE_IMAGE_H