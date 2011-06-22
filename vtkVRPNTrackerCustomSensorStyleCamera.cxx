/*=========================================================================

  Name:        vtkVRPNTrackerCustomSensorStyleCamera.cxx

  Author:      Alexis YL Chan

  Copyright:   The Renaissance Computing Institute (RENCI)

  License:     Licensed under the RENCI Open Source Software License v. 1.0.
               
               See included License.txt or 
               http://www.renci.org/resources/open-source-software-license
               for details.

=========================================================================*/

#include "vtkVRPNTrackerCustomSensorStyleCamera.h"

#include "vtkCamera.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"
#include "vtkRenderWindow.h"

vtkStandardNewMacro(vtkVRPNTrackerCustomSensorStyleCamera);
vtkCxxRevisionMacro(vtkVRPNTrackerCustomSensorStyleCamera, "$Revision: 1.0 $");

//----------------------------------------------------------------------------
vtkVRPNTrackerCustomSensorStyleCamera::vtkVRPNTrackerCustomSensorStyleCamera() 
{ 
}

//----------------------------------------------------------------------------
vtkVRPNTrackerCustomSensorStyleCamera::~vtkVRPNTrackerCustomSensorStyleCamera() 
{
}

//----------------------------------------------------------------------------
void vtkVRPNTrackerCustomSensorStyleCamera::OnEvent(vtkObject* caller, unsigned long eid, void* callData) 
{
  vtkVRPNTrackerCustomSensor* tracker = static_cast<vtkVRPNTrackerCustomSensor*>(caller);

  switch(eid)
    {
    case vtkVRPNDevice::TrackerEvent:
      this->OnTracker(tracker);
      break;
    }
}

//----------------------------------------------------------------------------
void vtkVRPNTrackerCustomSensorStyleCamera::SetTracker(vtkVRPNTrackerCustomSensor* tracker)
{  
  if (tracker != NULL) 
    {
    tracker->AddObserver(vtkVRPNDevice::TrackerEvent, this->DeviceCallback);
    }
} 

//----------------------------------------------------------------------------
void vtkVRPNTrackerCustomSensorStyleCamera::OnTracker(vtkVRPNTrackerCustomSensor* tracker)
{
  vtkCamera* camera = this->Renderer->GetActiveCamera();

  // Get the rotation matrix
  double rotMat[3][3];
  vtkMath::QuaternionToMatrix3x3(tracker->GetRotation(), rotMat);
  camera->SetHeadPose(rotMat[0][0], rotMat[0][1],rotMat[0][2], tracker->GetPosition()[0]*1,
                          rotMat[1][0], rotMat[1][1],rotMat[1][2], tracker->GetPosition()[1]*1,
                          rotMat[2][0], rotMat[2][1],rotMat[2][2], tracker->GetPosition()[2]*1,
                          0.0, 0.0, 0.0, 1.0 );
  
  //// Calculate the view direction
  //double forward[3] = { 0.0, 0.0, 1.0 };
  //vtkMath::Multiply3x3(matrix, forward, forward);
  //for (int i = 0; i < 3; i++) forward[i] += tracker->GetPosition()[i];

  //// Calculate the up vector
  //double up[3] = { 0.0, 1.0, 0.0 };
  //vtkMath::Multiply3x3(matrix, up, up);

  //// Set camera parameters
  //camera->SetPosition(tracker->GetPosition());
  //camera->SetFocalPoint(forward);
  //camera->SetViewUp(up);
  //camera->Modified();

  // Render
  //this->Renderer->ResetCameraClippingRange();
  // Render() will be called in the interactor
}

////----------------------------------------------------------------------------
//void vtkVRPNTrackerCustomSensorStyleCamera::PrintSelf(ostream& os, vtkIndent indent)
//{
//  this->Superclass::PrintSelf(os,indent);
//}