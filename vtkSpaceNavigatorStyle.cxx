/*=========================================================================

  Name:        vtkSpaceNavigatorStyle.cxx

  Author:      Alexis YL Chan

  Copyright:   The Renaissance Computing Institute (RENCI)

  License:     Licensed under the RENCI Open Source Software License v. 1.0.
               
               See included License.txt or 
               http://www.renci.org/resources/open-source-software-license
               for details.

=========================================================================*/

#include "vtkSpaceNavigatorStyle.h"

vtkCxxRevisionMacro(vtkSpaceNavigatorStyle, "$Revision: 1.0 $");

//----------------------------------------------------------------------------
vtkSpaceNavigatorStyle::vtkSpaceNavigatorStyle() 
{ 
  //this->AnalogOutput = NULL;
}

//----------------------------------------------------------------------------
vtkSpaceNavigatorStyle::~vtkSpaceNavigatorStyle() 
{
  //this->SetAnalogOutput(NULL);
}

//----------------------------------------------------------------------------
void vtkSpaceNavigatorStyle::SetAnalog(vtkVRPNAnalog* analog)
{  
  if (analog != NULL) 
    {
    analog->SetNumberOfChannels(6);
    analog->AddObserver(vtkVRPNDevice::AnalogEvent, this->DeviceCallback);
    }
} 

//----------------------------------------------------------------------------
void vtkSpaceNavigatorStyle::SetButton(vtkVRPNButton* button)
{  
  if (button != NULL) 
    {
    button->SetNumberOfButtons(6);
    button->AddObserver(vtkVRPNDevice::ButtonEvent, this->DeviceCallback);
    }
} 

//----------------------------------------------------------------------------
void vtkSpaceNavigatorStyle::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}