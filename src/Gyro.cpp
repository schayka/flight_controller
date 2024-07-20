#include "Gyro.h"
#include <Wire.h>
#include <Arduino.h>

void Gyro::setup()
{
    Wire.beginTransmission(0x68);
    Wire.write(0x6B);
    Wire.write(0x00);
    Wire.endTransmission();

    calibrate();
}

void Gyro::update_data(bool calibration)
{
    // start I2C communication
    Wire.beginTransmission(0x68);
    // turn on low pass filter
    Wire.write(0x1A);
    Wire.write(0x05);
    // finish I2C communication
    Wire.endTransmission();

    // set the sensitivity scale factor
    Wire.beginTransmission(0x68);
    Wire.write(0x1B);
    Wire.write(0x8);
    Wire.endTransmission();

    // access registers storing gyro measurements
    Wire.beginTransmission(0x68);
    Wire.write(0x43);
    Wire.endTransmission();

    // get the data

    Wire.requestFrom(0x68,6);
    GyroX=Wire.read()<<8 | Wire.read();
    GyroY=Wire.read()<<8 | Wire.read();
    GyroZ=Wire.read()<<8 | Wire.read();
    RateRoll= ( (float)GyroX/65.5 ) - ( calibration ? 0 : RateCalibrationRoll );
    RatePitch=( (float)GyroY/65.5 ) - ( calibration ? 0 : RateCalibrationPitch );
    RateYaw=( (float)GyroZ/65.5 ) - ( calibration ? 0 : RateCalibrationYaw );
}

void Gyro::calibrate()
{
    for(RateCalibrationNumber = 0;
            RateCalibrationNumber < 2000; RateCalibrationNumber++)
    {
        update_data(true);
        RateCalibrationRoll += RateRoll;
        RateCalibrationPitch += RatePitch;
        RateCalibrationYaw += RateYaw;
        delay(1);
    }
    RateCalibrationRoll /= 2000;
    RateCalibrationPitch /= 2000;
    RateCalibrationYaw /= 2000;
}

void Gyro::view_data()
{
    // todo need to begin Serial  ?
    Serial.print("Roll rate [°/s]= ");
    Serial.print(RateRoll);
    Serial.print(" Pitch Rate [°/s]= ");
    Serial.print(RatePitch);
    Serial.print(" Yaw Rate [°/s]= ");
    Serial.println(RateYaw);
}