/*
Author:		Wong, Matthew Lun
Date:		16th, June 2016
Occupation:	Chinese University of Hong Kong,
			Department of Imaging and Inteventional Radiology,
			Junior Research Assistant

Author:		Lok, Ka Hei Jason
Date:		16th, June 2016
Occupation:	Chinese University of Hong Kong,
			Department of Imaging and Inteventional Radiology,
			M.Phil Student

This class allows interactive segmentation on images with contour widget.

Wong Matthew Lun, Lok Ka Hei
Copyright (C) 2016
*/

#include <vtkContourWidget.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkOrientedGlyphContourRepresentation.h>
#include <vtkInteractorStyleImage.h>
#include <vtkPolygon.h>
#include <vtkCallbackCommand.h>
#include <vtkCommand.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkMath.h>
#include <vtkProperty.h>

#include "InteractorStylePolygonDraw.h"
#include "LumenSegmentation.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkRenderer.h"

vtkStandardNewMacro(InteractorStylePolygonDraw);

InteractorStylePolygonDraw::InteractorStylePolygonDraw()
	:AbstractInteractorStyleImage()
{
	DOUBLE_CLICKED_FLAG = false;
	CONTOUR_IS_ON_FLAG = false;
	m_timer.start();
	m_firstClickTimeStamp = m_timer.elapsed();
	m_contourWidget = NULL;
	m_contourRep = NULL;
	label = 1;
}

InteractorStylePolygonDraw::~InteractorStylePolygonDraw()
{
	if (m_contourRep) {
		m_contourRep->Delete();
		m_contourRep = NULL;
	}

	if (m_contourWidget) {
		m_contourWidget->Delete();
		m_contourWidget = NULL;
	}
}

void InteractorStylePolygonDraw::OnLeftButtonDown()
{
	if (this->CheckDoubleClicked() && CONTOUR_IS_ON_FLAG) {
		this->FillPolygon();
		this->SetPolygonModeEnabled(false);
		this->CONTOUR_IS_ON_FLAG = false;
	}
	else if (!CONTOUR_IS_ON_FLAG){
		this->SetPolygonModeEnabled(true);
		this->CONTOUR_IS_ON_FLAG = true;
	}

	AbstractInteractorStyleImage::OnLeftButtonDown();
}

void InteractorStylePolygonDraw::OnRightButtonDown()
{
	if (CONTOUR_IS_ON_FLAG) {
		this->FillPolygon();
		this->SetPolygonModeEnabled(false);
		this->CONTOUR_IS_ON_FLAG = false;
	}

	AbstractInteractorStyleImage::OnRightButtonDown();
}

void InteractorStylePolygonDraw::OnMouseMove()
{
	if (!this->CONTOUR_IS_ON_FLAG) {
		this->SetPolygonModeEnabled(true);
		this->CONTOUR_IS_ON_FLAG = true;
	}
}

void InteractorStylePolygonDraw::OnKeyPress()
{
	std::string key = this->Interactor->GetKeySym();

	std::cout << key << endl;

	if (key == "n") {
		NewContourWidget();
	}

	if (key == "a") {
		cout << "FILLPOLYGON" << endl;
		FillPolygon();
	}

	if (key == "Escape") {
		this->SetPolygonModeEnabled(false);
	}
	if (key == "Return" && m_contourWidget) {
		if (CONTOUR_IS_ON_FLAG)
			this->FillPolygon();
	}
	AbstractInteractorStyleImage::OnKeyPress();
}

bool InteractorStylePolygonDraw::CheckDoubleClicked()
{
	int t = m_timer.elapsed() - m_firstClickTimeStamp;

	if (t < 200 && !DOUBLE_CLICKED_FLAG) {
		DOUBLE_CLICKED_FLAG = true;
		m_firstClickTimeStamp = m_timer.elapsed();
		return true;
	}
	else {
		DOUBLE_CLICKED_FLAG = false;
		m_firstClickTimeStamp = m_timer.elapsed();
		return false;
	}
}

void InteractorStylePolygonDraw::SetPolygonModeEnabled(bool b)
{
	if (m_contourWidget) {
		m_contourWidget->Off();
		m_contourWidget->SetRepresentation(NULL);
		m_contourWidget->EnabledOff();
		m_contourWidget->Delete();
		m_contourWidget = NULL;
	}


	if (m_contourRep) {
		m_contourRep->Delete();
		m_contourRep = NULL;
	}
	if (b)
	{
		m_contourRep = vtkOrientedGlyphContourRepresentation::New();
		m_contourRep->SetRenderer(imageViewer->GetRenderer());
		m_contourRep->SetNeedToRender(true);
		m_contourRep->GetLinesProperty()->SetColor(255, 255, 0);
		m_contourRep->SetLineInterpolator(NULL);
		m_contourRep->AlwaysOnTopOn();
		m_contourRep->BuildRepresentation();

		m_contourWidget = vtkContourWidget::New();
		m_contourWidget->SetInteractor(this->Interactor);
		m_contourWidget->SetRepresentation(m_contourRep);
		m_contourWidget->SetDefaultRenderer(imageViewer->GetRenderer());
		m_contourWidget->FollowCursorOn();
		m_contourWidget->ContinuousDrawOn();
		//m_contourWidget->CreateDefaultRepresentation();
		m_contourWidget->On();
		m_contourWidget->EnabledOn();



		this->imageViewer->Render();
		this->CONTOUR_IS_ON_FLAG = true;
	} else{
		this->CONTOUR_IS_ON_FLAG = false;
	}
}

void InteractorStylePolygonDraw::SetLabel(int label)
{
	this->label = label;
}

void InteractorStylePolygonDraw::DisplayPolygon(vtkObject* caller, long unsigned vtkNotUsed(eventId), void* vtkNotUsed(clientData))
{
	imageViewer->Render();
}

void InteractorStylePolygonDraw::FillPolygon()
{

	vtkSmartPointer<vtkContourWidget> contourWidget =
		vtkSmartPointer<vtkContourWidget>::New();
	contourWidget->SetInteractor(this->Interactor);
	contourWidget->SetDefaultRenderer(imageViewer->GetRenderer());

	vtkSmartPointer<vtkOrientedGlyphContourRepresentation> contourRep =
		vtkSmartPointer<vtkOrientedGlyphContourRepresentation>::New();
	contourRep->SetRenderer(imageViewer->GetRenderer());
	contourRep->SetNeedToRender(true);
	contourRep->GetLinesProperty()->SetColor(0, 255, 0);
	contourRep->SetLineInterpolator(NULL);
	contourRep->AlwaysOnTopOn();
	contourRep->BuildRepresentation();

	contourWidget->SetRepresentation(contourRep);
	contourWidget->FollowCursorOn();
	contourWidget->ContinuousDrawOn();
	contourWidget->On();
	contourWidget->EnabledOn();

	int voi[6] = { 100,200,200,300,55,55 };

	vtkSmartPointer<LumenSegmentaiton> ls =
		vtkSmartPointer<LumenSegmentaiton>::New();
	ls->SetInputData(imageViewer->GetInput());
	ls->SetVOI(voi);
	ls->SetGenerateValues(1, 60, 60);
	ls->SetVesselWallContourRepresentation(this->m_contourRep);
	//ls->SetLumenWallContourRepresentation(contourRep);
	ls->Update();

	contourWidget->Initialize(ls->m_contour);
	cout << "Lines" << ls->m_contour->GetNumberOfPolys() << endl;

	vtkSmartPointer<vtkPolyDataMapper> contourMapper =
		vtkSmartPointer<vtkPolyDataMapper>::New();
	contourMapper->SetInputData(ls->m_contour);

	// Create an actor for the contours
	vtkSmartPointer<vtkActor> contourActor =
		vtkSmartPointer<vtkActor>::New();
	contourActor->SetMapper(contourMapper);
	imageViewer->GetRenderer()->AddActor(contourActor);
	imageViewer->Render();

}

void InteractorStylePolygonDraw::NewContourWidget()
{
		vtkContourWidget* m_contourWidget = vtkContourWidget::New();
		m_contourWidget->SetInteractor(this->Interactor);
		m_contourWidget->SetDefaultRenderer(imageViewer->GetRenderer());

		vtkOrientedGlyphContourRepresentation* m_contourRep = vtkOrientedGlyphContourRepresentation::New();
		m_contourRep->SetRenderer(imageViewer->GetRenderer());
		m_contourRep->SetNeedToRender(true);
		m_contourRep->GetLinesProperty()->SetColor(255, 255, 0);
		m_contourRep->SetLineInterpolator(NULL);
		m_contourRep->AlwaysOnTopOn();


		//vtkPolyData* cursorpolyData = m_contourRep->GetActiveCursorShape();
		//vtkSmartPointer<vtkTransform> translation = vtkSmartPointer<vtkTransform>::New();
		//if (orientation == 0) {
		//	translation->RotateX(90);
		//	translation->RotateZ(90);
		//}
		//else if (orientation == 1) {
		//	translation->RotateX(90);
		//	translation->RotateY(90);
		//}

		//vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter = 
		//	vtkSmartPointer<vtkTransformPolyDataFilter>::New();
		//transformFilter->SetInputData(cursorpolyData);
		//transformFilter->SetTransform(translation);
		//transformFilter->Update();

		//cursorpolyData->DeepCopy(transformFilter->GetOutput());
		m_contourRep->BuildRepresentation();
		m_contourWidget->SetRepresentation(m_contourRep);
		m_contourWidget->FollowCursorOn();
		m_contourWidget->ContinuousDrawOn();
		//m_contourWidget->CreateDefaultRepresentation();
		m_contourWidget->On();
		m_contourWidget->EnabledOn();

		this->imageViewer->Render();
		this->CONTOUR_IS_ON_FLAG = true;
}

