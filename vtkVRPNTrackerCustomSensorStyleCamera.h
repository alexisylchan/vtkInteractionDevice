/*=========================================================================

Name:        vtkVRPNTrackerCustomSensorStyleCamera.h

Author:      Alexis YL Chan

Copyright:   The Renaissance Computing Institute (RENCI)

License:     Licensed under the RENCI Open Source Software License v. 1.0.

See included License.txt or 
http://www.renci.org/resources/open-source-software-license
for details.

=========================================================================*/
// .NAME vtkVRPNTrackerCustomSensorStyleCamera
// .SECTION Description
// vtkVRPNTrackerCustomSensorStyleCamera extends vtkVRPNTrackerStyleCamera to 
// update camera view for vtkVRPNTrackerCustomSensor (1 sensor tracker)
// .SECTION see also
// vtkVRPNTrackerStyleCamera vtkVRPNTrackerCustomSensor

#ifndef __vtkVRPNTrackerCustomSensorStyleCamera_h
#define __vtkVRPNTrackerCustomSensorStyleCamera_h

#include "vtkVRPNTrackerStyleCamera.h"
#include "vtkVRPNTrackerCustomSensor.h"

class VTK_INTERACTIONDEVICE_EXPORT vtkVRPNTrackerCustomSensorStyleCamera : public vtkVRPNTrackerStyleCamera
{
public:
	static vtkVRPNTrackerCustomSensorStyleCamera* New();
	vtkTypeRevisionMacro(vtkVRPNTrackerCustomSensorStyleCamera,vtkVRPNTrackerStyleCamera);
	// Description:
	// Perform interaction based on an event
	virtual void OnEvent(vtkObject* caller, unsigned long eid, void* callData);

	// Description:
	// Set the tracker receiving events from
	void SetTracker(vtkVRPNTrackerCustomSensor*);
	void SetFrontOfMonitor(bool frontOfMonitor);
	void SetDistanceFromRemote(double distanceFromRemote);

protected:
	vtkVRPNTrackerCustomSensorStyleCamera();
	~vtkVRPNTrackerCustomSensorStyleCamera();

	virtual void OnTracker(vtkVRPNTrackerCustomSensor*);

private:
	bool frontOfMonitor;
	double distanceFromRemote;
	vtkVRPNTrackerCustomSensorStyleCamera(const vtkVRPNTrackerCustomSensorStyleCamera&);  // Not implemented.
	void operator=(const vtkVRPNTrackerCustomSensorStyleCamera&);  // Not implemented.
};

#endif