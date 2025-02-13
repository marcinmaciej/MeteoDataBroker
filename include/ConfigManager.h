/**
 * @author  Created by Marcin Guziołek on 23.11.21 r.
 */

#ifndef METEOSTATIONDRIVERS_CONFIGMANAGER_H
#define METEOSTATIONDRIVERS_CONFIGMANAGER_H

#include <iostream> /* std::cout, std::cerr std::endl */
#include <map> /* std::map */
#include <vector> /* std::vector */
#include <string> /* std::string */
#include <cerrno>   /* errno */

#include <yaml-cpp/yaml.h> /* YAML::const_iterator, YAML::Node, YAML::LoadFile(), YAML::isMap(), YAML::isNull() */

#include "MyLog.h"

class ConfigManager {

public:

    explicit ConfigManager(const char *appConfigPath);

    ConfigManager(ConfigManager &configManager);

    ~ConfigManager();

    /* Zwraca wartość żądanego parametru danych ustawień */
    const std::string &getConfig(const std::string &param, int) const;

    /* Zwraca nagłówki zapytania http jako mapa string-string */
    const std::map<std::string, std::string> &getHttpHeaders() const;

    /* Wczytuje wszystkie pliki z ustawieniami */
    void loadAllConfigFiles();

    /* Definiuje rodzaj ustawień dla metody 'getConfig',
     * każda ze stałych odpowiada kolejno indeksowi vectora 'configMaps' przechowującej
     * odpowiednio mapy z ustawieniami 'appConfig', 'socketConfig', 'serialConfig', 'httpConfig'
     * */
    enum {
        APP, SOCKET, SERIAL, HTTP
    };


private:

    /* Obsługuje zapis logów do dziennika systemowego */
    MyLog *meteoLog;

    /* Vector przechowuje mapy z ustawieniami */
    std::vector<std::map<std::string, std::string>> configMaps;

    /* Mapa dla ustawień połączenia szeregowego rs232 */
    std::map<std::string, std::string> serialConfig;

    /* Mapa dla ustawień połączenia http */
    std::map<std::string, std::string> httpConfig;

    /* Mapa dla ustawień połączenia socket */
    std::map<std::string, std::string> socketConfig;

    /* Mapa dla ustawień aplikacji */
    std::map<std::string, std::string> appConfig;

    /* Mapa dla nagłówków zapytania http */
    std::map<std::string, std::string> httpHeaders;

    /* Wczytuje konfigurację zapytania http */
    bool loadHttpConfig(const std::string &path);

    /* Wczytuje wybrany plik z ustawieniami */
    bool loadAppConfig(std::map<std::string, std::string> &configMap, std::string &path);

};


#endif //METEOSTATIONDRIVERS_CONFIGMANAGER_H
