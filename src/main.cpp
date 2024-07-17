/*
* Arduino Wireless Communication Tutorial
*       Example 1 - Receiver Code
*
* by Dejan Nedelkovski, www.HowToMechatronics.com
*
* Library: TMRh20/RF24, https://github.com/tmrh20/RF24/
*/

#include <SPI.h>
#include "ext/RF24/nRF24L01.h"
#include "ext/RF24/RF24.h"

RF24 radio(7, 10); // CE, CSN

const byte address[6] = "00001";

struct MyData {
    byte throttle;
    byte yaw;
    byte pitch;
    byte roll;
    byte AUX1;
    byte AUX2;
};

int motor_fl = 6;
int motor_fr = 9;
int motor_bl = 5;
int motor_br = 3;

int counter = 1;

MyData data;

void resetData()
{
    //This are the start values of each channal
    // Throttle is 0 in order to stop the motors
    //127 is the middle value of the 10ADC.

    data.throttle = 0;
    data.yaw = 127;
    data.pitch = 127;
    data.roll = 127;
    data.AUX1 = 0;
    data.AUX2 = 0;
}

void setup() {

    resetData();

    pinMode(motor_fl, OUTPUT);
    pinMode(motor_fr, OUTPUT);
    pinMode(motor_bl, OUTPUT);
    pinMode(motor_br, OUTPUT);

    Serial.begin(9600);
    radio.begin();
    radio.openReadingPipe(0, address);
    radio.setPALevel(RF24_PA_MIN);
    radio.startListening();

}

int motor_gain = 0;

void loop() {



    if (radio.available()) {
        Serial.println("radio available");
    }
    else {
        Serial.println("radio unavailable");
    }

    if (radio.available()) {
        radio.read(&data, sizeof(data));
        if(data.throttle > 135)
        {
            motor_gain = data.throttle / 9;
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