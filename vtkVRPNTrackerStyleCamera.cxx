/*=========================================================================

  Name:        vtkVRPNTrackerStyleCamera.cxx

  Author:      David Borland, The Renaissance Computing Institute (RENCI)

  Copyright:   The Renaissance Computing Institute (RENCI)

  License:     Licensed under the RENCI Open Source Software License v. 1.0.
               
               See included License.txt or 
               http://www.renci.org/resources/open-source-software-license
               for details.

=========================================================================*/

#include "vtkVRPNTrackerStyleCamera.h"

#include "vtkCamera.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"
#include "vtkRenderWindow.h"

vtkStandardNewMacro(vtkVRPNTrackerStyleCamera);
vtkCxxRevisionMacro(vtkVRPNTrackerStyleCamera, "$Revision: 1.0 $");

//----------------------------------------------------------------------------
vtkVRPNTrackerStyleCamera::vtkVRPNTrackerStyleCamera() 
{ 
}

//----------------------------------------------------------------------------
vtkVRPNTrackerStyleCamera::~vtkVRPNTrackerStyleCamera() 
{
}

//----------------------------------------------------------------------------
void vtkVRPNTrackerStyleCamera::OnEvent(vtkObject* caller, unsigned long eid, void* callData) 
{
  vtkVRPNTracker* tracker = static_cast<vtkVRPNTracker*>(caller);

  switch(eid)
    {
    case vtkVRPNDevice::TrackerEvent:
      this->OnTracker(tracker);
      break;
    }
}

//----------------------------------------------------------------------------
void vtkVRPNTrackerStyleCamera::SetTracker(vtkVRPNTracker* tracker)
{  
  if (tracker != NULL) 
    {
    tracker->AddObserver(vtkVRPNDevice::TrackerEvent, this->DeviceCallback);
    }
} 

//----------------------------------------------------------------------------
void vtkVRPNTrackerStyleCamera::OnTracker(vtkVRPNTracker* tracker)
{
  vtkCamera* camera = this->Renderer->GetActiveCamera();

  // Get the rotation matrix
  double matrix[3][3];
  vtkMath::QuaternionToMatrix3x3(tracker->GetRotation(), matrix);

  // Calculate the view direction
  double forward[3] = { 0.0, 0.0, 1.0 };
  vtkMath::Multiply3x3(matrix, forward, forward);
  for (int i = 0; i < 3; i++) forward[i] += tracker->GetPosition()[i];

  // Calculate the up vector
  double up[3] = { 0.0, 1.0, 0.0 };
  vtkMath::Multiply3x3(matrix, up, up);

  // Set camera parameters
  camera->SetPosition(tracker->GetPosition());
  camera->SetFocalPoint(forward);
  camera->SetViewUp(up);
  camera->Modified();

  // Render
  this->Renderer->ResetCameraClippingRange();
  // Render() will be called in the interactor
}

//----------------------------------------------------------------------------
void vtkVRPNTrackerStyleCamera::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}