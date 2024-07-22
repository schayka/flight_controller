#include "Gyro.h"
#include <Wire.h>
#include <Arduino.h>

void Gyro::setup()
{
    AccXErr = 0.0;
    AccYErr = 0.0;
    AccZErr = 0.0;

    this->mpu6050_init();

    calibrate();
}

void Gyro::update_data(bool calibration)
{
    mpu6050_update_data(calibration);
}

void Gyro::calibrate()
{
    // todo works for both bigger and lower values?
    for(i = 0;
        i < 2000; i++)
    {
        update_data(true);
        gCalibrationX += gX;
        gCalibrationY += gY;
        gCalibrationZ += gZ;
        delay(1);
    }
    gCalibrationX /= 2000;
    gCalibrationY /= 2000;
    gCalibrationZ /= 2000;
}

void Gyro::view_gyro()
{
    // todo need to begin Serial  ?
    Serial.print("RollRate[°/s]= ");
    Serial.print(gX);
    Serial.print(" PitchRate[°/s]= ");
    Serial.print(gY);
    Serial.print(" YawRate[°/s]= ");
    Serial.println(gZ);
}

void Gyro::view_gyro_angles()
{
    Serial.print("X angle [°]= ");
    Serial.print(gAngleX);
    Serial.print(" Y angle [°]= ");
    Serial.print(gAngleY);
    Serial.print(" Z angle [°]= ");
    Serial.println(gAngleZ);
}

void Gyro::view_acc()
{
    // todo need to begin Serial  ?
    Serial.print("Acceleration X [g]= ");
    Serial.print(AccX);
    Serial.print(" Acceleration Y [g]= ");
    Serial.print(AccY);
    Serial.print(" Acceleration Z [g]= ");
    Serial.println(AccZ);
}

void Gyro::view_acc_angles()
{
    Serial.print("X angle [°]= ");
    Serial.print(AngleRoll);
    Serial.print(" Y angle [°]= ");
    Serial.println(AnglePitch);
}

// MPU-6050
void Gyro::mpu6050_init()
{
    // Начало передачи на устройство с адресом 0x68 (MPU-6050)
    Wire.beginTransmission(0x68);
    // Адрес регистра питания (PWR_MGMT_1)
    Wire.write(0x6B);
    // Запись 0 в регистр (разбудить MPU6050)
    Wire.write(0x00);
    // Завершение передачи
    Wire.endTransmission();
}

void Gyro::mpu6050_update_data(bool calibration)
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
    gRawX=Wire.read()<<8 | Wire.read();
    gRawY=Wire.read()<<8 | Wire.read();
    gRawZ=Wire.read()<<8 | Wire.read();

    // Full Scale Range: ± 500 °/s, LSB Sensitivity: 65.5 LSB/°/s
    gX = ( (float)gRawX/65.5 ) - (calibration ? 0 : gCalibrationX );
    gY= ((float)gRawY / 65.5 ) - (calibration ? 0 : gCalibrationY );
    gZ= ((float)gRawZ / 65.5 ) - (calibration ? 0 : gCalibrationZ );

    // integrate to get current g angles

    if(calibration == false)
    {
        currentTime = micros();
        if(previousTime == 0) previousTime = currentTime;
//    Serial.print(gX);
//    Serial.print('/');
//    Serial.print((currentTime - previousTime));
//    Serial.print('/');
//    Serial.println(( (currentTime - previousTime) / 1000000.0 ) );
        gAngleX = gAngleX + ( gX * ( (currentTime - previousTime) / 1000000.0 ) );
        gAngleY = gAngleY + ( gY * ( (currentTime - previousTime) / 1000000.0 ) );
        gAngleZ = gAngleZ + ( gZ * ( (currentTime - previousTime) / 1000000.0 ) );
        previousTime = currentTime;
    }




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
    AccZLSB = Wire.read() << 8 | Wire.read();

    // Full Scale Range: ±8g, LSB Sensitivity: 4096 LSB/g
    // AccX/Y/Z/Err - measurement error, setting manually for each mpu module
    AccX = ( (float)AccXLSB / 4096 ) + AccXErr;
    AccY = ( (float)AccYLSB / 4096 ) + AccYErr;
    AccZ = ( (float)AccZLSB / 4096 ) + AccZErr;

    // todo не понимаю эти формулы угла через проекции на 3 оси
    AngleRoll=atan(AccY/sqrt(AccX*AccX+AccZ*AccZ))*1/(3.142/180);
    AnglePitch=-atan(AccX/sqrt(AccY*AccY+AccZ*AccZ))*1/(3.142/180);
}

