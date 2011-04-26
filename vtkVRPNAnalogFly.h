/*=========================================================================

  Name:        vtkVRPNAnalogFly.h

  Author:      David Borland, The Renaissance Computing Institute (RENCI)

  Copyright:   The Renaissance Computing Institute (RENCI)

  License:     Licensed under the RENCI Open Source Software License v. 1.0.
               
               See included License.txt or 
               http://www.renci.org/resources/open-source-software-license
               for details.

=========================================================================*/
// .NAME vtkVRPNAnalogFly
// .SECTION Description
// vtkVRPNAnalogFly interfaces with an analog device using the Virtual 
// Reality Peripheral Network (VRPN: 
// http://www.cs.unc.edu/Research/vrpn/).  

// .SECTION see also
// vtkInteractionDeviceManager vtkDeviceInteractorStyle

#ifndef __vtkVRPNAnalogFly_h
#define __vtkVRPNAnalogFly_h

#include "vtkInteractionDeviceConfigure.h"

#include "vtkVRPNDevice.h"

#include <vrpn_Tracker_AnalogFly.h>

// Holds vtkstd member variables, which must be hidden
class vtkVRPNAnalogFlyInternals;

class VTK_INTERACTIONDEVICE_EXPORT vtkVRPNAnalogFly : public vtkVRPNDevice
{
public:
  static vtkVRPNAnalogFly* New();
  vtkTypeRevisionMacro(vtkVRPNAnalogFly,vtkVRPNDevice);
  void PrintSelf(ostream&, vtkIndent);

  // Description:
  // Initialize the device
  virtual int Initialize();

  // Description:
  // Receive updates from the device
  virtual void Update();

  // Description:
  // Invoke vrpnDevice::AnalogEvent for observers to listen for
  virtual void InvokeInteractionEvent();

  // Description:
  // The number of channels to use
  void SetNumberOfChannels(int num);
  int GetNumberOfChannels();

  // Description:
  // Set/Get the analog information
  void SetChannel(int channel, double value);
  double GetChannel(int channel);

protected:
  vtkVRPNAnalogFly();
  ~vtkVRPNAnalogFly();

  vrpn_Tracker_AnalogFly_Remote* Analog;

  vtkVRPNAnalogFlyInternals* Internals;

private:
  vtkVRPNAnalogFly(const vtkVRPNAnalogFly&);  // Not implemented.
  void operator=(const vtkVRPNAnalogFly&);  // Not implemented.
};

#endif
