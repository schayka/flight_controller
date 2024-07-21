#ifndef GYRO_H
#define GYRO_H
#include <stdint.h>


struct Gyro {

    float RateRoll, RatePitch, RateYaw;
    int16_t GyroX, GyroY, GyroZ;
    float RateCalibrationRoll = 0, RateCalibrationPitch = 0, RateCalibrationYaw = 0;
    int i;

    int16_t AccXLSB,AccYLSB,AccZLSB;
    float AccX, AccY, AccZ;
    float AngleRoll, AnglePitch;
    float AccXErr, AccYErr, AccZErr;

    void setup();

    void update_data(bool calibration = false);

    void view_data_gyro();

    void view_data_acc_g();

    void view_data_acc_angles();

    void calibrate();
};


#endif // GYRO_H
