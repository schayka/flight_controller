#ifndef GYRO_H
#define GYRO_H
#include <stdint.h>


struct Gyro {

    int16_t gRawX, gRawY, gRawZ;
    float gX, gY, gZ;
    float gCalibrationX = 0, gCalibrationY = 0, gCalibrationZ = 0;
    // todo flat surface only ?
    // during controller init, before motors start, make cfK value lower to fit real angles by accelerometer data
    float gAngleX = 0, gAngleY = 0, gAngleZ = 0;
    int i;
    unsigned long previousTime = 0, currentTime;

    int16_t aRawX,aRawY,aRawZ;
    float aX, aY, aZ;
    float aAngleX, aAngleY;
    float AccXErr, AccYErr, AccZErr;

    unsigned long cfPrevTime = 0, cfCurrentTime;
    float cfAngleX = 0, cfAngleY = 0;
    float cfAngleX2 = 0, cfAngleY2 = 0;
    float cfK = 0.99;

    void setup();

    void update_data(bool calibration = false);

    void view_gyro();

    void view_gyro_angles();

    void view_acc();

    void view_acc_angles();

    void view_cf_angles();

    void view_angles(bool x = true, bool y = false);

    void calibrate();

    void mpu6050_init();

    void mpu6050_update_data(bool calibration);

    void update_complementary_filter();
};


#endif // GYRO_H
