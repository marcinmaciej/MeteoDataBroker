cmake_minimum_required(VERSION 3.10)
project(MeteoStationDriver)

set(CMAKE_CXX_STANDARD 11)


include_directories(include)



add_executable(meteo-station
        include/BufferManager.h
        include/DaemonsManager.h
        include/HttpConnection.h
        include/HttpTask.h
        include/MyLog.h
        include/MyDaemon.h
        include/SerialTask.h
        include/Task.h
        src/BufferManager.cpp
        src/DaemonsManager.cpp
        src/HttpConnection.cpp
        src/HttpTask.cpp
        src/MyLog.cpp
        src/MyDaemon.cpp
        src/SerialTask.cpp
        src/Task.cpp
        main.cpp)