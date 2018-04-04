//
// Created by Marcin Guziołek on 04.04.18.
//

#include "SerialPort.h"

SerialPort::SerialPort(string port) {

    this->setPort(port);
}

void SerialPort::setPort(string port) {

    this->port = port;
}

void SerialPort::setDataBits(unsigned char dataBits) {

    this->dataBits = dataBits;
}

void SerialPort::setFlowControl(string flowControl) {

    this->flowControl = flowControl;
}

void SerialPort::setSpeed(unsigned short speed) {

    this->speed = speed;
}

void SerialPort::setStopBits(unsigned char stopBits) {

    this->stopBits = stopBits;
}

void SerialPort::setParity(string parity) {

    this->parity = parity;
}


string SerialPort::getPort() {

    return this->port;
}

string SerialPort::getParity() {

    return this->parity;
}

string SerialPort::getFlowControl() {

    return this->flowControl;
}

unsigned short SerialPort::getSpeed() {

    return this->speed;
}

unsigned char SerialPort::getDataBits() {

    return this->dataBits;
}

unsigned char SerialPort::getStopBits() {

    return this->stopBits;
}
