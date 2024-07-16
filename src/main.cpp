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

void loop() {

    if(counter < 20)
    {
        analogWrite(motor_br,0);
        counter++;
    }
    else if(counter < 30) {
        analogWrite(motor_br,10);
        counter++;
    }
    else if(counter < 60) {
        analogWrite(motor_br,0);
        counter++;
    }
    else {
        analogWrite(motor_br,0);

        return;
    }

    if (radio.available()) {
        Serial.println("radio available");
    }
    else {
        Serial.println("radio unavailable");
    }



    if (radio.available()) {

        radio.read(&data, sizeof(data));


        // char text[32] = "";
        // radio.read(&text, sizeof(text));
        // Serial.println(text);

        if(data.throttle > 135)
        {
            Serial.println(" throttle > 135 ");


//        analogWrite(motor_fl,10);
//        analogWrite(motor_fr,10);
//        analogWrite(motor_bl,10);
//            if(counter < 5)
//            {
//                analogWrite(motor_br,5);
//                counter++;
//            }
//            else {
//                analogWrite(motor_br,0);
//            }

        }
        else
        {
//        analogWrite(motor_fl,0);
//        analogWrite(motor_fr,0);
//        analogWrite(motor_bl,0);
//        analogWrite(motor_br,0);
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