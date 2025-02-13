//
// Created by marcin on 26.12.2021.
//

#ifndef METEOSTATIONDRIVER_HTTPREQUESTCREATOR_H
#define METEOSTATIONDRIVER_HTTPREQUESTCREATOR_H

#include "ConfigManager.h"
#include <string>
#include "MyLog.h"

class HttpRequestCreator {

public:

    explicit HttpRequestCreator(const ConfigManager &configManager);

    /* Zwraca zapytanie http */

    std::string createRequest(const std::string &data);

private:

    MyLog *meteoLog;

    const std::string NEWLINE = "\r\n";
    const std::string SPACE = " ";
    const std::string EQUAL_SIGN = "=";
    const std::string QUESTION_MARK = "?";

    std::string httpMethod,
    httpPath,
    httpVersion = "HTTP/1.1",
    httpUserAgent = "User-Agent: ",
    httpContentLength = "Content-Length: ",
    httpDataKeyName,
    request;

    std::map<std::string,std::string> headers;

};


#endif //METEOSTATIONDRIVER_HTTPREQUESTCREATOR_H
