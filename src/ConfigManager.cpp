/** @author  Created by Marcin Guziołek on 23.11.21 r.
 *  @brief Klasa ConfigManager dostarcza ustawienia aplikacji.
 *
 */

#include "ConfigManager.h"

/** @brief Konstruktor klasy ConfigManager
 *
 *  @param const char * appConfigPath - Ścieżka do pliku konfiguracyjnego YAML 'paths.yaml'.
 */
ConfigManager::ConfigManager(const char *appConfigPath) : meteoLog(new MyLog("CLASS::ConfigManager")) {

    this->appConfig["appConfigPath"] = appConfigPath;

}


/** @brief Funkcja wczytująca plik konfiguracyjny YAML
 *
 * @param map<std::string, std::string> &configMap - Referencja do mapy, w której zapisane zostają ustawienia.
 *
 * @param std::string &path  - Ścieżka do pliku konfiguracyjnego YAML.
 *
 * @return bool
 */
bool ConfigManager::loadAppConfig(std::map<std::string, std::string> &configMap, std::string &path) {

    bool isLoaded = false;

    try {

        YAML::Node newSerialConfig = YAML::LoadFile(path);

        if (!newSerialConfig.IsNull() && newSerialConfig.IsMap()) {

            for (YAML::const_iterator it = newSerialConfig.begin(); it != newSerialConfig.end(); ++it) {

                configMap[it->first.as<std::string>()] = it->second.as<std::string>();

            }

            isLoaded = true;
        }
    } catch (std::exception &e) {

        std::cerr << "CLASS::ConfigManager::loadAppConfig(" << path << ")::" << e.what() << std::endl;

        std::exit(EXIT_FAILURE);

    }


    return isLoaded;
}

/** @brief Funkcja ładująca plik konfiguracyjny 'http.yaml' z ustawieniami zapytania http
 *
 * @details Jeśli jeden z elementów jest mapą, iteruje po niej, są to nagłówki zapytania http.
 * Zapisuje je w osobnej mapie z nagłówkami 'httpHeaders', pozostałe elementy
 * zapisuje w mapie 'httpConfig'
 *
 * @param const std::string &path - Ścieżka do pliku Konfiguracyjnego YAML
*/
bool ConfigManager::loadHttpConfig(const std::string &path) {

    bool isLoaded = false;

    try {

        /* Tworzy YAML node z wczytanego pliku konfiguracyjnego dla zapytania http. */
        YAML::Node newHttpConfig = YAML::LoadFile(path);

        /* Jeśli node jest mapą i nie jest pusty, iteruje po mapie. */
        if (!newHttpConfig.IsNull() && newHttpConfig.IsMap()) {

            for (YAML::const_iterator it = newHttpConfig.begin(); it != newHttpConfig.end(); ++it) {

                if (it->second.IsMap()) {

                    for (YAML::const_iterator ite = it->second.begin(); ite != it->second.end(); ++ite) {

                        /* Zapisuje do mapy 'httpHeaders' nagłówki zapytania http.  */
                        this->httpHeaders[ite->first.as<std::string>()] = ite->second.as<std::string>();

                    }

                } else {

                    /* Zapisuje do mapy 'httpConfig' inne elementy ustawień niż nagłówki zapytania http. */
                    this->httpConfig[it->first.as<std::string>()] = it->second.as<std::string>();

                }
            }

            isLoaded = true;
        }

    } catch (std::exception &e) {

        std::cerr << "CLASS::ConfigManager::loadHttpConfig(path)::" << e.what() << std::endl;

        std::exit(EXIT_FAILURE);

    }

    return isLoaded;
}


const std::string &ConfigManager::getConfig(const std::string &param, int whatConfig) const {

    auto *value = new std::string;

    try {

        if (!param.empty()) {

            if (whatConfig >= 0 && (whatConfig <= configMaps.size() - 1)) {

                switch (whatConfig) {
                    case APP:
                        *value = this->configMaps[APP].at(param);
                        break;
                    case SOCKET:
                        *value = this->configMaps[SOCKET].at(param);
                        break;
                    case SERIAL:
                        *value = (this->configMaps[SERIAL]).at(param);
                        break;
                    case HTTP:
                        *value = this->configMaps[HTTP].at(param);
                        break;
                    default:;

                }
            }
        }

    } catch (std::exception &e) {

        std::string msg = "::getConfig(param,whatConfig) => param="
                          + param
                          + "  whatConfig="
                          + std::to_string(whatConfig);

        this->meteoLog->err(msg.c_str());

        exit(EXIT_FAILURE);
    }

    return *value;
}


const std::map<std::string, std::string> &ConfigManager::getHttpHeaders() const {

    return this->httpHeaders;

}


void ConfigManager::loadAllConfigFiles() {

    if (this->loadAppConfig(this->appConfig, this->appConfig.at("appConfigPath"))) {

        /** @brief  Loading configuration file for serialFileDescriptor port.
         *  On success loading and do initiating params. Exit on failure.
        */
        if (!this->loadAppConfig(this->serialConfig, this->appConfig.at("rs232ConfigPath"))) {

            std::cerr << "Brak pliku 'rs232.yaml' lub zły format pliku yaml!" << std::endl;

            std::exit(EXIT_FAILURE);

        } else {

            std::cout << "Plik 'rs232.yaml' wczytany!" << std::endl;
        }

        /** @brief Loading configuration file for http httpRequest.
         *  On success loading and do initiating params. Exit on failure.
        */
        if (!this->loadHttpConfig(this->appConfig.at("httpConfigPath"))) {

            std::cerr << "Brak pliku 'http.yaml' lub zły format pliku yaml!" << std::endl;

            exit(EXIT_FAILURE);

        } else {

            std::cout << "Plik 'http.yaml' wczytany!" << std::endl;
        }

        /** @brief Loading configuration file for socket connection.
         *  On success loading and do initiating params. Exit on failure.
        */
        if (!this->loadAppConfig(this->socketConfig, this->appConfig.at("socketConfigPath"))) {

            std::cerr << "Brak pliku 'socket.yaml' lub zły format pliku yaml!" << std::endl;

            exit(EXIT_FAILURE);

        } else {

            std::cout << "Plik 'socket.yaml' wczytany!" << std::endl;
        }

        this->configMaps.push_back(this->appConfig);
        this->configMaps.push_back(this->socketConfig);
        this->configMaps.push_back(this->serialConfig);
        this->configMaps.push_back(this->httpConfig);
    }
}


ConfigManager::ConfigManager(ConfigManager &configManager) : appConfig(configManager.appConfig),
                                                             httpConfig(configManager.httpConfig),
                                                             serialConfig(configManager.serialConfig),
                                                             socketConfig(configManager.socketConfig) {

    this->meteoLog = new MyLog("MS-ConfigManager");
    this->meteoLog = configManager.meteoLog;


}

ConfigManager::~ConfigManager() {

    delete this->meteoLog;
}



