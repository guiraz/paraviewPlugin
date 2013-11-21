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

    //Scanning streamlines and get the point at the current step time
    vtkIdType nbPoints;
    vtkIdType * pointsIds;
    vtkDoubleArray * scalars = vtkDoubleArray::New();

    int i =0;
    streams->InitTraversal();
    while(streams->GetNextCell(nbPoints, pointsIds)!=0)
    {
        //Get the first point of the stream and insert in the returned points array
        vtkIdType * pointID = new vtkIdType[2];
        BoundPoints(nbPoints, pointsIds, IT, pointID);
        if(pointID[0]!=-1)
        {
            double *x = new double[3];
            WeightedAverage2Points(pointID, input, x);
            points->InsertNextPoint(x);
            scalars->InsertNextTuple1(this->CurrentIntegrationTime);
            pointsIds = NULL;
        }
        i++;
    }

    //Put scalars in the output
    output->GetPointData()->SetScalars(scalars);

    return 1;
}

//----------------------------------------------------------------------------
void newGlyph::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}


//Get the two points around the current time step
void newGlyph::BoundPoints(vtkIdType nbPoints, vtkIdType* pointsIds, vtkDoubleArray* IT, vtkIdType * result)
{
    result[0] = -1;
    result[1] = -1;
    double cit = this->CurrentIntegrationTime;
    vtkIdType id = 0;
    while(true)
    {
        if(id >= nbPoints)
            return;

        //Case around 0
        if(id==0)
        {
            //Forward
            if( (cit < IT->GetTuple1(pointsIds[1])) && (cit >= IT->GetTuple1(pointsIds[0])) )
            {
                result[0] = pointsIds[0];
                result[1] = pointsIds[1];
                return;
            }

            //Backward
            if( (cit >= IT->GetTuple1(pointsIds[1])) && (cit < IT->GetTuple1(pointsIds[0])) )
            {
                result[0] = pointsIds[1];
                result[1] = pointsIds[0];
                return;
            }
        }


        //Case around nbPoints
        if(id==nbPoints-1)
        {
            //Forward
            if( (cit < IT->GetTuple1(pointsIds[id])) && (cit >= IT->GetTuple1(pointsIds[id-1])) )
            {
                result[0] = pointsIds[id-1];
                result[1] = pointsIds[id];
                return;
            }


            //Backward
            if( (cit >= IT->GetTuple1(pointsIds[id])) && (cit < IT->GetTuple1(pointsIds[id-1])) )
            {
                result[0] = pointsIds[id];
                result[1] = pointsIds[id-1];
                return;
            }
        }
        else //Default case
        {
            //Forward
            if( (cit < IT->GetTuple1(pointsIds[id+1])) && (cit >= IT->GetTuple1(pointsIds[id])) )
            {
                result[0] = pointsIds[id];
                result[1] = pointsIds[id+1];
                return;
            }

            //Backward
            if( (cit >= IT->GetTuple1(pointsIds[id+1])) && (cit < IT->GetTuple1(pointsIds[id])) )
            {
                result[0] = pointsIds[id+1];
                result[1] = pointsIds[id];
                return;
            }
        }
        id++;
    }
}


//Get the coordonates of the point between the two points bounding the current time step
void newGlyph::WeightedAverage2Points(vtkIdType * AB, vtkPolyData* input, double * C)
{

    double *A = new double[3];
    input->GetPoint(AB[0], A);
    double *B = new double[3];
    input->GetPoint(AB[1], B);

    vtkPointData *inputPD = input->GetPointData();

    double tA = inputPD->GetScalars()->GetTuple1(AB[0]); //time at A
    double tB = inputPD->GetScalars()->GetTuple1(AB[1]); //time at B
    double tC = this->CurrentIntegrationTime;            //time at C = current time step

    double alpha = sqrt((tC-tA)*(tC-tA)) / sqrt((tB-tA)*(tB-tA));

    C[0] = alpha * B[0] - alpha * A[0] + A[0];
    C[1] = alpha * B[1] - alpha * A[1] + A[1];
    C[2] = alpha * B[2] - alpha * A[2] + A[2];

    /********************************************************

    vAB = (xb-xa), (yb-ya), (zb-za)

    alpha = sqrt((time_at_C - time_at_A)²) / sqrt((time_at_B - time_at_A)²)     -->     distance rate between A and C

    vAC = alpha * AB = (xc-xa), (yc-ya), (zc-za)

    C = (alpha * xb - alpha * xa + xa), (alpha * yb - alpha * ya + ya), (alpha * zb - alpha * za + za)

    ********************************************************/
}
