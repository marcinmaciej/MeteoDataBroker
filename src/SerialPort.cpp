//
// Created by Marcin Guziołek on 04.04.18.
//

#include "SerialPort.h"

SerialPort::SerialPort(const string &port)
{
    setPort(port);
    setSpeed();
    setFlowControl();
    setParity();
    setDataBits();
    setStopBits();
}

void SerialPort::setPort(const string &port)
{
    SerialPort::port = port;
}

void SerialPort::setParity(const string &parity)
{
    SerialPort::parity = parity;
}

void SerialPort::setFlowControl(const string &flowControl)
{
    SerialPort::flowControl = flowControl;
}

void SerialPort::setSpeed(unsigned short speed)
{
    SerialPort::speed = speed;
}

void SerialPort::setDataBits(unsigned char dataBits)
{
    SerialPort::dataBits = dataBits;
}

void SerialPort::setStopBits(unsigned char stopBits)
{
    SerialPort::stopBits = stopBits;
}

const string &SerialPort::getPort() const
{
    return port;
}

const string &SerialPort::getParity() const
{
    return parity;
}

const string &SerialPort::getFlowControl() const
{
    return flowControl;
}

unsigned short SerialPort::getSpeed() const
{
    return speed;
}

unsigned char SerialPort::getDataBits() const
{
    return dataBits;
}

unsigned char SerialPort::getStopBits() const
{
    return stopBits;
}
