/*=========================================================================

  Name:        vtkVRPNPhantom.h

  Author:      Alexis YL Chan

  Copyright:   The Renaissance Computing Institute (RENCI)

  License:     Licensed under the RENCI Open Source Software License v. 1.0.
               
               See included License.txt or 
               http://www.renci.org/resources/open-source-software-license
               for details.

=========================================================================*/
// .NAME vtkVRPNPhantom
// .SECTION Description
// vtkVRPNPhantom interfaces with a tracking device using the Virtual 
// Reality Peripheral Network (VRPN: 
// http://www.cs.unc.edu/Research/vrpn/).  

// .SECTION see also
// vtkInteractionDeviceManager vtkDeviceInteractorStyle

#ifndef __vtkVRPNPhantom_h
#define __vtkVRPNPhantom_h

#include "vtkInteractionDeviceConfigure.h"

#include "vtkVRPNDevice.h"

//#include <vrpn_Phantom.h>
#include <vrpn_Tracker.h>
#include <vrpn_Button.h>
#include <vrpn_ForceDevice.h>
#define PHANTOM_TYPE_DESKTOP 0
#define PHANTOM_TYPE_OMNI 1

// Holds vtkstd member variables, which must be hidden
class vtkVRPNPhantomInternals;

class VTK_INTERACTIONDEVICE_EXPORT vtkVRPNPhantom : public vtkVRPNDevice
{
public:
  static vtkVRPNPhantom* New();
  vtkTypeRevisionMacro(vtkVRPNPhantom,vtkVRPNDevice);
  void PrintSelf(ostream&, vtkIndent);
  void SetSensorIndex(int sensorIndex);
  int GetSensorIndex();
  
  // Description:
  // Initialize the device
  virtual int Initialize();

  // Description:
  // Receive updates from the device
  virtual void Update();

  // Description:
  // Invoke vrpnDevice::PhantomEvent for observers to listen for
  virtual void InvokeInteractionEvent();

  // Description:
  // The number of sensors to use
  void SetNumberOfSensors(int num);
  int GetNumberOfSensors();
   // Description:
  // The number of buttons to use
  void SetNumberOfButtons(int num);
  int GetNumberOfButtons();
 // Description:
  // Set/Get the button information
  void SetButton(int button, bool value);
  bool GetButton(int button);
    // Description:
  // Use toggle buttons or not.  Will have no effect until the device is initialized.
  void SetToggle(int button, bool toggle);

  // Description:
  // Transformation from Phantom space to world space
  vtkSetVector3Macro(Phantom2WorldTranslation,double);
  vtkGetVector3Macro(Phantom2WorldTranslation,double);
  vtkSetVector4Macro(Phantom2WorldRotation,double);
  vtkGetVector4Macro(Phantom2WorldRotation,double);

  // Description:
  // Set/Get the Phantom information
  void SetPosition(double* position, int sensor = 0);
  double* GetPosition(int sensor = 0);
  void SetRotation(double* rotation, int sensor = 0);
  double* GetRotation(int sensor = 0);

  void SetVelocity(double* velocity, int sensor = 0);
  double* GetVelocity(int sensor = 0);
  void SetVelocityRotation(double* rotation, int sensor = 0);
  double* GetVelocityRotation(int sensor = 0);
  void SetVelocityRotationDelta(double delta, int sensor = 0);
  double GetVelocityRotationDelta(int sensor = 0);

  void SetAcceleration(double* acceleration, int sensor = 0);
  double* GetAcceleration(int sensor = 0);
  void SetAccelerationRotation(double* rotation, int sensor = 0);
  double* GetAccelerationRotation(int sensor = 0);
  void SetAccelerationRotationDelta(double delta, int sensor = 0);
  double GetAccelerationRotationDelta(int sensor = 0);

  void SetPhantomMockButtonAddress(const char* address);
  vrpn_Tracker_Remote* PhantomTracker;
  vrpn_Button_Remote* PhantomButton;
  vrpn_Button_Remote* PhantomMockButton;
  vrpn_ForceDevice_Remote* PhantomForceDevice;
  int PhantomType;
protected:
  vtkVRPNPhantom();
  ~vtkVRPNPhantom();


  double Phantom2WorldTranslation[3];
  double Phantom2WorldRotation[4];

  vtkVRPNPhantomInternals* Internals;
  char* mockButtonAddress;
private:
  vtkVRPNPhantom(const vtkVRPNPhantom&);  // Not implemented.
  void operator=(const vtkVRPNPhantom&);  // Not implemented.
};

#endif
