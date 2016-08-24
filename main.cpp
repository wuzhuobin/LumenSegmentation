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
#include "LumenSegmentation.h"
#include "MyImageViewer2.h"
#include "MyImageViewer2.h"
#include "vtkImageViewer2.h"

#include <iostream>
using namespace std;


int main(int, char *[])
{
	int voi[6] = { 100,200,200,300,55,55 };
	//int voi[6] = { 0,500,0,500,55,55 };

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

	vtkSmartPointer<LumenSegmentaiton> ls =
		vtkSmartPointer<LumenSegmentaiton>::New();
	ls->SetInputData(niftiImageReader1->GetOutput());
	ls->SetVOI(voi);
	ls->SetGenerateValues(1, 60, 60);
	ls->Update();
	

	vtkSmartPointer<vtkPolyDataMapper> contourMapper =
		vtkSmartPointer<vtkPolyDataMapper>::New();
	contourMapper->SetInputData(ls->contour);

	// Create an actor for the contours
	vtkSmartPointer<vtkActor> contourActor =
		vtkSmartPointer<vtkActor>::New();
	contourActor->SetMapper(contourMapper);

	 //Visualize
	vtkSmartPointer<vtkRenderWindowInteractor> interactor =
		vtkSmartPointer<vtkRenderWindowInteractor>::New();

	MyImageViewer2* viewer2 = MyImageViewer2::New();
	viewer2->SetInputData(ls->GetOutput());
	viewer2->SetAnnotationImage(niftiImageReader2->GetOutput());
	viewer2->SetLookupTable(lookupTable);
	viewer2->GetAnnotationActor()->SetVisibility(false);
	viewer2->GetRenderer()->AddActor(contourActor);
	viewer2->SetupInteractor(interactor);
	viewer2->Render();


	//vtkSmartPointer<vtkImageViewer2> viewer =
	//	vtkSmartPointer<vtkImageViewer2>::New();
	//viewer->SetInputData(data);
	//viewer->SetupInteractor(interactor);
	//viewer->GetRenderer()->AddActor(contourActor);
	//viewer->Render();

	// Original Visualize
	//vtkSmartPointer<vtkRenderer> renderer =
	//	vtkSmartPointer<vtkRenderer>::New();
	//vtkSmartPointer<vtkRenderWindow> renderWindow =
	//	vtkSmartPointer<vtkRenderWindow>::New();
	//renderWindow->AddRenderer(renderer);
	//interactor->SetRenderWindow(renderWindow);

	interactor->Start();

	return EXIT_SUCCESS;
}

