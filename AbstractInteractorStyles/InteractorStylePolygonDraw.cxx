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
#include "MyImageViewer2.h"
//#include "MainWindow.h"

vtkStandardNewMacro(InteractorStylePolygonDraw);

InteractorStylePolygonDraw::InteractorStylePolygonDraw()
	:AbstractInteractorStyleImage()
{
	DOUBLE_CLICKED_FLAG = false;
	CONTOUR_IS_ON_FLAG = false;
	m_timer.start();
	m_firstClickTimeStamp = m_timer.elapsed();
	m_vesselWallContourWidget = NULL;
	m_lumenWallContourWidget = NULL;
	m_vesselWallContourRepresentation = NULL;
	m_lumenWallContourRepresentation = NULL;
	label = 1;
}

InteractorStylePolygonDraw::~InteractorStylePolygonDraw()
{
	if (m_vesselWallContourRepresentation) {
		m_vesselWallContourRepresentation->Delete();
		m_vesselWallContourRepresentation = NULL;
	}

	if (m_vesselWallContourWidget) {
		m_vesselWallContourWidget->Delete();
		m_vesselWallContourWidget = NULL;
	}
	if (m_lumenWallContourRepresentation) {
		m_lumenWallContourRepresentation->Delete();
		m_lumenWallContourRepresentation = NULL;
	}

	if (m_lumenWallContourWidget) {
		m_lumenWallContourWidget->Delete();
		m_lumenWallContourWidget = NULL;
	}
}

void InteractorStylePolygonDraw::OnLeftButtonDown()
{
	if (this->CheckDoubleClicked() && CONTOUR_IS_ON_FLAG) {
		this->m_vesselWallContourWidget->CloseLoop();
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
		this->m_vesselWallContourWidget->CloseLoop();
		this->GenerateLumenWallContourWidget();
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

	if (key == "a") {
	
	}

	if (key == "Escape") {
		this->SetPolygonModeEnabled(false);
	}
	if (key == "Return" && m_vesselWallContourWidget) {
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
	if (m_vesselWallContourWidget) {
		m_vesselWallContourWidget->Off();
		m_vesselWallContourWidget->SetRepresentation(NULL);
		m_vesselWallContourWidget->EnabledOff();
		m_vesselWallContourWidget->Delete();
		m_vesselWallContourWidget = NULL;
	}


	if (m_vesselWallContourRepresentation) {
		m_vesselWallContourRepresentation->Delete();
		m_vesselWallContourRepresentation = NULL;
	}
	if (m_lumenWallContourWidget) {
		m_lumenWallContourWidget->Off();
		m_lumenWallContourWidget->SetRepresentation(NULL);
		m_lumenWallContourWidget->EnabledOff();
		m_lumenWallContourWidget->Delete();
		m_lumenWallContourWidget = NULL;
	}


	if (m_lumenWallContourRepresentation) {
		m_lumenWallContourRepresentation->Delete();
		m_lumenWallContourRepresentation = NULL;
	}
	if (b)
	{
		m_vesselWallContourRepresentation = vtkOrientedGlyphContourRepresentation::New();

		MyImageViewer2* viewer2 = dynamic_cast<MyImageViewer2*>(imageViewer);
		if (viewer2 != NULL) {
			cout << "viewer2" << endl;
			m_vesselWallContourRepresentation->SetRenderer(viewer2->GetAnnotationRenderer());
		}
		else{
			m_vesselWallContourRepresentation->SetRenderer(imageViewer->GetRenderer());
		}

		m_vesselWallContourRepresentation->SetNeedToRender(true);
		m_vesselWallContourRepresentation->GetLinesProperty()->SetColor(0, 0, 255);
		m_vesselWallContourRepresentation->SetLineInterpolator(NULL);
		m_vesselWallContourRepresentation->AlwaysOnTopOn();
		m_vesselWallContourRepresentation->BuildRepresentation();

		m_vesselWallContourWidget = vtkContourWidget::New();
		m_vesselWallContourWidget->SetInteractor(this->Interactor);
		m_vesselWallContourWidget->SetRepresentation(m_vesselWallContourRepresentation);
		if (viewer2 != NULL) {
			cout << "viewer2" << endl;

			m_vesselWallContourWidget->SetDefaultRenderer(viewer2->GetAnnotationRenderer());
		}
		else {
			m_vesselWallContourWidget->SetDefaultRenderer(imageViewer->GetRenderer());
		}
		m_vesselWallContourWidget->FollowCursorOn();
		m_vesselWallContourWidget->ContinuousDrawOn();
		//m_vesselWallContourWidget->CreateDefaultRepresentation();
		m_vesselWallContourWidget->On();
		m_vesselWallContourWidget->EnabledOn();



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

void InteractorStylePolygonDraw::GenerateLumenWallContourWidget()
{
	if (m_lumenWallContourWidget) {
		m_lumenWallContourWidget->Off();
		m_lumenWallContourWidget->SetRepresentation(NULL);
		m_lumenWallContourWidget->EnabledOff();
		m_lumenWallContourWidget->Delete();
		m_lumenWallContourWidget = NULL;
	}


	if (m_lumenWallContourRepresentation) {
		m_lumenWallContourRepresentation->Delete();
		m_lumenWallContourRepresentation = NULL;
	}
	if (this->CONTOUR_IS_ON_FLAG) {
		m_lumenWallContourRepresentation = vtkOrientedGlyphContourRepresentation::New();
		MyImageViewer2* viewer2 = dynamic_cast<MyImageViewer2*>(imageViewer);
		if (viewer2 != NULL) {
			cout << "viewer2" << endl;
			m_lumenWallContourRepresentation->SetRenderer(viewer2->GetAnnotationRenderer());
		}
		else {
			m_lumenWallContourRepresentation->SetRenderer(imageViewer->GetRenderer());
		}
		m_lumenWallContourRepresentation->SetNeedToRender(true);
		m_lumenWallContourRepresentation->GetLinesProperty()->SetColor(255, 0, 0);
		m_lumenWallContourRepresentation->SetLineInterpolator(NULL);
		m_lumenWallContourRepresentation->AlwaysOnTopOn();
		m_lumenWallContourRepresentation->BuildRepresentation();

		m_lumenWallContourWidget = vtkContourWidget::New();
		m_lumenWallContourWidget->SetInteractor(this->Interactor);
		m_lumenWallContourWidget->SetRepresentation(m_lumenWallContourRepresentation);
		if (viewer2 != NULL) {
			cout << "viewer2" << endl;
			m_lumenWallContourWidget->SetDefaultRenderer(viewer2->GetAnnotationRenderer());
		}
		else {
			m_lumenWallContourWidget->SetDefaultRenderer(imageViewer->GetRenderer());
		}
		m_lumenWallContourWidget->FollowCursorOn();
		m_lumenWallContourWidget->ContinuousDrawOn();
		m_lumenWallContourWidget->On();
		m_lumenWallContourWidget->EnabledOn();

		vtkSmartPointer<LumenSegmentaiton> ls =
			vtkSmartPointer<LumenSegmentaiton>::New();
		ls->SetInputData(imageViewer->GetInput());
		cout << "m_slice:";
		cout << m_slice << endl;
		ls->SetSlice(imageViewer->GetSlice());
		//int voi[6] = { 100,200,200,300,55,55 };
		//ls->SetVOI(voi);
		ls->SetGenerateValues(1, 60, 60);
		ls->SetVesselWallContourRepresentation(this->m_vesselWallContourRepresentation);
		ls->Update();
		m_lumenWallContourWidget->Initialize(ls->GetOutput());


		imageViewer->Render();
	}
}

void InteractorStylePolygonDraw::FillPolygon()
{
	////Get Main window
	//MainWindow* mainWnd = MainWindow::GetMainWindow();

	//if (m_vesselWallContourRepresentation == NULL) return;
	//if (m_vesselWallContourWidget == NULL) return;

	////Get most updated current value
	////mainWnd->GetCursorPosition(m_currentPos);

	//m_vesselWallContourWidget->CloseLoop();

	//vtkPolyData* polydata = m_vesselWallContourRepresentation->GetContourRepresentationAsPolyData();

	//// Check if contour is drawn
	//if (polydata->GetNumberOfPoints() == 0)
	//	return;

	//vtkSmartPointer<vtkPolygon> polygon = vtkSmartPointer<vtkPolygon>::New();
	//int numOfPoints = polydata->GetNumberOfPoints();

	//// Get the coordinates of the contour data points

	//double lastPoint[3] = { VTK_DOUBLE_MAX, VTK_DOUBLE_MAX, VTK_DOUBLE_MAX };
	//for (vtkIdType i = 0; i < numOfPoints; i++)
	//{
	//	double pictureCoordinate[3];
	//	double worldCoordinate[3];
	//	polydata->GetPoint(i, pictureCoordinate);

	//	//Take one image data 1 to be reference
	//	worldCoordinate[0] = (pictureCoordinate[0] - origin[0]) / spacing[0];
	//	worldCoordinate[1] = (pictureCoordinate[1] - origin[1]) / spacing[1];
	//	worldCoordinate[2] = (pictureCoordinate[2] - origin[2]) / spacing[2];
	//	//cout << s[0] << " " << s[1] << " " << s[2] << endl;
	//	//Test whether the points are inside the polygon or not
	//	// if the points is too close to the previous point, skip it to avoid error in PointInPolygon algorithm
	//	double d = vtkMath::Distance2BetweenPoints(lastPoint, worldCoordinate);
	//	if (d < 1E-5)
	//		continue;
	//	memcpy(lastPoint, worldCoordinate, sizeof(double) * 3);
	//	switch (orientation)
	//	{
	//	case 0:
	//		polygon->GetPoints()->InsertNextPoint(0.0, worldCoordinate[1], worldCoordinate[2]);
	//		break;
	//	case 1:
	//		polygon->GetPoints()->InsertNextPoint(worldCoordinate[0], 0.0, worldCoordinate[2]);
	//		break;
	//	case 2:
	//		polygon->GetPoints()->InsertNextPoint(worldCoordinate[0], worldCoordinate[1], 0.0);
	//		break;
	//	default:
	//		break;
	//	}
	//}
	////Test whether the points are inside the polygon or not
	//double n[3];
	//polygon->ComputeNormal(polygon->GetPoints()->GetNumberOfPoints(),
	//	static_cast<double*>(polygon->GetPoints()->GetData()->GetVoidPointer(0)), n);
	//double bounds[6];
	//int bounds_int[6];

	//polygon->GetPoints()->GetBounds(bounds);

	//bounds_int[0] = ceil(bounds[0]);
	//bounds_int[1] = floor(bounds[1]);
	//bounds_int[2] = ceil(bounds[2]);
	//bounds_int[3] = floor(bounds[3]);
	//bounds_int[4] = ceil(bounds[4]);
	//bounds_int[5] = floor(bounds[5]);


	//double p[3];
	//int p2[3];
	//if (orientation == 0)
	//{
	//	for (int y = bounds_int[2]; y < bounds_int[3]; y++) {
	//		for (int z = bounds_int[4]; z < bounds_int[5]; z++) {
	//			p[0] = 0.0;
	//			p[1] = (double)y;
	//			p[2] = (double)z;
	//			p2[0] = imageViewer->GetSlice();
	//			p2[1] = y;
	//			p2[2] = z;
	//			if (polygon->PointInPolygon(p, polygon->GetPoints()->GetNumberOfPoints(),
	//				static_cast<double*>(polygon->GetPoints()->GetData()->GetVoidPointer(0)), bounds, n)) {
	//				// fill the contour
	//				mainWnd->GetOverlay()->SetPixel(p2, (double)label);
	//			}
	//		}
	//	}
	//}
	//else if (orientation == 1)
	//{
	//	for (int x = bounds_int[0]; x < bounds_int[1]; x++) {
	//		for (int z = bounds_int[4]; z < bounds_int[5]; z++) {
	//			p[0] = (double)x;
	//			p[1] = 0.0;
	//			p[2] = (double)z;
	//			p2[0] = x;
	//			p2[1] = imageViewer->GetSlice();
	//			p2[2] = z;
	//			if (polygon->PointInPolygon(p, polygon->GetPoints()->GetNumberOfPoints(),
	//				static_cast<double*>(polygon->GetPoints()->GetData()->GetVoidPointer(0)), bounds, n)) {
	//				// fill the contour
	//				mainWnd->GetOverlay()->SetPixel(p2, (double)label);
	//			}
	//		}
	//	}
	//}
	//else if (orientation == 2)
	//{
	//	for (int x = bounds_int[0]; x < bounds_int[1]; x++) {
	//		for (int y = bounds_int[2]; y < bounds_int[3]; y++) {
	//			p[0] = (double)x;
	//			p[1] = (double)y;
	//			p[2] = 0.0;
	//			p2[0] = x;
	//			p2[1] = y;
	//			p2[2] = imageViewer->GetSlice();
	//			if (polygon->PointInPolygon(p, polygon->GetPoints()->GetNumberOfPoints(),
	//				static_cast<double*>(polygon->GetPoints()->GetData()->GetVoidPointer(0)), bounds, n)) {
	//				// fill the contour
	//				mainWnd->GetOverlay()->SetPixel(p2, (double)label);
	//			}

	//		}
	//	}
	//}

	//SetPolygonModeEnabled(false);
	//SetPolygonModeEnabled(true);
	//mainWnd->GetOverlay()->GetOutput()->Modified();
}
//
//void InteractorStylePolygonDraw::NewContourWidget()
//{
//		vtkContourWidget* m_vesselWallContourWidget = vtkContourWidget::New();
//		m_vesselWallContourWidget->SetInteractor(this->Interactor);
//		m_vesselWallContourWidget->SetDefaultRenderer(imageViewer->GetRenderer());
//
//		vtkOrientedGlyphContourRepresentation* m_vesselWallContourRepresentation = vtkOrientedGlyphContourRepresentation::New();
//		m_vesselWallContourRepresentation->SetRenderer(imageViewer->GetRenderer());
//		m_vesselWallContourRepresentation->SetNeedToRender(true);
//		m_vesselWallContourRepresentation->GetLinesProperty()->SetColor(255, 255, 0);
//		m_vesselWallContourRepresentation->SetLineInterpolator(NULL);
//		m_vesselWallContourRepresentation->AlwaysOnTopOn();
//
//
//		//vtkPolyData* cursorpolyData = m_vesselWallContourRepresentation->GetActiveCursorShape();
//		//vtkSmartPointer<vtkTransform> translation = vtkSmartPointer<vtkTransform>::New();
//		//if (orientation == 0) {
//		//	translation->RotateX(90);
//		//	translation->RotateZ(90);
//		//}
//		//else if (orientation == 1) {
//		//	translation->RotateX(90);
//		//	translation->RotateY(90);
//		//}
//
//		//vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter = 
//		//	vtkSmartPointer<vtkTransformPolyDataFilter>::New();
//		//transformFilter->SetInputData(cursorpolyData);
//		//transformFilter->SetTransform(translation);
//		//transformFilter->Update();
//
//		//cursorpolyData->DeepCopy(transformFilter->GetOutput());
//		m_vesselWallContourRepresentation->BuildRepresentation();
//		m_vesselWallContourWidget->SetRepresentation(m_vesselWallContourRepresentation);
//		m_vesselWallContourWidget->FollowCursorOn();
//		m_vesselWallContourWidget->ContinuousDrawOn();
//		//m_vesselWallContourWidget->CreateDefaultRepresentation();
//		m_vesselWallContourWidget->On();
//		m_vesselWallContourWidget->EnabledOn();
//
//		this->imageViewer->Render();
//		this->CONTOUR_IS_ON_FLAG = true;
//}
//
