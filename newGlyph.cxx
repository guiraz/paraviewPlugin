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
#include "vtkVersion.h"

vtkStandardNewMacro(newGlyph);
//----------------------------------------------------------------------------
// Construct object with scaling on, scaling mode is by scalar value,
// scale factor = 1.0, the range is (0,1), orient geometry is on, and
// orientation is by vector. Clamping and indexing are turned off. No
// initial sources are defined.
newGlyph::newGlyph()
{
    //this->DebugOn();
    this->SetNumberOfInputPorts(1);
    this->CurrentIntegrationTime=0.0;
}

//----------------------------------------------------------------------------
newGlyph::~newGlyph()
{
}

//----------------------------------------------------------------------------

//VTK Version : 5.9.0
int newGlyph::RequestData(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector)
{
    /***********************************************************************************/

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

    // Check we have a scalar point data with the required name
    if (inputPD->SetActiveAttribute("IntegrationTime", 0) == -1) {
      vtkErrorMacro("No point scalar field named \"IntegrationTime\"");
      return 0;
    }

    /***********************************************************************************/

    // Check we have lines as input
    vtkCellArray *streams = input->GetLines();
    if (streams == NULL) {
      vtkErrorMacro(<< "Invalid dataset: no lines");
      return 0;
    }

    // Get IntegrationTime array
    vtkDoubleArray *IT = vtkDoubleArray::SafeDownCast(inputPD->GetScalars());

    /***********************************************************************************/

    //The array of points that will be returned
    vtkPoints * points = vtkPoints::New();
    output->SetPoints(points);

    //Scanning streamlines
    vtkIdType nbPoints;
    vtkIdType * pointsIds;

    streams->InitTraversal();
    while(streams->GetNextCell(nbPoints, pointsIds)!=0)
    {
        //Get the first point of the stream and insert in the returned points array
        vtkIdType pointID = UnderBoundPoint(nbPoints, pointsIds, IT);
        if(pointID!=-1)
        {
            vtkDebugMacro(<<"retour");
            double * x = input->GetPoint(pointID);
            points->InsertNextPoint(x);
            pointsIds = NULL;
        }
    }

    return 1;
}

//----------------------------------------------------------------------------
void newGlyph::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

vtkIdType newGlyph::UnderBoundPoint(vtkIdType nbPoints, vtkIdType* pointsIds, vtkDoubleArray* IT)
{
    double cit = this->CurrentIntegrationTime;
    vtkIdType id = 0;
    while(true)
    {
        vtkDebugMacro(<<"id : "<<id);
        if(id >= nbPoints)
            return -1;

        //Case around 0
        if(id==0)
        {
            //Forward
            if( (cit < IT->GetTuple1(pointsIds[1])) && (cit >= IT->GetTuple1(pointsIds[0])) )
                return pointsIds[0];

            //Backward
            if( (cit >= IT->GetTuple1(pointsIds[1])) && (cit < IT->GetTuple1(pointsIds[0])) )
                return pointsIds[1];
        }


        //Case around nbPoints
        if(id==nbPoints-1)
        {
            //Forward
            if( (cit < IT->GetTuple1(pointsIds[id])) && (cit >= IT->GetTuple1(pointsIds[id-1])) )
                return pointsIds[id-1];


            //Backward
            if( (cit >= IT->GetTuple1(pointsIds[id])) && (cit < IT->GetTuple1(pointsIds[id-1])) )
                return pointsIds[id];
        }
        else //Default case
        {
            //Forward
            if( (cit < IT->GetTuple1(pointsIds[id+1])) && (cit >= IT->GetTuple1(pointsIds[id])) )
                return pointsIds[id];

            //Backward
            if( (cit >= IT->GetTuple1(pointsIds[id+1])) && (cit < IT->GetTuple1(pointsIds[id])) )
                return pointsIds[id+1];
        }

        id++;
    }
}
