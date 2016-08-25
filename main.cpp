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
#include <vtkCallbackCommand.h>

///
#include <vtkNIFTIImageReader.h>
#include <vtkExtractVOI.h>
#include <vtkInteractorStyleImage.h>
#include "LumenSegmentation.h"
#include "MyImageViewer2.h"
#include "InteractorStylePolygonDraw.h"

#include <iostream>
using namespace std;

void KeypressCallbackFunction(vtkObject* caller, long unsigned int vtkNotUsed(eventId), void* vtkNotUsed(clientData), void* vtkNotUsed(callData))
{
	std::cout << "Keypress callback" << std::endl;

	vtkRenderWindowInteractor *iren =
		static_cast<vtkRenderWindowInteractor*>(caller);

	std::cout << "Pressed: " << iren->GetKeySym() << std::endl;
}

int main(int, char *[])
{
	int voi[6] = { 100,200,200,300,53,55 };
	//int voi[6] = { 0,500,0,500,55,55 };

	vtkSmartPointer<vtkNIFTIImageReader> niftiImageReader1 =
		vtkSmartPointer<vtkNIFTIImageReader>::New();
	niftiImageReader1->SetFileName(
		//"E:\\Andy\\blood_vessel_v_1.0.0\\Data\\JackyData\\nifti_corrected\\CUBE_T1_corrected.nii"
		"C:\\Users\\jieji\\Desktop\\MACOSX_BUNDLE\\JackyData\\nifti_corrected\\CUBE T1 corrected.nii"
	);
	niftiImageReader1->Update();

	vtkSmartPointer<vtkNIFTIImageReader> niftiImageReader2 =
		vtkSmartPointer<vtkNIFTIImageReader>::New();
	niftiImageReader2->SetFileName(
		//"E:\\Andy\\blood_vessel_v_1.0.0\\Data\\JackyData\\nifti_corrected\\segmentation_right.nii"
		"C:\\Users\\jieji\\Desktop\\MACOSX_BUNDLE\\JackyData\\nifti_corrected\\segmentation_right.nii"
	);


	vtkSmartPointer<vtkExtractVOI> extractVOI =
		vtkSmartPointer<vtkExtractVOI>::New();
	extractVOI->SetVOI(voi);
	extractVOI->SetInputConnection(niftiImageReader1->GetOutputPort());
	extractVOI->Update();

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



	//vtkSmartPointer<vtkCallbackCommand> keypressCallback =
	//	vtkSmartPointer<vtkCallbackCommand>::New();
	//keypressCallback->SetCallback(KeypressCallbackFunction);

	 //Visualize
	vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
		vtkSmartPointer<vtkRenderWindowInteractor>::New();
	//renderWindowInteractor->AddObserver(vtkCommand::KeyPressEvent, keypressCallback);

	MyImageViewer2* viewer2 = MyImageViewer2::New();
	viewer2->SetInputData(extractVOI->GetOutput());
	viewer2->SetAnnotationImage(niftiImageReader2->GetOutput());
	viewer2->GetAnnotationActor()->SetVisibility(false);
	viewer2->SetLookupTable(lookupTable);
	viewer2->SetupInteractor(renderWindowInteractor);
	viewer2->SetSize(1000, 1000);
	viewer2->Render();

	//vtkSmartPointer<vtkImageViewer2> viewer =
	//	vtkSmartPointer<vtkImageViewer2>::New();
	//viewer->SetInputData(niftiImageReader1->GetOutput());
	//viewer->SetupInteractor(renderWindowInteractor);
	//viewer->SetSize(1000, 1000);
	//viewer->Render();

	vtkSmartPointer<InteractorStylePolygonDraw> polygonDraw =
		vtkSmartPointer<InteractorStylePolygonDraw>::New();
	polygonDraw->SetImageViewer(viewer2);
	renderWindowInteractor->SetInteractorStyle(polygonDraw);
	




	// Original Visualize
	//vtkSmartPointer<vtkRenderer> renderer =
	//	vtkSmartPointer<vtkRenderer>::New();
	//vtkSmartPointer<vtkRenderWindow> renderWindow =
	//	vtkSmartPointer<vtkRenderWindow>::New();
	//renderWindow->AddRenderer(renderer);
	//interactor->SetRenderWindow(renderWindow);

	renderWindowInteractor->Start();

	return EXIT_SUCCESS;
}

