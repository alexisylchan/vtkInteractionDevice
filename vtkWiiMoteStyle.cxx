/*=========================================================================

  Name:        vtkWiiMoteStyle.cxx

  Author:      David Borland, The Renaissance Computing Institute (RENCI)

  Copyright:   The Renaissance Computing Institute (RENCI)

  License:     Licensed under the RENCI Open Source Software License v. 1.0.
               
               See included License.txt or 
               http://www.renci.org/resources/open-source-software-license
               for details.

=========================================================================*/

#include "vtkWiiMoteStyle.h"

vtkCxxRevisionMacro(vtkWiiMoteStyle, "$Revision: 1.0 $");

//----------------------------------------------------------------------------
vtkWiiMoteStyle::vtkWiiMoteStyle() 
{ 
  this->AnalogOutput = NULL;
}

//----------------------------------------------------------------------------
vtkWiiMoteStyle::~vtkWiiMoteStyle() 
{
  this->SetAnalogOutput(NULL);
}

//----------------------------------------------------------------------------
void vtkWiiMoteStyle::SetAnalog(vtkVRPNAnalog* analog)
{  
  if (analog != NULL) 
    {
    analog->SetNumberOfChannels(16);
    analog->AddObserver(vtkVRPNDevice::AnalogEvent, this->DeviceCallback);
    }
} 

//----------------------------------------------------------------------------
void vtkWiiMoteStyle::SetButton(vtkVRPNButton* button)
{  
  if (button != NULL) 
    {
    button->SetNumberOfButtons(16);
    button->AddObserver(vtkVRPNDevice::ButtonEvent, this->DeviceCallback);
    }
} 

//----------------------------------------------------------------------------
void vtkWiiMoteStyle::SetAnalogOutput(vtkVRPNAnalogOutput* analogOutput)
{  
  if (this->AnalogOutput == analogOutput)
    {
    return;
    }

  if (this->AnalogOutput != NULL) 
    {
    this->AnalogOutput->UnRegister(this);
    }

  this->AnalogOutput = analogOutput;

  if (this->AnalogOutput != NULL) 
    {
    this->AnalogOutput->Register(this);
    }

  this->Modified();
}

//----------------------------------------------------------------------------
void vtkWiiMoteStyle::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "AnalogOutput: " << this->AnalogOutput << "\n";
}