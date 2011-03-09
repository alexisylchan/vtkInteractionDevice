/*=========================================================================

  Name:        vtkDeviceInteractorStyle.cxx

  Author:      David Borland, The Renaissance Computing Institute (RENCI)

  Copyright:   The Renaissance Computing Institute (RENCI)

  License:     Licensed under the RENCI Open Source Software License v. 1.0.
               
               See included License.txt or 
               http://www.renci.org/resources/open-source-software-license
               for details.

=========================================================================*/

#include "vtkDeviceInteractorStyle.h"

vtkCxxRevisionMacro(vtkDeviceInteractorStyle, "$Revision: 1.0 $");

//----------------------------------------------------------------------------
vtkDeviceInteractorStyle::vtkDeviceInteractorStyle() 
{
  this->Renderer = NULL;

  this->DeviceCallback = vtkCallbackCommand::New();
  this->DeviceCallback->SetClientData(this);
  this->DeviceCallback->SetCallback(vtkDeviceInteractorStyle::ProcessEvents);
}

//----------------------------------------------------------------------------
vtkDeviceInteractorStyle::~vtkDeviceInteractorStyle() 
{
  this->DeviceCallback->Delete();

  this->SetRenderer(NULL);
}

//----------------------------------------------------------------------------
void vtkDeviceInteractorStyle::SetRenderer(vtkRenderer* renderer)
{  
  if (this->Renderer == renderer)
    {
    return;
    }

  if (this->Renderer != NULL) 
    {
    this->Renderer->UnRegister(this);
    }

  this->Renderer = renderer;

  if (this->Renderer != NULL) 
    {
    this->Renderer->Register(this);
    }

  this->Modified();
} 

//----------------------------------------------------------------------------
void vtkDeviceInteractorStyle::ProcessEvents(vtkObject* caller, 
                                             unsigned long eid,
                                             void* clientdata, 
                                             void* calldata) 
{  
  vtkDeviceInteractorStyle* self = static_cast<vtkDeviceInteractorStyle*>(clientdata);
  self->OnEvent(caller, eid, calldata);
}

//----------------------------------------------------------------------------
void vtkDeviceInteractorStyle::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "Renderer:\n";
  this->Renderer->PrintSelf(os,indent.GetNextIndent());
  os << indent << "DeviceCallback:\n";
  this->DeviceCallback->PrintSelf(os,indent.GetNextIndent());
}