//
// Created by Ivan Kishchenko on 14.03.2022.
//


#pragma once

#include "service/PropertiesSource.h"

struct MqttProperties {
    std::string clientId;
    std::string username;
    std::string password;
    std::string uri;
    const char* certDev;
    const char* rootCa;
};
