/*=========================================================================

  Name:        vtkSpaceNavigatorStyle.h

  Author:      Alexis YL Chan

  Copyright:   The Renaissance Computing Institute (RENCI)

  License:     Licensed under the RENCI Open Source Software License v. 1.0.
               
               See included License.txt or 
               http://www.renci.org/resources/open-source-software-license
               for details.

=========================================================================*/
// .NAME vtkSpaceNavigatorStyle
// .SECTION Description
// vtkSpaceNavigatorStyle is an abstract base class for 
// vtkDeviceInteractorStyles that interface with a SpaceNavigator using the 
// Virtual Reality Peripheral Network (VRPN: 
// http://www.cs.unc.edu/Research/vrpn/).  
//
// .SECTION see also
// vtkDeviceInteractor vtkInteractionDevice

#ifndef __vtkSpaceNavigatorStyle_h
#define __vtkSpaceNavigatorStyle_h

#include "vtkInteractionDeviceConfigure.h"

#include "vtkDeviceInteractorStyle.h"

#include "vtkVRPNAnalog.h"
#include "vtkVRPNAnalogOutput.h"
#include "vtkVRPNButton.h"

class VTK_INTERACTIONDEVICE_EXPORT vtkSpaceNavigatorStyle : public vtkDeviceInteractorStyle
{
public:
  vtkTypeRevisionMacro(vtkSpaceNavigatorStyle,vtkDeviceInteractorStyle);
  void PrintSelf(ostream&, vtkIndent); 

  // Description:
  // Set the devices to connect to the SpaceNavigator
  void SetAnalog(vtkVRPNAnalog*);
  void SetButton(vtkVRPNButton*);

protected:
  vtkSpaceNavigatorStyle();
  ~vtkSpaceNavigatorStyle();
 
  //BTX
  enum SpaceNavigatorButtons {
    Button1 = 0,
    Button2,
  };
  //ETX
  //BTX
  enum SpaceNavigatorAnalogChannels {
    PositionX = 0,
    PositionY,
    PositionZ,
    RotationX,
    RotationY,
    RotationZ
  };
  //ETX

  virtual void OnAnalog(vtkVRPNAnalog*) = 0;
  virtual void OnButton(vtkVRPNButton*) = 0;

private:
  vtkSpaceNavigatorStyle(const vtkSpaceNavigatorStyle&);  // Not implemented.
  void operator=(const vtkSpaceNavigatorStyle&);  // Not implemented.
};

#endif