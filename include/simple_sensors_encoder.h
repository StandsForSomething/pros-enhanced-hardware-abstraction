#ifndef SIMPLE_SENSORS_ENCODER_H_
#define SIMPLE_SENSORS_ENCODER_H_
#include "simple_sensors.h"

typedef enum encoderType {IME, TWO_WIRE, ONE_WIRE}encoderType;
typedef enum encoderGetType {TICKS, VELOCITY, ROTATIONS, RPM}encoderGetType;
typedef enum motorGearing {TORQUE, SPEED, TURBO}motorGearing;

//this should be initialized with initEncoder().
typedef struct encoderSimpleSensor
{
	encoderType encoderType;
	encoderGetType encoderGetType;

	//for any external gearing
	float gearRatio;

	char port2;

	/*
		INTERNAL USE
	*/
	float ticksDivisor;
	float velocityDivisor;
	Encoder shaftEncoder;

}encoderSimpleSensor;

bool initEncoder(simpleSensor *parent, int port2, motorGearing motorGearing,
								 encoderType encoderType, encoderGetType encoderGetType,
								 float gearRatio);
double getEncoder(simpleSensor encoder, encoderGetType encoderGetType);
#endif
