#ifndef MYIMAGEVIEWER2_H
#define MYIMAGEVIEWER2_H

#include <vtkImageViewer2.h>
#include <vtkObjectFactory.h>
#include <vtkRenderer.h>
#include <vtkImageActor.h>
#include <vtkImageMapToWindowLevelColors.h>
#include <vtkImageData.h>
#include <vtkTextActor.h>
#include <vtkCursor3D.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkLookupTable.h>

#include <qstring.h>

class MyImageViewer2: public vtkImageViewer2
{
public:
	static MyImageViewer2* New();
	vtkTypeMacro(MyImageViewer2, vtkImageViewer2);
	void PrintSelf(ostream& os, vtkIndent indent);

	// Text methods
	virtual void InitializeHeader(QString File);
	virtual void InitializeIntensityText(QString IntText);
	virtual void InitializeOrientationText();
	void ResizeOrientationText();

	// Cursor methods
	virtual void SetCursorBoundary();
	virtual void SetBound(double* b);
	virtual void SetFocalPoint(double x, double y, double z);
	virtual double* GetBound();
	virtual double* GetFocalPoint();

	// Description:
	// Render the resulting image.
	virtual void Render(void);

	// Description:
	// Set/Get the input image to the viewer.
	// the AnnotationImage will be set as the input
	virtual void SetInputData(vtkImageData* in);
	//virtual vtkImageData* GetInput();

	// Description:
	// Get the AnnotationImage
	virtual void SetAnnotationImage(vtkImageData* in);
	virtual vtkImageData* GetAnnotationImage();
	virtual vtkLookupTable* GetLookupTable();
	virtual void SetLookupTable(vtkLookupTable* lookupTable);

	//// Replace the old AnnotationImage
	//virtual void ResetAnnotationImage(vtkImageData* AnnotationImage);

	// Description:
	// Update the display extent manually so that the proper slice for the
	// given orientation is displayed. It will also try to set a
	// reasonable camera clipping range.
	// This method is called automatically when the Input is changed, but
	// most of the time the input of this class is likely to remain the same,
	// i.e. connected to the output of a filter, or an image reader. When the
	// input of this filter or reader itself is changed, an error message might
	// be displayed since the current display extent is probably outside
	// the new whole extent. Calling this method will ensure that the display
	// extent is reset properly.
	virtual void UpdateDisplayExtent();

	// Description:
	// Get the internal annotation renderer, annotation actor, and
	// annotation image map instances.
	vtkGetObjectMacro(AnnotationRenderer, vtkRenderer);
	vtkGetObjectMacro(AnnotationActor, vtkImageActor);
	vtkGetObjectMacro(AnnotationWindowLevel, vtkImageMapToWindowLevelColors);

	// Description:
	// Set your own Annotation Renderer
	virtual void SetAnnotationRenderer(vtkRenderer *arg);

	// Description:
	// Attach an interactor for the internal render window.
	virtual void SetupInteractor(vtkRenderWindowInteractor* arg);



protected:
	MyImageViewer2();
	~MyImageViewer2();

	virtual void InstallPipeline();
	virtual void UnInstallPipeline();

	// Annotation Things
	vtkImageMapToWindowLevelColors* AnnotationWindowLevel;
	vtkRenderer* AnnotationRenderer;
	vtkImageActor* AnnotationActor;

	// use an internal AnnotationImage pointer for doing paint brush
	//vtkImageData* AnnotationImage;




	//OrientationText
	vtkTextActor* OrientationTextActor[4];

	//Header
	vtkTextActor* HeaderActor;

	// IntensityText
	vtkTextActor* IntTextActor;

	//Cursor
	vtkCursor3D*		 Cursor3D;
	vtkPolyDataMapper* CursorMapper;
	vtkActor*			 CursorActor;

	//BackGround
	double Bound[6];


private:

	MyImageViewer2(const MyImageViewer2&);
	void operator=(const MyImageViewer2&);
};


#endif // !MYIMAGEVIEWER2_H