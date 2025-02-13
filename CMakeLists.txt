cmake_minimum_required(VERSION 3.5)
project(MeteoStationDriver)

set(CMAKE_CXX_STANDARD 11)

set(LIBYAML_CPP /usr/local/lib/libyaml-cpp.a)

include_directories(include /usr/local/include)


add_executable(meteo-station
        src/ConfigManager.cpp
        src/DataManager.cpp
        src/DaemonsManager.cpp
        src/Connection.cpp
        src/HttpRequestCreator.cpp
        src/HttpTask.cpp
        src/MyLog.cpp
        src/MyDaemon.cpp
        src/SerialTask.cpp
        src/Task.cpp
        main.cpp)


target_link_libraries(meteo-station PRIVATE ${LIBYAML_CPP})
