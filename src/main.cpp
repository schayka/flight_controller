/*
* Arduino Flight Controller
 *
 * /dev/ttyUSB0
 *
*/

#include <SPI.h>
#include <Wire.h>
#include "ext/RF24/nRF24L01.h"
#include "ext/RF24/RF24.h"

RF24 radio(7, 10); // CE, CSN

struct TransmitterInData {
    byte throttle;
    byte yaw;
    byte pitch;
    byte roll;
    byte AUX1;
    byte AUX2;
};

const byte address[6] = "00001";

int motor_fl = 6;
int motor_fr = 9;
int motor_bl = 5;
int motor_br = 3;

int motor_gain = 0;

double RateRoll, RatePitch, RateYaw;

TransmitterInData data;

void gyro_signals()
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
    int16_t GyroX=Wire.read()<<8 | Wire.read();
    int16_t GyroY=Wire.read()<<8 | Wire.read();
    int16_t GyroZ=Wire.read()<<8 | Wire.read();
    RateRoll=(float)GyroX/65.5;
    RatePitch=(float)GyroY/65.5;
    RateYaw=(float)GyroZ/65.5;

}

void reset_data()
{
    data.throttle = 0;
    data.yaw = 127;
    data.pitch = 127;
    data.roll = 127;
    data.AUX1 = 0;
    data.AUX2 = 0;
}

void test_motors()
{
    if (radio.available()) {
        radio.read(&data, sizeof(data));
    }
    else
    {
        reset_data();
    }

    if(data.throttle > 135)
    {
        int val = data.throttle - 135;
        motor_gain = val * (255 / (255 - 136) );
        motor_gain = constrain(motor_gain, 0, 255);
        Serial.print(" throttle > 135 |  ");
        Serial.println(motor_gain);
        analogWrite(motor_fl,motor_gain);
        analogWrite(motor_fr,motor_gain);
        analogWrite(motor_bl,motor_gain);
        analogWrite(motor_br,motor_gain);
    }
    else
    {
        analogWrite(motor_fl,0);
        analogWrite(motor_fr,0);
        analogWrite(motor_bl,0);
        analogWrite(motor_br,0);
    }
}

void view_TransmitterInData()
{
    Serial.print("throttle : ");
    Serial.print(data.throttle);
    Serial.print("/ yaw : ");
    Serial.print(data.yaw);
    Serial.print(" / pitch :");
    Serial.print(data.pitch);
    Serial.print(" / roll : ");
    Serial.print(data.roll);
    Serial.print(" / AUX1 : ");
    Serial.print(data.AUX1);
    Serial.print(" / AUX2 : ");
    Serial.println(data.AUX2);
}

void setup() {

    reset_data();

    pinMode(motor_fl, OUTPUT);
    pinMode(motor_fr, OUTPUT);
    pinMode(motor_bl, OUTPUT);
    pinMode(motor_br, OUTPUT);

    Serial.begin(9600);
    radio.begin();
    radio.openReadingPipe(0, address);
    radio.setPALevel(RF24_PA_MIN);
    radio.startListening();

    Wire.begin();
    delay(250);

    Wire.beginTransmission(0x68);
    Wire.write(0x6B);
    Wire.write(0x00);
    Wire.endTransmission();

}

void loop() {

    if (radio.available()) {
        Serial.println("radio available");
    }
    else {
        Serial.println("radio unavailable");
    }

    test_motors();
    view_TransmitterInData();

    gyro_signals();
    Serial.print("Roll rate [°/s]= ");
    Serial.print(RateRoll);
    Serial.print(" Pitch Rate [°/s]= ");
    Serial.print(RatePitch);
    Serial.print(" Yaw Rate [°/s]= ");
    Serial.println(RateYaw);
}