//
// Created by Marcin Guziołek on 04.04.18.
//

#ifndef METEOSTATIONDRIVERS_SERIALPORT_H
#define METEOSTATIONDRIVERS_SERIALPORT_H

#include <iostream>

#define RASPBERRYPI false

using namespace std;

class Rs232Config
{

public:
    explicit Rs232Config();

    virtual ~Rs232Config() = default;

#if RASPBERRYPI

    /* Default serial raw mode with 8n1 frame no parity and baud rate 9600 */
    void setPort(const string &serialPort = "/dev/ttyAMA0"); /* Default serial port for Raspbian */

#else

    void setPort(const string &serialPort = "/dev/ttyUSB0");

#endif

    void setSpeed(const string & speed = "9600"); /* Default baud rate for Raspberry Pi B */

    void setDataBits(const string & dataBits = "8");

    void setStopBits(const string & stopBits = "1");

    void setParity(const string &parity = "none");

    void setFlowControl(const string &flowControl = "none");

    const string &getPort() const;

    const string &getParity() const;

    const string &getFlowControl() const;

    const string &getSpeed() const;

    const string &getDataBits() const;

    const string &getStopBits() const;

private:
    string serialPort,
            parity,
            flowControl,
            speed,
            dataBits,
            stopBits;


};


#endif //METEOSTATIONDRIVERS_SERIALPORT_H
