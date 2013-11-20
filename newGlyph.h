#ifndef __newGlyph_h
#define __newGlyph_h

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

class VTK_GRAPHICS_EXPORT newGlyph : public vtkPolyDataAlgorithm
{
public:
  vtkTypeMacro(newGlyph,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);
  static newGlyph *New();

protected:
  newGlyph();
  ~newGlyph();
  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);
};

#endif
