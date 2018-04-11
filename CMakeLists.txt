cmake_minimum_required(VERSION 3.10)
project(MeteoStationDrivers)

set(CMAKE_CXX_STANDARD 11)

include_directories(include)


add_executable(meteo-station
        include/BufferManager.h
        include/DaemonsManager.h
        include/HttpConnection.h
        include/HttpTask.h
        include/MeteoLog.h
        include/MyDaemon.h
        include/Rs232Task.h
        include/Task.h
        src/BufferManager.cpp
        src/DaemonsManager.cpp
        src/HttpConnection.cpp
        src/HttpTask.cpp
        src/MeteoLog.cpp
        src/MyDaemon.cpp
        src/Rs232Task.cpp
        src/Task.cpp
        main.cpp src/SerialPort.cpp include/SerialPort.h src/ConnectionConfig.cpp include/HttpConnection.h src/ConfigManager.cpp include/ConfigManager.h src/ProgramConfig.cpp include/ProgramConfig.h)
