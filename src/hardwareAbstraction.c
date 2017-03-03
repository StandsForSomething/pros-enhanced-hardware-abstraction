
#include "main.h"
//function prototype for function that shouldn't be used outside of this file
double getEncoderPriv(sensor encoderParent);

//initalizes an encoder as an IME
void initEncoderIme(encoder *encoder, int port, bool reversed,
                    motorGearing motorGearing, encoderGetType encoderGetType,
                    float gearRatio)
{
    initEncoder(encoder, port, 0, reversed, motorGearing, IME,
                            encoderGetType, gearRatio);
}

//initializes an encoder
void initEncoder(encoder *initEncoder, int port1, int port2, bool reversed,
                 motorGearing motorGearing, encoderType encoderType,
                 encoderGetType encoderGetType, float gearRatio)
{
    //copy the enocder we wan to initialize to a temporary encoder
    encoder tempEncoder = *initEncoder;
    
    //parent sensor
    tempEncoder.parent.port = port1;
    tempEncoder.parent.sensorType = OTHER;
    tempEncoder.parent.reversed = reversed;
    tempEncoder.parent.sensorValue = &getEncoderPriv;
    tempEncoder.parent.child = (struct child *)initEncoder;

    tempEncoder.encoderType = encoderType;

    //this is the default value to be retreived
    tempEncoder.encoderGetType = encoderGetType;

    switch(motorGearing)
    {
    case TORQUE: //defualt factory gearing
        tempEncoder.countsDivisor = 627.2;
        tempEncoder.velocityDivisor = 39.2;
        break;

    case SPEED: //highspeed gearing
        tempEncoder.countsDivisor = 392;
        tempEncoder.velocityDivisor = 24.5;
        break;

    case TURBO: //turbospeed gearing
        tempEncoder.countsDivisor = 261.333;
        tempEncoder.velocityDivisor = 16.333;
        break;
    }

    //if a two wire shaft encoder is being used.
    if(encoderType == TWO_WIRE)
    {
        tempEncoder.port2 = port2;
        tempEncoder.shaftEncoder = encoderInit(port1, port2, false);
    }

    //if a one wire shaft encoder is being used
    if(encoderType == ONE_WIRE)
    {
        //stub
    }
    
    //copy the temporary encoder to the real encoder.
    *initEncoder = tempEncoder;
}

void initEncoderTW(encoder *encoderToInit, int port1, int port2, bool reversed,
                   motorGearing motorGearing, encoderGetType encoderGetType,
                   float gearRatio)
{
    initEncoder(encoderToInit, port1, port2, reversed, motorGearing, TWO_WIRE,
                encoderGetType, gearRatio);
}

int motorGroupAdd(motor *motorToAdd, motorGroup *groupToChange)
{
    motorGroup tempGroup = *groupToChange;
    for(int i = 0; i < 10; i++)
    {
        if(!tempGroup.group[i])
        {
            tempGroup.group[i] = motorToAdd;
            
            return 0;
        }

    }

    return 1;
}

void setMotorGroup(motorGroup motorGroup, int speed)
{
    for(int i = 0; i < 10; i++)
    {
        if(motorGroup.group[i])
            setMotor(*motorGroup.group[i], speed);
    }
}

void setMotor(motor motor, int speed)
{
    motorSet(motor.port, motor.reversed ? -speed : speed);
}

void setMotorBVVC(motor motor, int speed)
{
    if(motor.portType == CORTEX)
    {
        setMotor(motor, (powerLevelMain() * 100) * -0.0117904 + 147.22);
    }

    else if(motor.portType == POWER_EXPANDER)
    {
        setMotor(motor, (getSensor(powerExpand) / 70) * -0.0117904 + 147.22);
    }
}

void writeDigital(sensor sensor, bool value)
{
    //if HIGH should be written as LOW and vise versa
    if(sensor.reversed)
        //make sure the correct value is written
        digitalWrite(sensor.port, value ? HIGH : LOW);

    //if HIGH should be written as HIGH and LOw should be written as LOW
    else
        //write the value
        digitalWrite(sensor.port, value);
}

double getSensor(sensor sensor)
{
    double sensorValue;

    //if sensor is digital and not reversed
    if(sensor.sensorType == DIGITAL && !sensor.reversed)
        return(digitalRead(sensor.port));

    //if the sensor is digital and reversed
    else if(sensor.sensorType == DIGITAL)
        return digitalRead(sensor.port) ? false : true;

    //if the sensor is analog
    else if(sensor.sensorType == ANALOG)
        sensorValue = analogRead(sensor.port);

    //if the sensor is another type use it's own specific function
    //(sensor.sensorValue() is a pointer)
    else if(sensor.sensorType == OTHER)
        return sensor.sensorValue(sensor);

    //if the sensorType is something un expected
    else
        sensorValue = 0;

    //reverse the sensor value if needed.
    return sensor.reversed ? -sensorValue : sensorValue;
}

//a wrapper of getEncoder() but with sensor as an argument to be compatible with
//getSensor().
//to prevent someone trying to pass a sensor that isn't an encoder to this
//function, this is private to this file (no function prototype in a header
//file).
double getEncoderPriv(sensor encoderParent)
{
    encoder *tempEncoder = (encoder *)encoderParent.child;
    return getEncoder(*tempEncoder);
}

//a wrapper for getEncoderMode with the encoder's default encoderGetType.
double getEncoder(encoder encoder)
{
    return getEncoderMode(encoder, encoder.encoderGetType);
}

//returns the value of the given encoder.
double getEncoderMode(encoder encoder, encoderGetType encoderGetType)
{
    double returnValue = 0.0;

    //in addition to returnValue since certain functions used expect an int
    //pointer as an argument.
    int sensorValue = 0;

    switch(encoder.encoderType)
    {
    case IME:
        switch(encoderGetType)
        {
        case COUNTS: //retreive the cumlative ticks
            imeGet(encoder.parent.port, &sensorValue);
            returnValue = sensorValue;
            break;

        case VELOCITY: //retrieve the velocity
            imeGetVelocity(encoder.parent.port, &sensorValue);
            returnValue = sensorValue;
            break;

        case ROTATIONS: //retrieve the number of rotations
            imeGet(encoder.parent.port, &sensorValue);

            //calculate the number of rotations
            returnValue = (sensorValue / encoder.countsDivisor) * encoder.gearRatio;
            break;
            

        case RPM: //retrieve the rotations per minute
            imeGetVelocity(encoder.parent.port, &sensorValue);

            //calculcate the RPM
            returnValue = (sensorValue / encoder.countsDivisor) * encoder.gearRatio;
            
        } //switch encoder.encoderGetType
        
        break; //switch encoderGetType, case IME

    case TWO_WIRE:
        switch(encoderGetType)
        {
        case COUNTS:
            returnValue = encoderGet(encoder.shaftEncoder);
            break;
            
        case VELOCITY:
            //stub
            break;

        case ROTATIONS:
            returnValue = encoderGet(encoder.shaftEncoder) / 360.0;
            break;

        case RPM:
            //stub
            break;
            
        } //switch encoderGetType

        break; //switch encoder.encoderType, case TWO_WIRE

    case ONE_WIRE:
        switch(encoderGetType)
        {
            
        case COUNTS:
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

    
    return encoder.parent.reversed ? -returnValue : returnValue;
}
