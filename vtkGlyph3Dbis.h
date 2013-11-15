#ifndef __vtkGlyph3Dbis_h
#define __vtkGlyph3Dbis_h

#include "vtkPolyDataAlgorithm.h"

#define VTK_SCALE_BY_SCALAR 0
#define VTK_SCALE_BY_VECTOR 1
#define VTK_SCALE_BY_VECTORCOMPONENTS 2
#define VTK_DATA_SCALING_OFF 3

#define VTK_COLOR_BY_SCALE  0
#define VTK_COLOR_BY_SCALAR 1
#define VTK_COLOR_BY_VECTOR 2

#define VTK_USE_VECTOR 0
#define VTK_USE_NORMAL 1
#define VTK_VECTOR_ROTATION_OFF 2

#define VTK_INDEXING_OFF 0
#define VTK_INDEXING_BY_SCALAR 1
#define VTK_INDEXING_BY_VECTOR 2

class vtkTransform;

class VTK_GRAPHICS_EXPORT vtkGlyph3Dbis : public vtkPolyDataAlgorithm
{
public:
  vtkTypeMacro(vtkGlyph3Dbis,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description
  // Construct object with scaling on, scaling mode is by scalar value,
  // scale factor = 1.0, the range is (0,1), orient geometry is on, and
  // orientation is by vector. Clamping and indexing are turned off. No
  // initial sources are defined.
  static vtkGlyph3Dbis *New();

protected:
  vtkGlyph3Dbis();
  ~vtkGlyph3Dbis();

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);
};

#endif
