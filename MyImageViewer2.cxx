#include "MyImageViewer2.h"

#include <vtkCamera.h>
#include <vtkRenderWindow.h>
#include <vtkImageMapper3D.h>
#include <vtkStreamingDemandDrivenPipeline.h>
#include <vtkInformation.h>
#include <vtkTextProperty.h>
#include <vtkImageProperty.h>

vtkStandardNewMacro(MyImageViewer2);

void MyImageViewer2::PrintSelf(ostream & os, vtkIndent indent)
{
	vtkImageViewer2::PrintSelf(os, indent);
}

void MyImageViewer2::InitializeHeader(QString File)
{
	int* size = Renderer->GetSize();
	double margin = 15;
	int coord[2] = { 5,size[1] - (int)margin };

	if (HeaderActor != NULL) {
		Renderer->RemoveActor2D(HeaderActor);
		//HeaderActor->Delete();
	}
	HeaderActor->GetTextProperty()->SetFontSize(15);
	HeaderActor->GetTextProperty()->SetColor(1.0, 0.7490, 0.0);
	Renderer->AddActor2D(HeaderActor);

	HeaderActor->SetDisplayPosition(coord[0], coord[1]);


	QByteArray Fileba = File.toLatin1();
	const char *Filestr = Fileba.data();


	if (GetInput() != NULL)
		HeaderActor->SetInput(Filestr);
	else
		cout << "Error in setting text, file not found" << endl;
}

void MyImageViewer2::InitializeIntensityText(QString IntText)
{
	int* size = Renderer->GetSize();
	//double margin = 15;
	int coord[2] = { 5,5 };

	if (this->FirstRender) {
		IntTextActor->GetTextProperty()->SetFontSize(15);
		IntTextActor->GetTextProperty()->SetColor(1.0, 0.7490, 0.0);
		Renderer->AddActor2D(IntTextActor);
		IntTextActor->SetDisplayPosition(coord[0], coord[1]);
		return;
	}

	QByteArray IntTextba = IntText.toLatin1();
	const char *IntTextstr = IntTextba.data();


	if (GetInput() != NULL)
		IntTextActor->SetInput(IntTextstr);
	else
		cout << "Error in setting text, file not found" << endl;
}

void MyImageViewer2::InitializeOrientationText()
{
	int* size = Renderer->GetSize();
	double margin = 15;

	int down[2] = { size[0] / 2	,5 };
	int up[2] = { size[0] / 2	,size[1] - (int)margin };
	int left[2] = { 5			,size[1] / 2 };
	int right[2] = { size[0] - (int)margin	,size[1] / 2 };

	for (int i = 0; i<4; i++)
	{
		if (OrientationTextActor[i] != NULL) {
			OrientationTextActor[i]->GetTextProperty()->SetFontSize(15);
			OrientationTextActor[i]->GetTextProperty()->SetColor(1.0, 0.7490, 0.0);
			Renderer->AddActor2D(OrientationTextActor[i]);
		}

	}

	OrientationTextActor[0]->SetDisplayPosition(up[0], up[1]);
	OrientationTextActor[1]->SetDisplayPosition(down[0], down[1]);
	OrientationTextActor[2]->SetDisplayPosition(left[0], left[1]);
	OrientationTextActor[3]->SetDisplayPosition(right[0], right[1]);

	switch (SliceOrientation)
	{
	case 0:
		OrientationTextActor[0]->SetInput("S");
		OrientationTextActor[1]->SetInput("I");
		OrientationTextActor[2]->SetInput("A");
		OrientationTextActor[3]->SetInput("P");
		break;
	case 1:
		OrientationTextActor[0]->SetInput("S");
		OrientationTextActor[1]->SetInput("I");
		OrientationTextActor[2]->SetInput("R");
		OrientationTextActor[3]->SetInput("L");
		break;
	case 2:
		OrientationTextActor[0]->SetInput("A");
		OrientationTextActor[1]->SetInput("P");
		OrientationTextActor[2]->SetInput("R");
		OrientationTextActor[3]->SetInput("L");
		break;
	}
}

void MyImageViewer2::ResizeOrientationText()
{
	int* size = Renderer->GetSize();

	double margin = 15;
	int coord[2] = { 5,size[1] - (int)margin };
	int down[2] = { size[0] / 2	,5 };
	int up[2] = { size[0] / 2	,size[1] - (int)margin };
	int left[2] = { 5			,size[1] / 2 };
	int right[2] = { size[0] - (int)margin	,size[1] / 2 };

	OrientationTextActor[0]->SetDisplayPosition(up[0], up[1]);
	OrientationTextActor[1]->SetDisplayPosition(down[0], down[1]);
	OrientationTextActor[2]->SetDisplayPosition(left[0], left[1]);
	OrientationTextActor[3]->SetDisplayPosition(right[0], right[1]);
	HeaderActor->SetDisplayPosition(coord[0], coord[1]);
}

void MyImageViewer2::SetCursorBoundary()
{
	double spacing[3];
	double origin[3];
	int extent[6];
	vtkImageData* image = vtkImageData::SafeDownCast(this->WindowLevel->GetInput());
	image->GetSpacing(spacing);
	image->GetOrigin(origin);
	image->GetExtent(extent);


	Bound[0] = origin[0] + extent[0] * spacing[0];
	Bound[1] = origin[0] + extent[1] * spacing[0];
	Bound[2] = origin[1] + extent[2] * spacing[1];
	Bound[3] = origin[1] + extent[3] * spacing[1];
	Bound[4] = origin[2] + extent[4] * spacing[2];
	Bound[5] = origin[2] + extent[5] * spacing[2];

	Cursor3D->SetTranslationMode(false);
	Cursor3D->SetModelBounds(Bound);
	Cursor3D->Update();
}

void MyImageViewer2::SetBound(double * b)
{
	for (int i = 0; i < 6; i++)
	{
		Bound[i] = b[i];
	}
	//Cursor
	this->SetCursorBoundary();
}

void MyImageViewer2::SetFocalPoint(double x, double y, double z)
{
	Cursor3D->SetFocalPoint(x, y, z);
	Cursor3D->Update();
	//SliceImplicitPlane->SetOrigin(x, y, z);
}

double * MyImageViewer2::GetBound()
{
	return Bound;
}

double * MyImageViewer2::GetFocalPoint()
{
	return Cursor3D->GetFocalPoint();
}

void MyImageViewer2::Render(void)
{

	if (this->FirstRender)
	{
		// Initialize Text
		this->InitializeOrientationText();
		this->InitializeIntensityText("");
		// Initialize the size if not set yet

		vtkAlgorithm *input = this->GetInputAlgorithm();
		if (input)
		{
			input->UpdateInformation();
			int *w_ext = this->GetInputInformation()->Get(
				vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT());
			int xs = 0, ys = 0;
			// not right for now
			// paintBrush ???
			switch (this->SliceOrientation)
			{
			case vtkImageViewer2::SLICE_ORIENTATION_XY:
			default:
				xs = w_ext[1] - w_ext[0] + 1;
				ys = w_ext[3] - w_ext[2] + 1;
				this->Renderer->GetActiveCamera()->Roll(180);
				this->Renderer->GetActiveCamera()->Azimuth(180);
				break;

			case vtkImageViewer2::SLICE_ORIENTATION_XZ:
				xs = w_ext[1] - w_ext[0] + 1;
				ys = w_ext[5] - w_ext[4] + 1;
				this->Renderer->GetActiveCamera()->Azimuth(180);
				this->Renderer->GetActiveCamera()->Elevation(180);
				break;

			case vtkImageViewer2::SLICE_ORIENTATION_YZ:
				xs = w_ext[3] - w_ext[2] + 1;
				ys = w_ext[5] - w_ext[4] + 1;
				//this->Renderer->GetActiveCamera()->Elevation(180);
				//this->Renderer->GetActiveCamera()->Roll(180);
				break;
			}

			// if it would be smaller than 150 by 100 then limit to 150 by 100
			if (this->RenderWindow->GetSize()[0] == 0)
			{
				this->RenderWindow->SetSize(
					xs < 150 ? 150 : xs, ys < 100 ? 100 : ys);
			}

			if (this->Renderer)
			{
				this->Renderer->ResetCamera();
				this->Renderer->GetActiveCamera()->SetParallelScale(
					xs < 150 ? 75 : (xs - 1) / 2.0);
			}
			this->FirstRender = 0;
		}
	}
	if (this->GetInput())
	{
		this->RenderWindow->Render();
	}
	if (this->AnnotationRenderer) {
		this->AnnotationRenderer->SetActiveCamera(this->Renderer->GetActiveCamera());
	}
	if (this->GetInput()) {
		RenderWindow->Render();
	}
}

void MyImageViewer2::SetInputData(vtkImageData* in)
{
	vtkImageViewer2::SetInputData(in);

	//Color Map
	double* range = in->GetScalarRange();
	this->SetColorWindow(range[1] - range[0]);
	this->SetColorLevel((range[1] + range[0])*0.5);
	//DefaultWindowLevel[0] = this->GetColorWindow();
	//DefaultWindowLevel[1] = this->GetColorLevel();

	//Cursor
	this->SetCursorBoundary();
	//AnnotationImage->SetSpacing(in->GetSpacing());
	//AnnotationImage->SetOrigin(in->GetOrigin());
	//AnnotationImage->SetExtent(in->GetExtent());
	//AnnotationImage->AllocateScalars(VTK_DOUBLE, 1);
	//this->AnnotationWindowLevel->SetInputData(AnnotationImage);

}

void MyImageViewer2::SetAnnotationImage(vtkImageData * in)
{
	this->AnnotationWindowLevel->SetInputData(in);
	this->UpdateDisplayExtent();
	double* range = in->GetScalarRange();

	// Behaviour Setting
	this->AnnotationRenderer->SetActiveCamera(this->Renderer->GetActiveCamera());
	this->AnnotationRenderer->SetLayer(1);
	//this->AnnotationActor->SetVisibility(false);

	RenderWindow->SetNumberOfLayers(2);
}

vtkImageData * MyImageViewer2::GetAnnotationImage()
{
	return vtkImageData::SafeDownCast(this->AnnotationWindowLevel->GetInput());
}

vtkLookupTable * MyImageViewer2::GetLookupTable()
{
	return NULL;
	//return this->AnnotationActor->GetProperty()->GetLookupTable();
}

void MyImageViewer2::SetLookupTable(vtkLookupTable * lookupTable)
{
	this->AnnotationActor->GetProperty()->SetLookupTable(lookupTable);
	double* range = lookupTable->GetTableRange();
	this->AnnotationWindowLevel->SetWindow(range[1] - range[0]);
	this->AnnotationWindowLevel->SetLevel((range[1] + range[0]) * 0.5);

}
//void MyImageViewer2::ResetAnnotationImage(vtkImageData * AnnotationImage)
//{
//	if (this->AnnotationImage != NULL) {
//		this->AnnotationImage->Delete();
//
//	}
//	this->AnnotationImage = AnnotationImage;
//	this->AnnotationWindowLevel->SetInputData(this->AnnotationImage);
//}

void MyImageViewer2::UpdateDisplayExtent()
{
	vtkImageViewer2::UpdateDisplayExtent();
	vtkAlgorithm *input = this->GetInputAlgorithm();
	vtkInformation* outInfo = input->GetOutputInformation(0);
	int *w_ext = outInfo->Get(
		vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT());
	if (this->AnnotationWindowLevel->GetOutput())
	{
		switch (this->SliceOrientation)
		{
		case MyImageViewer2::SLICE_ORIENTATION_XY:
			this->AnnotationActor->SetDisplayExtent(
				w_ext[0], w_ext[1], w_ext[2], w_ext[3], this->Slice, this->Slice);
			break;

		case MyImageViewer2::SLICE_ORIENTATION_XZ:
			this->AnnotationActor->SetDisplayExtent(
				w_ext[0], w_ext[1], this->Slice, this->Slice, w_ext[4], w_ext[5]);
			break;

		case MyImageViewer2::SLICE_ORIENTATION_YZ:
			this->AnnotationActor->SetDisplayExtent(
				this->Slice, this->Slice, w_ext[2], w_ext[3], w_ext[4], w_ext[5]);
			break;
		}
	}

}

MyImageViewer2::MyImageViewer2()
	:vtkImageViewer2()
{
	this->AnnotationRenderer = NULL;
	this->AnnotationActor = vtkImageActor::New();
	this->AnnotationWindowLevel = vtkImageMapToWindowLevelColors::New();
	this->CursorActor = NULL;

	//Setup the pipeline again because of Annotation
	this->UnInstallPipeline();

	vtkRenderer *ren = vtkRenderer::New();
	this->SetAnnotationRenderer(ren);
	ren->Delete();

	//AnnotationImage = vtkImageData::New();
	
	// Text and Cursor
	// Text
	this->IntTextActor = vtkTextActor::New();
	this->HeaderActor = vtkTextActor::New();
	for (int i = 0; i < 4; i++)
	{
		OrientationTextActor[i] = vtkTextActor::New();
	}

	//Cursor
	Cursor3D = vtkCursor3D::New();
	Cursor3D->AllOff();
	Cursor3D->AxesOn();
	Cursor3D->SetModelBounds(0, 0, 0, 0, 0, 0);
	Cursor3D->Update();

	CursorMapper = vtkPolyDataMapper::New();
	CursorMapper->SetInputData(Cursor3D->GetOutput());

	CursorActor = vtkActor::New();
	CursorActor->SetMapper(CursorMapper);
	CursorActor->GetProperty()->SetColor(0, 0, 1);
	CursorActor->GetProperty()->SetLineStipplePattern(0xf0f0);

	this->InstallPipeline();

}

MyImageViewer2::~MyImageViewer2()
{
	if (this->AnnotationWindowLevel) {
		this->AnnotationWindowLevel->Delete();
		this->AnnotationWindowLevel = NULL;
	}
	if (this->AnnotationActor) {
		this->AnnotationActor->Delete();
		this->AnnotationActor = NULL;
	}
	if (this->AnnotationRenderer) {
		this->AnnotationRenderer->Delete();
		this->AnnotationRenderer = NULL;
	}
	//if (AnnotationImage != NULL) {
	//	AnnotationImage->Delete();
	//	AnnotationImage = NULL;
	//}
}

void MyImageViewer2::InstallPipeline()
{
	vtkImageViewer2::InstallPipeline();
	if (this->RenderWindow && this->AnnotationRenderer) {
		this->RenderWindow->AddRenderer(this->AnnotationRenderer);
	}

	if (this->AnnotationRenderer && this->AnnotationActor)
	{
		this->AnnotationRenderer->AddViewProp(this->AnnotationActor);
	}

	if (this->AnnotationActor && this->AnnotationWindowLevel)
	{
		this->AnnotationActor->GetMapper()->SetInputConnection(
			this->AnnotationWindowLevel->GetOutputPort());
	}
	if (this->Renderer && this->CursorActor)
	{
		this->Renderer->AddActor(this->CursorActor);
	}
}

void MyImageViewer2::UnInstallPipeline()
{
	vtkImageViewer2::UnInstallPipeline();
	if (this->AnnotationActor)
	{
		this->AnnotationActor->GetMapper()->SetInputConnection(NULL);
	}

	if (this->AnnotationRenderer && this->AnnotationActor)
	{
		this->AnnotationRenderer->RemoveViewProp(this->AnnotationActor);
	}

	if (this->RenderWindow && this->AnnotationRenderer)
	{
		this->RenderWindow->RemoveRenderer(this->AnnotationRenderer);
	}
	if (this->Renderer && this->CursorActor)
	{
		this->Renderer->RemoveActor(this->CursorActor);
	}
}

void MyImageViewer2::SetAnnotationRenderer(vtkRenderer * arg)
{
	if (this->AnnotationRenderer == arg) {
		return;
	}
	this->UnInstallPipeline();

	if (this->AnnotationRenderer)
	{
		this->AnnotationRenderer->UnRegister(this);
	}

	this->AnnotationRenderer = arg;

	if (this->AnnotationRenderer)
	{
		this->AnnotationRenderer->Register(this);
	}

	this->InstallPipeline();
	this->UpdateOrientation();
}

void MyImageViewer2::SetupInteractor(vtkRenderWindowInteractor * arg)
{
	vtkImageViewer2::SetupInteractor(arg);
	if (this->AnnotationRenderer) {
		this->AnnotationRenderer->SetActiveCamera(this->Renderer->GetActiveCamera());
	}
}
