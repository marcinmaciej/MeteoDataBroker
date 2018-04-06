//
// Created by Marcin Guziołek on 04.04.18.
//

#ifndef METEOSTATIONDRIVERS_SERIALPORT_H
#define METEOSTATIONDRIVERS_SERIALPORT_H

#include <iostream>

using namespace std;

class SerialPort
{

public:
    explicit SerialPort(const string &port = "/dev/ttyAMA0");

    virtual ~SerialPort() = default;


    /* Default serial raw mode with 8n1 frame no parity and baud rate 9600 */
    void setPort(const string &port = "/dev/ttyAMA0"); /* Default port for Raspbian */

    void setSpeed(unsigned short speed = 9600); /* Default baud rate for Raspberry Pi B */

    void setDataBits(unsigned char dataBits = 8);

    void setStopBits(unsigned char stopBits = 1);

    void setParity(const string &parity = "none");

    void setFlowControl(const string &flowControl = "none");


    const string &getPort() const;

    const string &getParity() const;

    const string &getFlowControl() const;

    unsigned short getSpeed() const;

    unsigned char getDataBits() const;

    unsigned char getStopBits() const;

private:
    string port;
    string parity;
    string flowControl;
    unsigned short speed;
    unsigned char dataBits;
    unsigned char stopBits;


};


#endif //METEOSTATIONDRIVERS_SERIALPORT_H
