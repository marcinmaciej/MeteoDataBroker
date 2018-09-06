//
// Created by Marcin Guziołek on 04.04.18.
//

#include "Rs232Config.h"

Rs232Config::Rs232Config()
{
    this->setPort();
    this->setSpeed();
    this->setFlowControl();
    this->setParity();
    this->setDataBits();
    this->setStopBits();
}

void Rs232Config::setPort(const string &serialPort)
{
    this->serialPort = serialPort;
}

void Rs232Config::setParity(const string &parity)
{
    this->parity = parity;
}

void Rs232Config::setFlowControl(const string &flowControl)
{
    this->flowControl = flowControl;
}

void Rs232Config::setSpeed(const string & speed)
{
    this->speed = speed;
}

void Rs232Config::setDataBits(const string & dataBits)
{
    this->dataBits = dataBits;
}

void Rs232Config::setStopBits(const string & stopBits)
{
    this->stopBits = stopBits;
}

const string &Rs232Config::getPort() const
{
    return this->serialPort;
}

const string &Rs232Config::getParity() const
{
    return this->parity;
}

const string &Rs232Config::getFlowControl() const
{
    return this->flowControl;
}

const string & Rs232Config::getSpeed() const
{
    return this->speed;
}

const string & Rs232Config::getDataBits() const
{
    return this->dataBits;
}

const string &Rs232Config::getStopBits() const
{
    return this->stopBits;
}
