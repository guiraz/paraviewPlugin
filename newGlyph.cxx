#include "newGlyph.h"

#include "vtkCellData.h"
#include "vtkCell.h"
#include "vtkCellArray.h"
#include "vtkDataSet.h"
#include "vtkFloatArray.h"
#include "vtkIdList.h"
#include "vtkIdTypeArray.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"
#include "vtkSmartPointer.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkTransform.h"
#include "vtkUnsignedCharArray.h"
#include "vtkDoubleArray.h"

vtkStandardNewMacro(newGlyph);
//----------------------------------------------------------------------------
// Construct object with scaling on, scaling mode is by scalar value,
// scale factor = 1.0, the range is (0,1), orient geometry is on, and
// orientation is by vector. Clamping and indexing are turned off. No
// initial sources are defined.
newGlyph::newGlyph()
{
    this->DebugOn();
    this->SetNumberOfInputPorts(1);
}

//----------------------------------------------------------------------------
newGlyph::~newGlyph()
{
}

//----------------------------------------------------------------------------
int newGlyph::RequestData(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector)
{
    // get the info objects
    vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
    vtkInformation *outInfo = outputVector->GetInformationObject(0);

    // get the input and output
    vtkPolyData *input = vtkPolyData::SafeDownCast(inInfo->Get(vtkDataObject::DATA_OBJECT()));
    vtkPolyData *output = vtkPolyData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));

    // Null input check
    if (!input)
      return 0;

    // Get the point data
    vtkPointData *inputPD = input->GetPointData();
    vtkPointData *outputPD = output->GetPointData();

    // Check we have a scalar point data with the required name
    if (input->GetPointData()->SetActiveAttribute("IntegrationTime", 0) == -1)
    {
      vtkErrorMacro("No point scalar field named \"IntegrationTime\"");
      return 0;
    }

    // Check we have lines as input
    vtkCellArray *streams = input->GetLines();
    if (streams == NULL) {
      vtkErrorMacro(<< "Invalid dataset: no lines");
      return 0;
    }
    streams->InitTraversal();

    // Get IntegrationTime array
    vtkDoubleArray *IT = vtkDoubleArray::SafeDownCast(inputPD->GetScalars());

    //The array of points that will be returned
    vtkPoints * points = vtkPoints::New();
    output->SetPoints(points);

    vtkDebugMacro(<<"nb lignes : "<<streams->GetNumberOfCells());

    //Processing streamlines
    vtkIdList * map = vtkIdList::New();
    while(streams->GetNextCell(map)!=0)
    {
        vtkDebugMacro(<<"ligne : "<<streams->GetTraversalLocation()+1);
        //Get the first point of the stream and insert in the returned points array
        int pointID = map->GetId(0);
        double * x = inputPD->GetArray(pointID)->GetTuple(0);
        //points->InsertNextPoint(x[0],x[1],x[2]);
    }

    return 1;
}

//----------------------------------------------------------------------------
void newGlyph::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
