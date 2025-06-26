#ifndef MOTORS_H
#define MOTORS_H

#include <misc.h>


struct MotorShield{
    int enable_pin;
    int left_pwm_pin, left_a_pin, left_b_pin;
    int right_pwm_pin, right_a_pin, right_b_pin;
    int MAX_SPEED = 255;

    MotorShield(){}

    void init(int en, int pwm_A, int a_1, int a_2, int pwm_B, int b_1, int b_2, int max_speed=255){
        enable_pin = en;
        left_pwm_pin = pwm_A;
        left_a_pin = a_1;
        left_b_pin = a_2;
        
        right_pwm_pin = pwm_B;
        right_a_pin = b_1;
        right_b_pin = b_2;
        
        MAX_SPEED = max_speed;
        
        pinMode(enable_pin, OUTPUT);
        pinMode(left_pwm_pin, OUTPUT); pinMode(left_a_pin, OUTPUT); pinMode(left_b_pin, OUTPUT);
        pinMode(right_pwm_pin, OUTPUT); pinMode(right_a_pin, OUTPUT); pinMode(right_b_pin, OUTPUT);
        stopMotors();
    }

    void stopMotors(){
        controlMotors(0, 0);
        digitalWrite(enable_pin, LOW);
    }

    void setMotorSpeed(int motor, int speed){
        bool reverse = speed<0;
        speed = constrain(abs(speed), 0, MAX_SPEED);

        if(motor == 0){
            //Left
            analogWrite(left_pwm_pin, speed);
            digitalWrite(left_a_pin, !reverse);
            digitalWrite(left_b_pin, reverse);
        }
        else{
            //Right
            analogWrite(right_pwm_pin, speed);
            digitalWrite(right_a_pin, !reverse);
            digitalWrite(right_b_pin, reverse);
        }
    }

    void controlMotors(int left_speed, int right_speed){
        digitalWrite(enable_pin, HIGH);
        setMotorSpeed(0, left_speed);
        setMotorSpeed(1, right_speed);
    }

};


#endif