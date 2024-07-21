#include "Gyro.h"
#include <Wire.h>
#include <Arduino.h>

void Gyro::setup()
{
    AccXErr = 0.0;
    AccYErr = 0.0;
    AccZErr = 0.0;

    // Начало передачи на устройство с адресом 0x68 (MPU-6050)
    Wire.beginTransmission(0x68);
    // Адрес регистра питания (PWR_MGMT_1)
    Wire.write(0x6B);
    // Запись 0 в регистр (разбудить MPU6050)
    Wire.write(0x00);
    // Завершение передачи
    Wire.endTransmission();

    calibrate();
}

void Gyro::update_data(bool calibration)
{
    // Начало передачи на устройство с адресом 0x68 (MPU-6050)
    Wire.beginTransmission(0x68);
    // Регистр конфигурации (CONFIG (адрес 0x1A) )
    Wire.write(0x1A);
    // Установить фильтр с частотой среза 10 Гц
    Wire.write(0x05);
    // finish I2C communication
    Wire.endTransmission();

    // // Установить диапазон гироскопа на ± 500 °/s
    Wire.beginTransmission(0x68);
    // Регистр конфигурации гироскопа (GYRO_CONFIG (адрес 0x1B) )
    Wire.write(0x1B);
    // значение для 500 °/s - 1, в бинарном виде - 00001000, т.е. 8
    Wire.write(0x8);
    Wire.endTransmission();

    // access registers storing gyro measurements
    Wire.beginTransmission(0x68);
    Wire.write(0x43);
    Wire.endTransmission(false);

    // get the data from registers storing gyro measurements
    Wire.requestFrom(0x68,6);
    GyroX=Wire.read()<<8 | Wire.read();
    GyroY=Wire.read()<<8 | Wire.read();
    GyroZ=Wire.read()<<8 | Wire.read();
    // Full Scale Range: ± 500 °/s, LSB Sensitivity: 65.5 LSB/°/s
    RateRoll= ( (float)GyroX/65.5 ) - ( calibration ? 0 : RateCalibrationRoll );
    RatePitch=( (float)GyroY/65.5 ) - ( calibration ? 0 : RateCalibrationPitch );
    RateYaw=( (float)GyroZ/65.5 ) - ( calibration ? 0 : RateCalibrationYaw );

    // конфигурация вывода акселерометра
    Wire.beginTransmission(0x68);
    // ACCEL_CONFIG
    Wire.write(0x1C);
    // значение для  +/- 8g (2), 000 10 000 (16 или x10)
    Wire.write(0x10);
    Wire.endTransmission();

    // access registers storing accelerometer values (3B)
    Wire.beginTransmission(0x68);
    Wire.write(0x3B);
    Wire.endTransmission(false);

    // get data from register storing accelerometer values
    Wire.requestFrom(0x68,6);
    AccXLSB = Wire.read() << 8 | Wire.read();
    AccYLSB = Wire.read() << 8 | Wire.read();
    AccZLSB = Wire.read() << 8  | Wire.read();

    // Full Scale Range: ±8g, LSB Sensitivity: 4096 LSB/g
    // AccX/Y/Z/Err - measurement error, setting manually for each mpu module
    AccX = ( (float)AccXLSB / 4096 ) + AccXErr;
    AccY = ( (float)AccYLSB / 4096 ) + AccYErr;
    AccZ = ( (float)AccZLSB / 4096 ) + AccZErr;

    // todo не понимаю эти формулы угла через проекции на 3 оси
    AngleRoll=atan(AccY/sqrt(AccX*AccX+AccZ*AccZ))*1/(3.142/180);
    AnglePitch=-atan(AccX/sqrt(AccY*AccY+AccZ*AccZ))*1/(3.142/180);
}

void Gyro::calibrate()
{
    for(i = 0;
        i < 2000; i++)
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

void Gyro::view_data_gyro()
{
    // todo need to begin Serial  ?
    Serial.print("Roll rate [°/s]= ");
    Serial.print(RateRoll);
    Serial.print(" Pitch Rate [°/s]= ");
    Serial.print(RatePitch);
    Serial.print(" Yaw Rate [°/s]= ");
    Serial.println(RateYaw);
}

void Gyro::view_data_acc_g()
{
    // todo need to begin Serial  ?
    Serial.print("Acceleration X [g]= ");
    Serial.print(AccX);
    Serial.print(" Acceleration Y [g]= ");
    Serial.print(AccY);
    Serial.print(" Acceleration Z [g]= ");
    Serial.println(AccZ);
}

void Gyro::view_data_acc_angles()
{
    Serial.print("Roll angle [°]= ");
    Serial.print(AngleRoll);
    Serial.print(" Pitch angle [°]= ");
    Serial.println(AnglePitch);
}