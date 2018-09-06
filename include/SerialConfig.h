/**@author Created by Marcin Guziołek on 06.04.18.
 * @class SerialConfig
 * @brief Serial port base configuration class
 *
 * Class sets base configuration for serial port.
 * Configuration data is taken from outside configuration file at path /etc/meteo-station/config/rs232.config.
 * In case there is no configuration file default configuration set is taken as below:\n
 * Default serial raw mode with 8n1 frame, no parity, no flow control and baud rate 9600.\n
 * serialPort: path for Raspbian is /dev/ttyAMA0 and for linux desktop with Prolific PL2303 /dev/ttyUSB0\n
 * speed: 9600 bauds\n
 * dataBits: 8 bits\n
 * stopBits: 1 bit\n
 * parity: none\n
 * flowControl: none\n
 * Default amount of data to read at a time dataLength: 1024 bytes\n
 * Default daemon params sleepTime: 1 second \n
 *
 */

#ifndef METEOSTATIONDRIVERS_SERIALPORT_H
#define METEOSTATIONDRIVERS_SERIALPORT_H

#include <iostream>
#include <termios.h>

#define RASPBERRYPI false

using namespace std;

class SerialConfig
{

public:

    SerialConfig();

    virtual ~SerialConfig() = default;


#if RASPBERRYPI

    /**@brief Setter for field serialPort
     *
     * Default serial port path for Raspbian.
     *
     * @param const string & serialPort path default /dev/ttyAMA0
     * /
        void setPort(const string &serialPort = "/dev/ttyAMA0"); /* */

#else

/**@brief Setter for private field serialPort
 *
 * Path to serial port using Prolific usb to serial adapter PL2303.
 *
 * @param const string & serialPort path default /dev/ttyUSB0
 */
    void setPort(const string &serialPort = "/dev/ttyUSB0");

#endif

/**@brief Setter for private field speed
*
 *  Default baud rate for Raspberry Pi B.
 *
* @param const string & speed default 9600 bauds
*/
    void setSpeed(const string &speed = "9600");

/**@brief Setter for private field dataBits
 *
 * @param const string & dataBits default 8 bits
 */
    void setDataBits(const string &dataBits = "8");

/**@brief Setter for private field stopBits
 *
 * @param const string & stopBits default 1 bit
 */
    void setStopBits(const string &stopBits = "1");

/**@brief Setter for private field parity
 *
 * @param const string & parity type default none
 */
    void setParity(const string &parity = "none");

/**@brief Setter for field flowControl
 *
 * @param const string & flowControl type default none
 */
    void setFlowControl(const string &flowControl = "none");

/**@brief Setter for private field dataLength
 *
 * @param int dataLength default 1024 bytes read at a time
 */
    void setDataLength(const string &dataLength = "1024");

/**@brief Setter for private field sleepTime
 *
 * @param int sleepTime default 1 second for daemon managing serial port reading
 */
    void setSleepTime(const string &sleepTime = "1");

/**@brief Getter for private field serialPort
 *
 * @return const string & serial port path
 */
    const string &getPort() const;

/**@brief Getter for private field parity
 *
 * @return const string & parity type for serial port frame
 */
    const string &getParity() const;

/**@brief Getter for private field flowControl
 *
 * @return const string & flow control type for serial port frame
 */
    const string &getFlowControl() const;

/**@brief Getter for private field speed
 *
 * @return const string & serial port baud rate speed
 */
    const string &getSpeed() const;

/**@brief Getter for field dataBits
 *
 * @return const string & number of data bits for serial port frame
 */
    const string &getDataBits() const;

/**@brief Getter for private field stopBits
 *
 * @return const string & number of stop bits for serial port frame
 */
    const string &getStopBits() const;

/**@brief Getter for private field sleepTime
 *
 * @return int sleep time in seconds for daemon managing serial port reading
 */
    const unsigned int getSleepTime() const;

/**@brief Getter for private field dataLength
 *
 * @return int data length int bytes to read from port
 */
    const size_t getDataLength() const;


private:
    string serialPort,
            parity,
            flowControl,
            speed,
            dataBits,
            stopBits;

    size_t dataLength;

    unsigned int sleepTime;

};


#endif //METEOSTATIONDRIVERS_SERIALPORT_H
