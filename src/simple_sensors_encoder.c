#include "main.h"
#include "simple_sensors.h"
#include "simple_sensors_encoder.h"

//a wrapper for getEncoderMode with the encoder's default encoderGetType.
double _getEncoder(simpleSensor encoder) {
	encoderSimpleSensor *encoderData = (encoderSimpleSensor *)encoder.sensorData;
	return getEncoder(encoder, encoderData->encoderGetType);
}

bool initEncoder(simpleSensor *parent, int port2, motorGearing motorGearing,
								 encoderType encoderType, encoderGetType encoderGetType,
								 float gearRatio) {
	//copy the encoder we wan to initialize to a temporary encoder
	static encoderSimpleSensor tempEncoder;
	struct sensorData *tempSensorData;

	tempEncoder.encoderType = encoderType;
	parent->sensorValue = &_getEncoder;

	//this is the default value to be retrieved
	tempEncoder.encoderGetType = encoderGetType;

	switch(motorGearing) {
	case TORQUE: //default factory gearing
		tempEncoder.ticksDivisor = 627.2;
		tempEncoder.velocityDivisor = 39.2;
		break;

	case SPEED: //highspeed gearing
		tempEncoder.ticksDivisor = 392;
		tempEncoder.velocityDivisor = 24.5;
		break;

	case TURBO: //turbospeed gearing
		tempEncoder.ticksDivisor = 261.333;
		tempEncoder.velocityDivisor = 16.333;
		break;
	}

	//if a two wire shaft encoder is being used.
	if(encoderType == TWO_WIRE) {
		tempEncoder.port2 = port2;
		tempEncoder.shaftEncoder = encoderInit(parent->port, port2, false);
	}

	//if a one wire shaft encoder is being used
	if(encoderType == ONE_WIRE) {
		//stub
	}

	//copy the temporary encoder to the real encoder.
	tempSensorData = (struct sensorData *)&tempEncoder;
	parent->sensorData = tempSensorData;
}

//returns the value of the given encoder.
double getEncoder(simpleSensor encoder, encoderGetType encoderGetType) {
	double returnValue = 0.0;

	//in addition to returnValue since certain functions used expect an int
	//pointer as an argument.
	int sensorValue = 0;
	encoderSimpleSensor *encoderData = (encoderSimpleSensor *)encoder.sensorData;

	switch(encoderData->encoderType) {
	case IME:
		switch(encoderGetType) {
		case TICKS: //retrieve the cumlative ticks
			imeGet(encoder.port, &sensorValue);
			returnValue = sensorValue;
			break;

		case VELOCITY: //retrieve the velocity
			imeGetVelocity(encoder.port, &sensorValue);
			returnValue = sensorValue;
			break;

		case ROTATIONS: //retrieve the number of rotations
			imeGet(encoder.port, &sensorValue);

			//calculate the number of rotations
			returnValue = (sensorValue / encoderData->ticksDivisor) *
				encoderData->gearRatio;
			break;

		case RPM: //retrieve the rotations per minute
			imeGetVelocity(encoder.port, &sensorValue);

			//calculcate the RPM
			returnValue = (sensorValue / encoderData->ticksDivisor) *
				encoderData->gearRatio;

		} //switch encoder.encoderGetType

		break; //switch encoderGetType, case IME

	case TWO_WIRE:
		switch(encoderGetType) {
		case TICKS:
			returnValue = encoderGet(encoderData->shaftEncoder);
			break;

		case VELOCITY:
			//stub
			break;

		case ROTATIONS:
			returnValue = encoderGet(encoderData->shaftEncoder) / 360.0;
			break;

		case RPM:
			//stub
			break;

		} //switch encoderGetType

		break; //switch encoder.encoderType, case TWO_WIRE

	case ONE_WIRE:
		switch(encoderGetType) {

		case TICKS:
			//stub
			break;

		case VELOCITY:
			//stub
			break;

		case ROTATIONS:
			//stub
			break;

		case RPM:
			//stub
			break;

		} //switch encoderGetType

		break; //switch encoderType, case ONE_WIRE
	}

	return encoder.reversed ? -returnValue : returnValue;
}
