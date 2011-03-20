/*=========================================================================

  Name:        vtkSpaceNavigatorStyleCamera.cxx

  Author:      Alexis YL Chan

  Copyright:   The Renaissance Computing Institute (RENCI)

  License:     Licensed under the RENCI Open Source Software License v. 1.0.
               
               See included License.txt or 
               http://www.renci.org/resources/open-source-software-license
               for details.

=========================================================================*/

#include "vtkSpaceNavigatorStyleCamera.h"

#include "vtkCamera.h"
#include "vtkInteractorObserver.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"
#include "vtkRenderWindow.h"

vtkStandardNewMacro(vtkSpaceNavigatorStyleCamera);
vtkCxxRevisionMacro(vtkSpaceNavigatorStyleCamera, "$Revision: 1.0 $");

//----------------------------------------------------------------------------
vtkSpaceNavigatorStyleCamera::vtkSpaceNavigatorStyleCamera() 
{ 
  this->TranslateSensitivity = 1.0;
  this->RotateSensitivity = 1.0;

  this->XPos = 0.0;
  this->YPos = 0.0;
  this->ZPos = 0.0;

  this->XRotate = 0.0;
  this->YRotate = 0.0;
  this->ZRotate = 0.0;
}

//----------------------------------------------------------------------------
vtkSpaceNavigatorStyleCamera::~vtkSpaceNavigatorStyleCamera() 
{
}

//----------------------------------------------------------------------------
void vtkSpaceNavigatorStyleCamera::OnEvent(vtkObject* caller, unsigned long eid, void* callData) 
{
  // Need to initialize both here, as they cannot be initialized in the case statement
  vtkVRPNAnalog* analog = static_cast<vtkVRPNAnalog*>(caller);
  vtkVRPNButton* button = static_cast<vtkVRPNButton*>(caller);

  switch(eid)
    {
    case vtkVRPNDevice::AnalogEvent:
      this->OnAnalog(analog);
      break;

    case vtkVRPNDevice::ButtonEvent:
      this->OnButton(button);
      break;
    }
}

//----------------------------------------------------------------------------
void vtkSpaceNavigatorStyleCamera::OnAnalog(vtkVRPNAnalog* analog)
{
  vtkCamera* camera = this->Renderer->GetActiveCamera();

  this->XPos = analog->GetChannel(vtkSpaceNavigatorStyle::PositionX);
  this->YPos = analog->GetChannel(vtkSpaceNavigatorStyle::PositionY);
  this->ZPos = analog->GetChannel(vtkSpaceNavigatorStyle::PositionZ);
  this->XRotate = analog->GetChannel(vtkSpaceNavigatorStyle::RotationX);
  this->YRotate = analog->GetChannel(vtkSpaceNavigatorStyle::RotationY);
  this->ZRotate = analog->GetChannel(vtkSpaceNavigatorStyle::RotationZ);

}

//----------------------------------------------------------------------------
void vtkSpaceNavigatorStyleCamera::OnButton(vtkVRPNButton* button)
{
//  vtkCamera* camera = this->Renderer->GetActiveCamera();
//
//  // Reset
//  if (button->GetButton(vtkSpaceNavigatorStyle::ButtonHome))
//    {
//    camera->SetPosition(0.0, 0.0, 1.0);
//    camera->SetFocalPoint(0.0, 0.0, 0.0);
//    camera->SetViewUp(0.0, 1.0, 0.0);
//    this->Renderer->ResetCamera();
//
//    if (this->AnalogOutput) this->AnalogOutput->SetChannel(0, 1.0);
//
//    this->HomeDown = true;
//    }
//  else 
//    {
//    if (this->HomeDown)
//      {
//      if (this->AnalogOutput) this->AnalogOutput->SetChannel(0, 0.0);
//      this->HomeDown = false;
//      }
//    }
//
//  // Zoom
//  if (button->GetButton(vtkSpaceNavigatorStyle::ButtonMinus))
//    {
//    // Zoom out
//    camera->Dolly(1.0 - this->ZoomSensitivity);
//    } 
//  else if (button->GetButton(vtkSpaceNavigatorStyle::ButtonPlus))
//    {
//    // Zoom in
//    camera->Dolly(1.0 + this->ZoomSensitivity);
//    }
//
//  // Pan
//  if (button->GetButton(vtkSpaceNavigatorStyle::ButtonLeft))
//    {
//    // Pan left
//    this->Pan(-1.0, 0.0);
//    }
//  else if (button->GetButton(vtkSpaceNavigatorStyle::ButtonRight))
//    {
//    // Pan right
//    this->Pan(1.0, 0.0);
//    }
//  else if (button->GetButton(vtkSpaceNavigatorStyle::ButtonDown))
//    {
//    // Pan down
//    this->Pan(0.0, -1.0);
//    }
//  else if (button->GetButton(vtkSpaceNavigatorStyle::ButtonUp))
//    {
//    // Pan up
//    this->Pan(0.0, 1.0);
//    }
//
//  // Rotate
//  if (button->GetButton(ButtonB))
//    {
//    // Rotate based on SpaceNavigator orientation
//    if (!this->TriggerDown)
//      {
//      this->XRotate = this->XGravity;
//      this->YRotate = this->YGravity;
//      this->ZRotate = this->ZGravity;
//      }
//
//    camera->Azimuth((this->XGravity - this->XRotate) * this->RotateSensitivity);
//    camera->Elevation(-(this->YGravity - this->YRotate) * this->RotateSensitivity);
////    camera->Roll(-(this->ZGravity - this->ZRotate) * this->RotateSensitivity);
//    camera->OrthogonalizeViewUp();
//
//    this->TriggerDown = true;
//    }
//  else
//    {
//    this->TriggerDown = false;
//    }
//
//  this->Renderer->ResetCameraClippingRange();
//  // Render() will be called in the interactor
}

//----------------------------------------------------------------------------
void vtkSpaceNavigatorStyleCamera::Translate(double xDelta, double yDelta, double zDelta)
{
  vtkCamera* camera = this->Renderer->GetActiveCamera();

  double viewFocus[4], focalDepth, viewPoint[3];
  double newPickPoint[4], oldPickPoint[4], motionVector[3];

  camera->GetFocalPoint(viewFocus);
  vtkInteractorObserver::ComputeWorldToDisplay(this->Renderer, 
                                               viewFocus[0], viewFocus[1], viewFocus[2], 
                                               viewFocus);
  focalDepth = viewFocus[2];

  // Normalize for window size
  int width = this->Renderer->GetRenderWindow()->GetSize()[0];
  int height = this->Renderer->GetRenderWindow()->GetSize()[1];
  xDelta *= width * this->PanSensitivity;
  yDelta *= height * this->PanSensitivity;

  vtkInteractorObserver::ComputeDisplayToWorld(this->Renderer, 
                                               xDelta, yDelta, focalDepth, 
                                               newPickPoint);

  vtkInteractorObserver::ComputeDisplayToWorld(this->Renderer, 
                                               0, 0, focalDepth, 
                                               oldPickPoint);

  // Camera motion is reversed
  motionVector[0] = newPickPoint[0] - oldPickPoint[0];
  motionVector[1] = newPickPoint[1] - oldPickPoint[1];
  motionVector[2] = newPickPoint[2] - oldPickPoint[2];

  camera->GetFocalPoint(viewFocus);
  camera->GetPosition(viewPoint);
  camera->SetFocalPoint(motionVector[0] + viewFocus[0],
                        motionVector[1] + viewFocus[1],
                        motionVector[2] + viewFocus[2]);

  camera->SetPosition(motionVector[0] + viewPoint[0],
                      motionVector[1] + viewPoint[1],
                      motionVector[2] + viewPoint[2]);
}

//----------------------------------------------------------------------------
void vtkSpaceNavigatorStyleCamera::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "TranslateSensitivity: " << this->TranslateSensitivity << "\n";
  os << indent << "RotateSensitivity: " << this->RotateSensitivity << "\n";
  os << indent << "XPos: " << this->XPos << "\n";
  os << indent << "YPos: " << this->YPos << "\n";
  os << indent << "ZPos: " << this->ZPos << "\n";
  os << indent << "XRotate: " << this->XRotate << "\n";
  os << indent << "YRotate: " << this->YRotate << "\n";
  os << indent << "ZRotate: " << this->ZRotate << "\n";
}