#ifndef SIMPLE_SENSORS_H_
#define SIMPLE_SENSORS_H_

typedef enum sensorType {ANALOG, DIGITAL, DIGITAL_OUT, OTHER}sensorType;

typedef struct simpleSensor
{
	char port;
	sensorType sensorType;
	bool reversed;

	//if sensorType is OTHER getSensor() will run whatever this points to.
	double (*sensorValue)();

	struct sensorData *sensorData;
}simpleSensor;

double getSensor(simpleSensor sensor);
bool setOutput(simpleSensor sensor, bool value);
#endif
