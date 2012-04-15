/*=========================================================================

Name:        vtkVRPNTrackerCustomSensor.h

Author:      Alexis YL Chan

Copyright:   The Renaissance Computing Institute (RENCI)

License:     Licensed under the RENCI Open Source Software License v. 1.0.

See included License.txt or 
http://www.renci.org/resources/open-source-software-license
for details.

=========================================================================*/
// .NAME vtkVRPNTrackerCustomSensor
// .SECTION Description
// vtkVRPNTrackerCustomSensor is a subclass of vtkVRPNTracker. Wraps a sensor of a vrpn
// Tracker as a vtkVRPNTracker with its own vtkVRPNTrackerCameraStyle
// .SECTION see also
// vtkVRPNTracker vtkInteractionDeviceManager vtkDeviceInteractorStyle

#ifndef __vtkVRPNTrackerCustomSensor_h
#define __vtkVRPNTrackerCustomSensor_h

#include "vtkVRPNTracker.h"

class vtkMatrix4x4;


class VTK_INTERACTIONDEVICE_EXPORT vtkVRPNTrackerCustomSensor : public vtkVRPNTracker
{
public: 
	static vtkVRPNTrackerCustomSensor* New();
	vtkTypeRevisionMacro(vtkVRPNTrackerCustomSensor,vtkVRPNTracker);
	void SetSensorIndex(int sensorIndex);
	int GetSensorIndex();

	// Description:
	// Initialize the device
	virtual int Initialize();

	// Description:
	// The number of sensors to use
	void SetNumberOfSensors(int num);
	int GetNumberOfSensors();

	// Description:
	// Set/Get the tracker information
	void SetPosition(double* position);
	double* GetPosition();
	void SetRotation(double* rotation);
	double* GetRotation();

	void SetVelocity(double* velocity);
	double* GetVelocity();
	void SetVelocityRotation(double* rotation);
	double* GetVelocityRotation();
	void SetVelocityRotationDelta(double delta);
	double GetVelocityRotationDelta();

	void SetAcceleration(double* acceleration);
	double* GetAcceleration();
	void SetAccelerationRotation(double* rotation);
	double* GetAccelerationRotation();
	void SetAccelerationRotationDelta(double delta);
	double GetAccelerationRotationDelta();

	void SetTrackerToVTKTransform(vtkMatrix4x4* trackerToVTKTransform);
	vtkMatrix4x4* GetTrackerToVTKTransform();


protected:
	vtkVRPNTrackerCustomSensor();
	~vtkVRPNTrackerCustomSensor();

	vtkMatrix4x4* trackerToVTKTransform;

private:
	static const int INTERNAL_SENSOR = 0;
	void InitializeSensors();
	vtkVRPNTrackerCustomSensor(const vtkVRPNTrackerCustomSensor&);  // Not implemented.
	void operator=(const vtkVRPNTrackerCustomSensor&);  // Not implemented.
};

#endif
