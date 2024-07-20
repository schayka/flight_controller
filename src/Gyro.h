#ifndef GYRO_H
#define GYRO_H
#include <stdint.h>


struct Gyro {

    float RateRoll, RatePitch, RateYaw;
    int16_t GyroX, GyroY, GyroZ;
    float RateCalibrationRoll = 0, RateCalibrationPitch = 0, RateCalibrationYaw = 0;
    int RateCalibrationNumber;

    void setup();

    void update_data(bool calibration = false);

    void view_data();

    void calibrate();
};


#endif // GYRO_H
