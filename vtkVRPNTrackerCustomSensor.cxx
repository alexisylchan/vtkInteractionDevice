/*=========================================================================

  Name:        vtkVRPNTracker.cxx

  Author:      Alexis YL Chan

  Copyright:   The Renaissance Computing Institute (RENCI)

  License:     Licensed under the RENCI Open Source Software License v. 1.0.
               
               See included License.txt or 
               http://www.renci.org/resources/open-source-software-license
               for details.

=========================================================================*/

#include "vtkVRPNTrackerCustomSensor.h"

#include "vtkMath.h"
#include "vtkObjectFactory.h"
#include "vtkstd/vector"
#include "vtkMatrix4x4.h"

// Structure to hold tracker information
struct TrackerInformation 
{
  double Position[3];
  double Rotation[4];

  double Velocity[3];
  double VelocityRotation[4];
  double VelocityRotationDelta;

  double Acceleration[3];
  double AccelerationRotation[4];
  double AccelerationRotationDelta;
};

class vtkVRPNTrackerInternals
{
public:
  vtkstd::vector<TrackerInformation> Sensors;
  int sensorIndex;
};
//
// Callbacks
static void VRPN_CALLBACK HandlePosition(void* userData, const vrpn_TRACKERCB t);
static void VRPN_CALLBACK HandleVelocity(void* userData, const vrpn_TRACKERVELCB t);
static void VRPN_CALLBACK HandleAcceleration(void* userData, const vrpn_TRACKERACCCB t);

vtkCxxRevisionMacro(vtkVRPNTrackerCustomSensor, "$Revision: 1.0 $");
vtkStandardNewMacro(vtkVRPNTrackerCustomSensor);

//----------------------------------------------------------------------------
vtkVRPNTrackerCustomSensor::vtkVRPNTrackerCustomSensor() 
{
	this->Internals = new vtkVRPNTrackerInternals();

	this->Tracker = NULL;

    this->SetTracker2WorldTranslation(0.0, 0.0, 0.0);
    this->SetTracker2WorldRotation(1.0, 0.0, 0.0, 0.0);
	this->Internals->sensorIndex = 0;

    this->InitializeSensors();
}

//----------------------------------------------------------------------------
vtkVRPNTrackerCustomSensor::~vtkVRPNTrackerCustomSensor() 
{
  if (this->Tracker) delete this->Tracker;

  delete this->Internals;
}
//Allows user to specify the sensor used for this tracker
void vtkVRPNTrackerCustomSensor::SetSensorIndex(int sensorIndex)
{
	this->Internals->sensorIndex = sensorIndex;
}

//Allows VRPN Callback to identify the sensor used for this tracker
int vtkVRPNTrackerCustomSensor::GetSensorIndex() 
{
  return this->Internals->sensorIndex;
}
//----------------------------------------------------------------------------
int vtkVRPNTrackerCustomSensor::Initialize() 
{
  // Check that the device name is set
  if (this->DeviceName == NULL) 
    {
    vtkErrorMacro(<<"DeviceName not set.");
    return 0;
    }

  // Create the VRPN tracker remote 
  this->Tracker = new vrpn_Tracker_Remote(this->DeviceName);

  // Set up the tracker callbacks
  if (this->Tracker->register_change_handler(this, HandlePosition) == -1 ||
      this->Tracker->register_change_handler(this, HandleVelocity) == -1 ||
      this->Tracker->register_change_handler(this, HandleAcceleration) == -1)
    {
    vtkErrorMacro(<<"Can't register callback.");
    return 0;
    }

  return 1;
}

////----------------------------------------------------------------------------
//void vtkVRPNTrackerCustomSensor::Update() 
//{
//  if (this->Tracker)
//    {
//    this->Tracker->mainloop();
//    }
//}
//
////----------------------------------------------------------------------------
//void vtkVRPNTrackerCustomSensor::InvokeInteractionEvent() 
//{
//  if (this->Tracker)
//    {
//    // XXX: Should there be a flag to check for new data?
//    this->InvokeEvent(vtkVRPNDevice::TrackerEvent);
//    }
//}
//
//----------------------------------------------------------------------------
void vtkVRPNTrackerCustomSensor::InitializeSensors() 
{
  this->Internals->Sensors.resize(1);

  double identityVector[3] = { 0.0, 0.0, 0.0 };
  double identityRotation[4] = { 1.0, 0.0, 0.0, 0.0 };
    this->SetPosition(identityVector);
    this->SetRotation(identityRotation);
    
    this->SetVelocity(identityVector);
    this->SetVelocityRotation(identityRotation);
    this->SetVelocityRotationDelta(1.0);

    this->SetAcceleration(identityVector);
    this->SetAccelerationRotation(identityRotation);
    this->SetAccelerationRotationDelta(1.0);
}


//----------------------------------------------------------------------------
void vtkVRPNTrackerCustomSensor::SetNumberOfSensors(int num) 
{
  // Do not change number of sensors. Use only 1 sensor.
  return;
  /*int currentNum = this->Internals->Sensors.size();

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
    }*/
}

//----------------------------------------------------------------------------
int vtkVRPNTrackerCustomSensor::GetNumberOfSensors() 
{
  return this->Internals->Sensors.size();
}

//----------------------------------------------------------------------------
void vtkVRPNTrackerCustomSensor::SetPosition(double* position)
{
    for (int i = 0; i < 3; i++)
    {
    this->Internals->Sensors[INTERNAL_SENSOR].Position[i] = position[i];
    }
}

//----------------------------------------------------------------------------
double* vtkVRPNTrackerCustomSensor::GetPosition()
{
  return this->Internals->Sensors[INTERNAL_SENSOR].Position;
}

//----------------------------------------------------------------------------
void vtkVRPNTrackerCustomSensor::SetRotation(double* rotation)
{
  for (int i = 0; i < 4; i++)
    {
    this->Internals->Sensors[INTERNAL_SENSOR].Rotation[i] = rotation[i];
    }
}

//----------------------------------------------------------------------------
double* vtkVRPNTrackerCustomSensor::GetRotation()
{
  return this->Internals->Sensors[INTERNAL_SENSOR].Rotation;
}

//----------------------------------------------------------------------------
void vtkVRPNTrackerCustomSensor::SetVelocity(double* velocity)
{
  for (int i = 0; i < 3; i++)
    {
    this->Internals->Sensors[INTERNAL_SENSOR].Velocity[i] = velocity[i];
    }
}

//----------------------------------------------------------------------------
double* vtkVRPNTrackerCustomSensor::GetVelocity()
{
  return this->Internals->Sensors[INTERNAL_SENSOR].Velocity;
}

//----------------------------------------------------------------------------
void vtkVRPNTrackerCustomSensor::SetVelocityRotation(double* rotation)
{
  for (int i = 0; i < 4; i++)
    {
    this->Internals->Sensors[INTERNAL_SENSOR].VelocityRotation[i] = rotation[i];
    }
}

//----------------------------------------------------------------------------
double* vtkVRPNTrackerCustomSensor::GetVelocityRotation()
{
  return this->Internals->Sensors[INTERNAL_SENSOR].Velocity;
}

//----------------------------------------------------------------------------
void vtkVRPNTrackerCustomSensor::SetVelocityRotationDelta(double delta)
{
  this->Internals->Sensors[INTERNAL_SENSOR].VelocityRotationDelta = delta;
}

//----------------------------------------------------------------------------
double vtkVRPNTrackerCustomSensor::GetVelocityRotationDelta()
{
  return this->Internals->Sensors[INTERNAL_SENSOR].VelocityRotationDelta;
}

//----------------------------------------------------------------------------
void vtkVRPNTrackerCustomSensor::SetAcceleration(double* acceleration)
{
  for (int i = 0; i < 3; i++)
    {
    this->Internals->Sensors[INTERNAL_SENSOR].Acceleration[i] = acceleration[i];
    }
}

//----------------------------------------------------------------------------
double* vtkVRPNTrackerCustomSensor::GetAcceleration()
{
  return this->Internals->Sensors[INTERNAL_SENSOR].Acceleration;
}

//----------------------------------------------------------------------------
void vtkVRPNTrackerCustomSensor::SetAccelerationRotation(double* rotation)
{
  for (int i = 0; i < 4; i++)
    {
    this->Internals->Sensors[INTERNAL_SENSOR].AccelerationRotation[i] = rotation[i];
    }
}

//----------------------------------------------------------------------------
double* vtkVRPNTrackerCustomSensor::GetAccelerationRotation()
{
  return this->Internals->Sensors[INTERNAL_SENSOR].Acceleration;
}

//----------------------------------------------------------------------------
void vtkVRPNTrackerCustomSensor::SetAccelerationRotationDelta(double delta)
{
  this->Internals->Sensors[INTERNAL_SENSOR].AccelerationRotationDelta = delta;
}

//----------------------------------------------------------------------------
double vtkVRPNTrackerCustomSensor::GetAccelerationRotationDelta()
{
  return this->Internals->Sensors[INTERNAL_SENSOR].AccelerationRotationDelta;
}

//----------------------------------------------------------------------------
void VRPN_CALLBACK HandlePosition(void* userData, const vrpn_TRACKERCB t) {
  vtkVRPNTrackerCustomSensor* tracker = static_cast<vtkVRPNTrackerCustomSensor*>(userData);

  // Get the current sensor based on stored sensorIndex given by user
  if (t.sensor == tracker->GetSensorIndex())
    {
    // Transform the position
    double pos[3];
    double t2wTrans[3];
    tracker->GetTracker2WorldTranslation(t2wTrans);
    for (int i = 0; i < 3; i++) {
        pos[i] = t.pos[i] + t2wTrans[i];
    } 

   
	
	//Multiply tracker position change by Tracker-Space to World-Space rotation
	vtkMatrix4x4* t2wmatrix = vtkMatrix4x4::New();
		//0 -1 0
		//0  0 1
		//-1 0 0
	t2wmatrix->SetElement(0,0,0); 
	t2wmatrix->SetElement(0,1,-1); 
	t2wmatrix->SetElement(0,2,0); 
	t2wmatrix->SetElement(1,0,0); 
	t2wmatrix->SetElement(1,1,0); 
	t2wmatrix->SetElement(1,2,1); 
	t2wmatrix->SetElement(2,0,-1); 
	t2wmatrix->SetElement(2,1,0); 
	t2wmatrix->SetElement(2,2,0);  
	t2wmatrix->MultiplyPoint(pos,pos);
    // Set the position for this sensor
    tracker->SetPosition(pos);

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
	//Note:Tracker2WorldRotation is used to denote rotation transform from Tracker Space
	// To ParaView Space. The original Tracker2WorldRotation was used to denote rotation from
	// Head to Eye.
    vtkMath::QuaternionToMatrix3x3(tracker->GetTracker2WorldRotation(), t2wRot);
    vtkMath::Multiply3x3(t2wRot, rot, rot); 
    vtkMath::Matrix3x3ToQuaternion(rot, vtkQuat);

    // Set the rotation for this sensor
    tracker->SetRotation(vtkQuat);
    }
}

//----------------------------------------------------------------------------
void VRPN_CALLBACK HandleVelocity(void* userData, const vrpn_TRACKERVELCB t) {
  vtkVRPNTrackerCustomSensor* tracker = static_cast<vtkVRPNTrackerCustomSensor*>(userData);

  if (t.sensor == tracker->GetSensorIndex())
    {
    // Set the velocity for this sensor
    tracker->SetVelocity((double*)t.vel);
    
    // Convert from vrpn quaternion (x, y, z, w) to vtk quaternion (w, x, y, z)
    double vtkQuat[4];
    vtkQuat[0] = t.vel_quat[1];
    vtkQuat[1] = t.vel_quat[2];
    vtkQuat[2] = t.vel_quat[3];
    vtkQuat[3] = t.vel_quat[0];
    tracker->SetVelocityRotation(vtkQuat);

    // Set the velocity rotation for this sensor
    tracker->SetVelocityRotation(vtkQuat);
    
    // Set the velocity rotation delta for this sensor
    tracker->SetVelocityRotationDelta(t.vel_quat_dt);
    }
}

//----------------------------------------------------------------------------
void VRPN_CALLBACK HandleAcceleration(void* userData, const vrpn_TRACKERACCCB t) {
  vtkVRPNTrackerCustomSensor* tracker = static_cast<vtkVRPNTrackerCustomSensor*>(userData);

  if (t.sensor == tracker->GetSensorIndex())
    {
    // Set the acceleration for this sensor
    tracker->SetAcceleration((double*)t.acc);
    
    // Convert from vrpn quaternion (x, y, z, w) to vtk quaternion (w, x, y, z)
    double vtkQuat[4];
    vtkQuat[0] = t.acc_quat[1];
    vtkQuat[1] = t.acc_quat[2];
    vtkQuat[2] = t.acc_quat[3];
    vtkQuat[3] = t.acc_quat[0];
    tracker->SetAccelerationRotation(vtkQuat);

    // Set the acceleration rotation for this sensor
    tracker->SetAccelerationRotation(vtkQuat);

    // Set the acceleration rotation delta for this sensor
    tracker->SetAccelerationRotationDelta(t.acc_quat_dt);
    }
}

////----------------------------------------------------------------------------
//void vtkVRPNTrackerCustomSensor::PrintSelf(ostream& os, vtkIndent indent)
//{
//  this->Superclass::PrintSelf(os,indent);
//
//  os << indent << "Tracker: "; Tracker->print_latest_report();
//
//  os << indent << "Sensors:" << endl;
//  for (unsigned int i = 0; i < this->Internals->Sensors.size(); i++)
//    {
//    os << indent << indent << "Position: (" << this->Internals->Sensors[i].Position[0]
//                 << ", " << this->Internals->Sensors[i].Position[1]
//                 << ", " << this->Internals->Sensors[i].Position[2] << ")\n";
//    os << indent << indent << "Rotation: (" << this->Internals->Sensors[i].Rotation[0]
//                 << ", " << this->Internals->Sensors[i].Rotation[1]
//                 << ", " << this->Internals->Sensors[i].Rotation[2]
//                 << ", " << this->Internals->Sensors[i].Rotation[3] << ")\n";
//    os << indent << indent << "Velocity: (" << this->Internals->Sensors[i].Velocity[0]
//                 << ", " << this->Internals->Sensors[i].Velocity[1]
//                 << ", " << this->Internals->Sensors[i].Velocity[2] << ")\n";    
//    os << indent << indent << "Velocity Rotation: (" << this->Internals->Sensors[i].VelocityRotation[0]
//                 << ", " << this->Internals->Sensors[i].VelocityRotation[1]
//                 << ", " << this->Internals->Sensors[i].VelocityRotation[2] 
//                 << ", " << this->Internals->Sensors[i].VelocityRotation[3] << ")\n";   
//    os << indent << indent << "VelocityRotationDelta: " << this->Internals->Sensors[i].VelocityRotationDelta << "\n";    
//    os << indent << indent << "Acceleration: (" << this->Internals->Sensors[i].Acceleration[0]
//                 << ", " << this->Internals->Sensors[i].Acceleration[1]
//                 << ", " << this->Internals->Sensors[i].Acceleration[2] << ")\n";    
//    os << indent << indent << "Acceleration Rotation: (" << this->Internals->Sensors[i].AccelerationRotation[0]
//                 << ", " << this->Internals->Sensors[i].AccelerationRotation[1]
//                 << ", " << this->Internals->Sensors[i].AccelerationRotation[2]
//                 << ", " << this->Internals->Sensors[i].AccelerationRotation[3] << ")\n";   
//    os << indent << indent << "AccelerationRotationDelta: " << this->Internals->Sensors[i].AccelerationRotationDelta << "\n";   
//    }
//}
