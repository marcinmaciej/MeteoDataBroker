//
// Created by Marcin Guziołek on 04.04.18.
//

#ifndef METEOSTATIONDRIVERS_SERIALPORT_H
#define METEOSTATIONDRIVERS_SERIALPORT_H

#include <iostream>

using  namespace std;

class SerialPort {

public:
    explicit SerialPort(string port = "/dev/ttyAMA0"); /* Default port for Raspbian */
    ~SerialPort() = default;

    /* Default serial raw mode with 8n1 frame no parity and baud rate 9600 */
    void setPort(string port = "/dev/ttyAMA0"); /* Default port for Raspbian */
    void setParity(string parity = "none");
    void setFlowControl(string flowControl = "none");
    void setSpeed(unsigned short speed = 9600); /* Default baud rate for Raspberry Pi B */
    void setDataBits(unsigned char dataBits = 8);
    void setStopBits(unsigned char stopBits = 1);



private:
    string port;
    string parity;
    string flowControl;
    unsigned short speed;
    unsigned char dataBits;
    unsigned char stopBits;

    string getPort();
    string getParity();
    string getFlowControl();
    unsigned short getSpeed();
    unsigned char getDataBits();
    unsigned char getStopBits();

};


#endif //METEOSTATIONDRIVERS_SERIALPORT_H
