#ifndef GYRO_H
#define GYRO_H
#include <stdint.h>


struct Gyro {

    int16_t gRawX, gRawY, gRawZ;
    float gX, gY, gZ;
    float gCalibrationX = 0, gCalibrationY = 0, gCalibrationZ = 0;
    // todo flat surface only ?
    float gAngleX = 0, gAngleY = 0, gAngleZ = 0;
    int i;
    unsigned long previousTime = 0, currentTime;

    int16_t AccXLSB,AccYLSB,AccZLSB;
    float AccX, AccY, AccZ;
    float AngleRoll, AnglePitch;
    float AccXErr, AccYErr, AccZErr;

    void setup();

    void update_data(bool calibration = false);

    void view_gyro();

    void view_gyro_angles();

    void view_acc();

    void view_acc_angles();

    void calibrate();

    void mpu6050_init();

    void mpu6050_update_data(bool calibration);

};


#endif // GYRO_H
