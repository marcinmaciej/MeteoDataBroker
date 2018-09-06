/**
 * @author  Created by Marcin Guziołek on 06.04.18.
 */

#include "SerialConfig.h"

SerialConfig::SerialConfig()
{
    this->setPort();
    this->setSpeed();
    this->setFlowControl();
    this->setParity();
    this->setDataBits();
    this->setStopBits();
    this->setSleepTime();
    this->setDataLength();
}

void SerialConfig::setPort(const string &serialPort)
{
    this->serialPort = serialPort;
}

void SerialConfig::setParity(const string &parity)
{
    this->parity = parity;
}

void SerialConfig::setFlowControl(const string &flowControl)
{
    this->flowControl = flowControl;
}

void SerialConfig::setSpeed(const string & speed)
{
    this->speed = speed;
}

void SerialConfig::setDataBits(const string & dataBits)
{
    this->dataBits = dataBits;
}

void SerialConfig::setStopBits(const string & stopBits)
{
    this->stopBits = stopBits;
}

const string &SerialConfig::getPort() const
{
    return this->serialPort;
}

const string &SerialConfig::getParity() const
{
    return this->parity;
}

const string &SerialConfig::getFlowControl() const
{
    return this->flowControl;
}

const string & SerialConfig::getSpeed() const
{
    return this->speed;
}

const string & SerialConfig::getDataBits() const
{
    return this->dataBits;
}

const string &SerialConfig::getStopBits() const
{
    return this->stopBits;
}

const size_t SerialConfig::getDataLength() const
{
    return this->dataLength;
}

void SerialConfig::setDataLength(const string &dataLength)
{
    this->dataLength = static_cast<size_t >(stoi(dataLength));
}

const unsigned int SerialConfig::getSleepTime() const
{
    return this->sleepTime;
}

void SerialConfig::setSleepTime(const string &sleepTime)
{
    this->sleepTime = static_cast<unsigned int>(stoi(sleepTime));
}

