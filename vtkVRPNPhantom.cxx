/*=========================================================================

  Name:        vtkVRPNPhantom.cxx

  Author:      Alexis YL Chan

  Copyright:   The Renaissance Computing Institute (RENCI)

  License:     Licensed under the RENCI Open Source Software License v. 1.0.
               
               See included License.txt or vrpn_
               http://www.renci.org/resources/open-source-software-license
               for details.

=========================================================================*/

#include "vtkVRPNPhantom.h"

#include "vtkMath.h"
#include "vtkObjectFactory.h"
#include "vtkstd/vector"
#include "vtkMatrix4x4.h"

// Structure to hold Phantom information
struct PhantomInformation 
{
  double Position[3];
  double Rotation[4];

  double Velocity[3];
  double VelocityRotation[4];
  double VelocityRotationDelta;

  double Acceleration[3];
  double AccelerationRotation[4];
  double AccelerationRotationDelta;
  
  //vrpn_ForceDevice Surface Contact Point 
  double SCPPosition[3];
  double SCPRotation[4];

  //vrpn_Force 
  double Force[3];

  //vrpn_Button
  double ButtonIndex;
  double ButtonState;

};

class vtkVRPNPhantomInternals
{
public:
  vtkstd::vector<PhantomInformation> Sensors;
  vtkstd::vector<bool> Buttons;
  int sensorIndex;
};

// Callbacks
static void VRPN_CALLBACK HandlePosition(void* userData, const vrpn_TRACKERCB t);
static void VRPN_CALLBACK HandleVelocity(void* userData, const vrpn_TRACKERVELCB t);
static void VRPN_CALLBACK HandleAcceleration(void* userData, const vrpn_TRACKERACCCB t);
static void VRPN_CALLBACK HandleButton(void* userData, const vrpn_BUTTONCB b);
static void VRPN_CALLBACK HandleMockButton(void* userData, const vrpn_BUTTONCB b);
vtkCxxRevisionMacro(vtkVRPNPhantom, "$Revision: 1.0 $");
vtkStandardNewMacro(vtkVRPNPhantom);

//----------------------------------------------------------------------------
vtkVRPNPhantom::vtkVRPNPhantom() 
{
  this->Internals = new vtkVRPNPhantomInternals();

 // this->Phantom = NULL;
  this->PhantomTracker = NULL;
  this->PhantomButton = NULL;
  this->PhantomForceDevice = NULL;
  this->PhantomType = PHANTOM_TYPE_OMNI;
  this->mockButtonAddress = NULL;
  this->PhantomMockButton = NULL;
  this->SetPhantom2WorldTranslation(0.0, 0.0, 0.0);
  this->SetPhantom2WorldRotation(1.0, 0.0, 0.0, 0.0);

  this->SetNumberOfSensors(1);
}

//----------------------------------------------------------------------------
vtkVRPNPhantom::~vtkVRPNPhantom() 
{
 // if (this->Phantom) delete this->Phantom;
  if (this->PhantomTracker) delete this->PhantomTracker;
  if (this->PhantomButton) delete this->PhantomButton;
  if (this->PhantomMockButton) delete this->PhantomMockButton;
  if (this->PhantomForceDevice) delete this->PhantomForceDevice;
  if (this->mockButtonAddress) free (this->mockButtonAddress);
  delete this->Internals;
}
//Allows user to specify the sensor used for this phantom
void vtkVRPNPhantom::SetSensorIndex(int sensorIndex)
{
	this->Internals->sensorIndex = sensorIndex;
}

//Allows VRPN Callback to identify the sensor used for this phantom
int vtkVRPNPhantom::GetSensorIndex() 
{
  return this->Internals->sensorIndex;
}
//----------------------------------------------------------------------------
int vtkVRPNPhantom::Initialize() 
{
  // Check that the device name is set
  if (this->DeviceName == NULL) 
    {
    vtkErrorMacro(<<"DeviceName not set.");
    return 0;
    }

  // Create the VRPN Phantom remote 
  this->PhantomTracker = new vrpn_Tracker_Remote(this->DeviceName);

  // Set up the Phantom callbacks
  if (this->PhantomTracker->register_change_handler(this, HandlePosition) == -1 ||
      this->PhantomTracker->register_change_handler(this, HandleVelocity) == -1 ||
      this->PhantomTracker->register_change_handler(this, HandleAcceleration) == -1)
    {
    vtkErrorMacro(<<"Can't register callback.");
    return 0;
    }
 
  this->PhantomButton = new vrpn_Button_Remote(this->DeviceName); 
  if (this->PhantomButton->register_change_handler(this, HandleButton) == -1)
  {
    vtkErrorMacro(<<"Can't register callback.");
    return 0;
  }
  if (this->mockButtonAddress != NULL )
  {
	  this->PhantomMockButton = new vrpn_Button_Remote(this->mockButtonAddress);
	  
	 if (this->PhantomMockButton->register_change_handler(this, HandleMockButton) == -1)
	 {
	 	vtkErrorMacro(<<"Can't register callback.");
		return 0;
	 }
	 this->PhantomType = PHANTOM_TYPE_DESKTOP;
  }
  else
  {
	  
	 this->PhantomType = PHANTOM_TYPE_OMNI;
  }
  return 1;
}
void vtkVRPNPhantom::SetPhantomMockButtonAddress(const char* address)
{
	this->mockButtonAddress = (char *) malloc(strlen(address)+1);
	memcpy(this->mockButtonAddress,address,strlen(address) + 1); 
}
  

char* vtkVRPNPhantom::GetPhantomMockButtonAddress()
{
	return this->mockButtonAddress;
}
  
//----------------------------------------------------------------------------
void vtkVRPNPhantom::Update() 
{
  if (this->PhantomTracker)
    {
    this->PhantomTracker->mainloop();
    }
  if (this->PhantomMockButton)
  {
	  this->PhantomMockButton->mainloop();
  }
}

//----------------------------------------------------------------------------
void vtkVRPNPhantom::InvokeInteractionEvent() 
{
  if (this->PhantomTracker)
    {
    // XXX: Should there be a flag to check for new data?
    this->InvokeEvent(vtkVRPNDevice::PhantomEvent);
    }
}

//----------------------------------------------------------------------------
void vtkVRPNPhantom::SetNumberOfSensors(int num) 
{
  int currentNum = this->Internals->Sensors.size();

  this->Internals->Sensors.resize(num);

  double identityVector[3] = { 0.0, 0.0, 0.0 };
  double identityRotation[4] = { 1.0, 0.0, 0.0, 0.0 };

  for (int i = currentNum; i < num; i++) 
    {
    this->SetPosition(identityVector, i);
    this->SetRotation(identityRotation, i);
    
    this->SetVelocity(identityVector, i);
    this->SetVelocityRotation(identityRotation, i);
    this->SetVelocityRotationDelta(1.0, i);

    this->SetAcceleration(identityVector, i);
    this->SetAccelerationRotation(identityRotation, i);
    this->SetAccelerationRotationDelta(1.0, i);
    }
}

//----------------------------------------------------------------------------
int vtkVRPNPhantom::GetNumberOfSensors() 
{
  return this->Internals->Sensors.size();
}

//----------------------------------------------------------------------------
void vtkVRPNPhantom::SetPosition(double* position, int sensor)
{
  for (int i = 0; i < 3; i++)
    {
    this->Internals->Sensors[sensor].Position[i] = position[i];
    }
}

//----------------------------------------------------------------------------
double* vtkVRPNPhantom::GetPosition(int sensor)
{
  return this->Internals->Sensors[sensor].Position;
}

//----------------------------------------------------------------------------
void vtkVRPNPhantom::SetRotation(double* rotation, int sensor)
{
  for (int i = 0; i < 4; i++)
    {
    this->Internals->Sensors[sensor].Rotation[i] = rotation[i];
    }
}

//----------------------------------------------------------------------------
double* vtkVRPNPhantom::GetRotation(int sensor)
{
  return this->Internals->Sensors[sensor].Rotation;
}

//----------------------------------------------------------------------------
void vtkVRPNPhantom::SetVelocity(double* velocity, int sensor)
{
  for (int i = 0; i < 3; i++)
    {
    this->Internals->Sensors[sensor].Velocity[i] = velocity[i];
    }
}

//----------------------------------------------------------------------------
double* vtkVRPNPhantom::GetVelocity(int sensor)
{
  return this->Internals->Sensors[sensor].Velocity;
}

//----------------------------------------------------------------------------
void vtkVRPNPhantom::SetVelocityRotation(double* rotation, int sensor)
{
  for (int i = 0; i < 4; i++)
    {
    this->Internals->Sensors[sensor].VelocityRotation[i] = rotation[i];
    }
}

//----------------------------------------------------------------------------
double* vtkVRPNPhantom::GetVelocityRotation(int sensor)
{
  return this->Internals->Sensors[sensor].Velocity;
}

//----------------------------------------------------------------------------
void vtkVRPNPhantom::SetVelocityRotationDelta(double delta, int sensor)
{
  this->Internals->Sensors[sensor].VelocityRotationDelta = delta;
}

//----------------------------------------------------------------------------
double vtkVRPNPhantom::GetVelocityRotationDelta(int sensor)
{
  return this->Internals->Sensors[sensor].VelocityRotationDelta;
}

//----------------------------------------------------------------------------
void vtkVRPNPhantom::SetAcceleration(double* acceleration, int sensor)
{
  for (int i = 0; i < 3; i++)
    {
    this->Internals->Sensors[sensor].Acceleration[i] = acceleration[i];
    }
}

//----------------------------------------------------------------------------
double* vtkVRPNPhantom::GetAcceleration(int sensor)
{
  return this->Internals->Sensors[sensor].Acceleration;
}

//----------------------------------------------------------------------------
void vtkVRPNPhantom::SetAccelerationRotation(double* rotation, int sensor)
{
  for (int i = 0; i < 4; i++)
    {
    this->Internals->Sensors[sensor].AccelerationRotation[i] = rotation[i];
    }
}

//----------------------------------------------------------------------------
double* vtkVRPNPhantom::GetAccelerationRotation(int sensor)
{
  return this->Internals->Sensors[sensor].Acceleration;
}

//----------------------------------------------------------------------------
void vtkVRPNPhantom::SetAccelerationRotationDelta(double delta, int sensor)
{
  this->Internals->Sensors[sensor].AccelerationRotationDelta = delta;
}

//----------------------------------------------------------------------------
double vtkVRPNPhantom::GetAccelerationRotationDelta(int sensor)
{
  return this->Internals->Sensors[sensor].AccelerationRotationDelta;
}

//----------------------------------------------------------------------------
void VRPN_CALLBACK HandlePosition(void* userData, const vrpn_TRACKERCB t) {
  vtkVRPNPhantom* Phantom = static_cast<vtkVRPNPhantom*>(userData);

  if (t.sensor < Phantom->GetNumberOfSensors()) 
    {
    // Transform the position
    double pos[3];
    double t2wTrans[3];
    Phantom->GetPhantom2WorldTranslation(t2wTrans);
    for (int i = 0; i < 3; i++) {
        pos[i] = t.pos[i] + t2wTrans[i];
    } 

    // Convert from vrpn quaternion (x, y, z, w) to vtk quaternion (w, x, y, z)
    double vtkQuat[4];
    vtkQuat[0] = t.quat[3];
    vtkQuat[1] = t.quat[0];
    vtkQuat[2] = t.quat[1];
    vtkQuat[3] = t.quat[2];

    // Transform the rotation via matrix operations.  
    // Would be nice if VTK had quaternion operations instead...
    double rot[3][3];
    vtkMath::QuaternionToMatrix3x3(vtkQuat, rot);

    double t2wRot[3][3];
    vtkMath::QuaternionToMatrix3x3(Phantom->GetPhantom2WorldRotation(), t2wRot);

    vtkMath::Multiply3x3(rot, t2wRot, rot);

    vtkMath::Matrix3x3ToQuaternion(rot, vtkQuat);

    // Set the rotation for this sensor
    Phantom->SetRotation(vtkQuat, t.sensor);

	vtkMatrix4x4* t2wmatrix = vtkMatrix4x4::New(); 
	t2wmatrix->SetElement(0,0,t2wRot[0][0]); 
	t2wmatrix->SetElement(0,1,t2wRot[0][1]); 
	t2wmatrix->SetElement(0,2,t2wRot[0][2]); 
	t2wmatrix->SetElement(1,0,t2wRot[1][0]); 
	t2wmatrix->SetElement(1,1,t2wRot[1][1]); 
	t2wmatrix->SetElement(1,2,t2wRot[1][2]); 
	t2wmatrix->SetElement(2,0,t2wRot[2][0]); 
	t2wmatrix->SetElement(2,1,t2wRot[2][1]); 
	t2wmatrix->SetElement(2,2,t2wRot[2][2]);  
	t2wmatrix->MultiplyPoint(pos,pos);
    // Set the position for this sensor
    Phantom->SetPosition(pos, t.sensor);

    }
}

//----------------------------------------------------------------------------
void VRPN_CALLBACK HandleVelocity(void* userData, const vrpn_TRACKERVELCB t) {
  vtkVRPNPhantom* Phantom = static_cast<vtkVRPNPhantom*>(userData);

  if (t.sensor < Phantom->GetNumberOfSensors()) 
    {
    // Set the velocity for this sensor
    Phantom->SetVelocity((double*)t.vel, t.sensor);
    
    // Convert from vrpn quaternion (x, y, z, w) to vtk quaternion (w, x, y, z)
    double vtkQuat[4];
    vtkQuat[0] = t.vel_quat[1];
    vtkQuat[1] = t.vel_quat[2];
    vtkQuat[2] = t.vel_quat[3];
    vtkQuat[3] = t.vel_quat[0];
    Phantom->SetVelocityRotation(vtkQuat, t.sensor);

    // Set the velocity rotation for this sensor
    Phantom->SetVelocityRotation(vtkQuat, t.sensor);
    
    // Set the velocity rotation delta for this sensor
    Phantom->SetVelocityRotationDelta(t.vel_quat_dt, t.sensor);
    }
}

//----------------------------------------------------------------------------
void VRPN_CALLBACK HandleAcceleration(void* userData, const vrpn_TRACKERACCCB t) {
  vtkVRPNPhantom* Phantom = static_cast<vtkVRPNPhantom*>(userData);

  if (t.sensor < Phantom->GetNumberOfSensors()) 
    {
    // Set the acceleration for this sensor
    Phantom->SetAcceleration((double*)t.acc, t.sensor);
    
    // Convert from vrpn quaternion (x, y, z, w) to vtk quaternion (w, x, y, z)
    double vtkQuat[4];
    vtkQuat[0] = t.acc_quat[1];
    vtkQuat[1] = t.acc_quat[2];
    vtkQuat[2] = t.acc_quat[3];
    vtkQuat[3] = t.acc_quat[0];
    Phantom->SetAccelerationRotation(vtkQuat, t.sensor);

    // Set the acceleration rotation for this sensor
    Phantom->SetAccelerationRotation(vtkQuat, t.sensor);

    // Set the acceleration rotation delta for this sensor
    Phantom->SetAccelerationRotationDelta(t.acc_quat_dt, t.sensor);
    }
}

//----------------------------------------------------------------------------
void vtkVRPNPhantom::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "Phantom: "; PhantomTracker->print_latest_report();

  os << indent << "Sensors:" << endl;
  for (unsigned int i = 0; i < this->Internals->Sensors.size(); i++)
    {
    os << indent << indent << "Position: (" << this->Internals->Sensors[i].Position[0]
                 << ", " << this->Internals->Sensors[i].Position[1]
                 << ", " << this->Internals->Sensors[i].Position[2] << ")\n";
    os << indent << indent << "Rotation: (" << this->Internals->Sensors[i].Rotation[0]
                 << ", " << this->Internals->Sensors[i].Rotation[1]
                 << ", " << this->Internals->Sensors[i].Rotation[2]
                 << ", " << this->Internals->Sensors[i].Rotation[3] << ")\n";
    os << indent << indent << "Velocity: (" << this->Internals->Sensors[i].Velocity[0]
                 << ", " << this->Internals->Sensors[i].Velocity[1]
                 << ", " << this->Internals->Sensors[i].Velocity[2] << ")\n";    
    os << indent << indent << "Velocity Rotation: (" << this->Internals->Sensors[i].VelocityRotation[0]
                 << ", " << this->Internals->Sensors[i].VelocityRotation[1]
                 << ", " << this->Internals->Sensors[i].VelocityRotation[2] 
                 << ", " << this->Internals->Sensors[i].VelocityRotation[3] << ")\n";   
    os << indent << indent << "VelocityRotationDelta: " << this->Internals->Sensors[i].VelocityRotationDelta << "\n";    
    os << indent << indent << "Acceleration: (" << this->Internals->Sensors[i].Acceleration[0]
                 << ", " << this->Internals->Sensors[i].Acceleration[1]
                 << ", " << this->Internals->Sensors[i].Acceleration[2] << ")\n";    
    os << indent << indent << "Acceleration Rotation: (" << this->Internals->Sensors[i].AccelerationRotation[0]
                 << ", " << this->Internals->Sensors[i].AccelerationRotation[1]
                 << ", " << this->Internals->Sensors[i].AccelerationRotation[2]
                 << ", " << this->Internals->Sensors[i].AccelerationRotation[3] << ")\n";   
    os << indent << indent << "AccelerationRotationDelta: " << this->Internals->Sensors[i].AccelerationRotationDelta << "\n";   
    }
}

//----------------------------------------------------------------------------
void vtkVRPNPhantom::SetNumberOfButtons(int num) 
{
  int currentNum = this->Internals->Buttons.size();

  this->Internals->Buttons.resize(num);

  for (int i = currentNum; i < num; i++) 
    {
    this->SetButton(i, false);
    }
}

//----------------------------------------------------------------------------
int vtkVRPNPhantom::GetNumberOfButtons() 
{
  return this->Internals->Buttons.size();
}

//----------------------------------------------------------------------------
void vtkVRPNPhantom::SetButton(int button, bool value)
{
  this->Internals->Buttons[button] = value;
}

//----------------------------------------------------------------------------
bool vtkVRPNPhantom::GetButton(int button)
{
	int buttonSize = this->Internals->Buttons.size()  - 1;
	if (buttonSize >= button)
	{
		bool returnVal =  this->Internals->Buttons[button];
		return returnVal;
	}
	return false;
}

//----------------------------------------------------------------------------
void vtkVRPNPhantom::SetToggle(int button, bool toggle) 
{
  if (!this->PhantomButton) return;

  if (toggle) this->PhantomButton->set_toggle(button, vrpn_BUTTON_TOGGLE_ON);
  else this->PhantomButton->set_momentary(button);
}


//----------------------------------------------------------------------------
void VRPN_CALLBACK HandleButton(void* userData, const vrpn_BUTTONCB b) {
  vtkVRPNPhantom* Phantom = static_cast<vtkVRPNPhantom*>(userData);

  if (b.button < Phantom->GetNumberOfButtons())// Num buttons set to 2
    {
    Phantom->SetButton(b.button, b.state != 0);
    }
}
//----------------------------------------------------------------------------
void VRPN_CALLBACK HandleMockButton(void* userData, const vrpn_BUTTONCB b) {
  vtkVRPNPhantom* Phantom = static_cast<vtkVRPNPhantom*>(userData);

  if ( (b.button + 1) < Phantom->GetNumberOfButtons())// Num buttons set to 2
    {
    Phantom->SetButton((b.button + 1), b.state != 0);
    }
}