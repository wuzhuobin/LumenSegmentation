#include <vtkVersion.h>
#include <vtkSmartPointer.h>
#include <vtkProperty.h>
#include <vtkOutlineFilter.h>
#include <vtkCommand.h>
#include <vtkSliderWidget.h>
#include <vtkSliderRepresentation.h>
#include <vtkSliderRepresentation3D.h>
#include <vtkImageData.h>
#include <vtkCellArray.h>
#include <vtkFloatArray.h>
#include <vtkPointData.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkContourFilter.h>
#include <vtkXMLImageDataWriter.h>

///
#include <vtkNIFTIImageReader.h>
#include <vtkExtractVOI.h>
#include <vtkInteractorStyleImage.h>
#include "MyImageViewer2.h"
#include "MyImageViewer2.h"
#include "vtkImageViewer2.h"

static void CreateData(vtkImageData* data);

class vtkSliderCallback : public vtkCommand
{
public:
	static vtkSliderCallback *New()
	{
		return new vtkSliderCallback;
	}
	virtual void Execute(vtkObject *caller, unsigned long, void*)
	{
		vtkSliderWidget *sliderWidget =
			reinterpret_cast<vtkSliderWidget*>(caller);
		double value = static_cast<vtkSliderRepresentation *>(sliderWidget->GetRepresentation())->GetValue();
		this->ContourFilter->GenerateValues(1, value, value);

	}
	vtkSliderCallback() :ContourFilter(NULL) {}
	vtkContourFilter* ContourFilter;
};

int main(int, char *[])
{

	vtkSmartPointer<vtkNIFTIImageReader> niftiImageReader1 =
		vtkSmartPointer<vtkNIFTIImageReader>::New();
	niftiImageReader1->SetFileName(
		"C:\\Users\\jieji\\Desktop\\MACOSX_BUNDLE\\JackyData\\nifti_corrected\\CUBE T1 corrected.nii");
	niftiImageReader1->Update();

	vtkSmartPointer<vtkNIFTIImageReader> niftiImageReader2 =
		vtkSmartPointer<vtkNIFTIImageReader>::New();
	niftiImageReader2->SetFileName(
		"C:\\Users\\jieji\\Desktop\\MACOSX_BUNDLE\\JackyData\\nifti_corrected\\segmentation_right.nii");
	niftiImageReader2->Update();

	vtkSmartPointer<vtkLookupTable> lookupTable =
		vtkSmartPointer<vtkLookupTable>::New();
	lookupTable->SetNumberOfTableValues(7);
	lookupTable->SetTableRange(0.0, 6);
	lookupTable->SetTableValue(0, 0, 0, 0, 0);
	lookupTable->SetTableValue(1, 1, 0, 0, 1);
	lookupTable->SetTableValue(2, 0, 0, 1, 0.3);
	lookupTable->SetTableValue(3, 0, 1, 0, 0.5);
	lookupTable->SetTableValue(4, 1, 1, 0, 0.8);
	lookupTable->SetTableValue(5, 0, 1, 1, 0.9);
	lookupTable->SetTableValue(6, 1, 0, 1, 1);
	lookupTable->Build();

	vtkSmartPointer<vtkExtractVOI> extractVOI =
		vtkSmartPointer<vtkExtractVOI>::New();
	extractVOI->SetInputConnection(niftiImageReader1->GetOutputPort());
	int voi[6] = { 100,200,200,300,55,55 };
	//int voi[6] = {0,500,0,500,55,55};
	extractVOI->SetVOI(voi);
	extractVOI->Update();

	vtkSmartPointer<vtkImageData> data =
		vtkSmartPointer<vtkImageData>::New();
	data = extractVOI->GetOutput();

	// Create an isosurface
	vtkSmartPointer<vtkContourFilter> contourFilter =
		vtkSmartPointer<vtkContourFilter>::New();
#if VTK_MAJOR_VERSION <= 5
	contourFilter->SetInput(data);
#else
	contourFilter->SetInputData(data);
#endif
	contourFilter->GenerateValues(1, 53, 53); // (numContours, rangeStart, rangeEnd)

											  // Map the contours to graphical primitives
	vtkSmartPointer<vtkPolyDataMapper> contourMapper =
		vtkSmartPointer<vtkPolyDataMapper>::New();
	contourMapper->SetInputConnection(contourFilter->GetOutputPort());

	// Create an actor for the contours
	vtkSmartPointer<vtkActor> contourActor =
		vtkSmartPointer<vtkActor>::New();
	contourActor->SetMapper(contourMapper);

	// Visualize
	vtkSmartPointer<vtkRenderWindowInteractor> interactor =
		vtkSmartPointer<vtkRenderWindowInteractor>::New();

	MyImageViewer2* viewer2 = MyImageViewer2::New();
	viewer2->SetInputData(data);
	viewer2->SetAnnotationImage(niftiImageReader2->GetOutput());
	viewer2->SetLookupTable(lookupTable);
	viewer2->GetAnnotationActor()->SetVisibility(false);

	viewer2->GetRenderer()->AddActor(contourActor);
	viewer2->SetupInteractor(interactor);

	//vtkSmartPointer<vtkImageViewer2> viewer =
	//	vtkSmartPointer<vtkImageViewer2>::New();
	//viewer->SetInputData(data);
	//viewer->SetupInteractor(interactor);
	//viewer->GetRenderer()->AddActor(contourActor);

	vtkSmartPointer<vtkSliderRepresentation3D> sliderRep =
		vtkSmartPointer<vtkSliderRepresentation3D>::New();
	sliderRep->SetMinimumValue(0.0);
	sliderRep->SetMaximumValue(500.0);
	sliderRep->SetValue(10.0);
	sliderRep->SetTitleText("Contour value");
	sliderRep->SetPoint1InWorldCoordinates(-20, -40, 0);
	sliderRep->SetPoint2InWorldCoordinates(0, -40, 0);
	sliderRep->SetSliderWidth(.2);
	sliderRep->SetLabelHeight(.1);

	vtkSmartPointer<vtkSliderWidget> sliderWidget =
		vtkSmartPointer<vtkSliderWidget>::New();
	sliderWidget->SetInteractor(interactor);
	sliderWidget->SetRepresentation(sliderRep);
	sliderWidget->SetAnimationModeToAnimate();
	sliderWidget->EnabledOn();

	vtkSmartPointer<vtkSliderCallback> callback =
		vtkSmartPointer<vtkSliderCallback>::New();
	callback->ContourFilter = contourFilter;

	sliderWidget->AddObserver(vtkCommand::InteractionEvent, callback);

	vtkSmartPointer<vtkInteractorStyleImage> style =
		vtkSmartPointer<vtkInteractorStyleImage>::New();
	interactor->SetInteractorStyle(style);

	viewer2->Render();
	interactor->Start();

	return EXIT_SUCCESS;
}

void CreateData(vtkImageData* data)
{
	data->SetExtent(-25, 25, -25, 25, 0, 0);
#if VTK_MAJOR_VERSION <= 5
	data->SetNumberOfScalarComponents(1);
	data->SetScalarTypeToDouble();
#else
	data->AllocateScalars(VTK_DOUBLE, 1);
#endif
	int* extent = data->GetExtent();

	for (int y = extent[2]; y <= extent[3]; y++)
	{
		for (int x = extent[0]; x <= extent[1]; x++)
		{
			double* pixel = static_cast<double*>(data->GetScalarPointer(x, y, 0));
			pixel[0] = sqrt(pow(x, 2.0) + pow(y, 2.0));
		}
	}

	vtkSmartPointer<vtkXMLImageDataWriter> writer =
		vtkSmartPointer<vtkXMLImageDataWriter>::New();
	writer->SetFileName("data.vti");
#if VTK_MAJOR_VERSION <= 5
	writer->SetInputConnection(data->GetProducerPort());
#else
	writer->SetInputData(data);
#endif
	writer->Write();
}
