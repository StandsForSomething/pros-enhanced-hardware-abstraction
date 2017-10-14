#include "main.h"
#include "simple_sensors.h"

bool writeDigital(simpleSensor sensor, bool value) {
	if(sensor.sensorType != DIGITAL_OUT) {
		return false;
	}

	//if HIGH should be written as LOW and vise versa
	if(sensor.reversed) {
		//make sure the correct value is written
		digitalWrite(sensor.port, value ? HIGH : LOW);
	}

	//if HIGH should be written as HIGH and LOw should be written as LOW
	else {
		//write the value
		digitalWrite(sensor.port, value);
	}
}

double getSensor(simpleSensor sensor)
{
	double sensorValue;

	//if sensor is digital and not reversed
	if(sensor.sensorType == DIGITAL && !sensor.reversed) {
		return(digitalRead(sensor.port));
	}

	//if the sensor is digital and reversed
	else if(sensor.sensorType == DIGITAL) {
		return digitalRead(sensor.port) ? false : true;
	}

	//if the sensor is analog
	else if(sensor.sensorType == ANALOG) {
		sensorValue = analogRead(sensor.port);
		return sensor.reversed ? 4095 - sensorValue : sensorValue;
	}

	//if the sensor is another type use it's own specific function
	//(sensor.sensorValue() is a pointer)
	else if(sensor.sensorType == OTHER) {
		return sensor.sensorValue(sensor);
	}

	//if the sensorType is something un expected
	else {
		return 0;
	}
}
