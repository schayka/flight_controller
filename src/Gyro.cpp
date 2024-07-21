#include "Gyro.h"
#include <Wire.h>
#include <Arduino.h>

void Gyro::setup()
{
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