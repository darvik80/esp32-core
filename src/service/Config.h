//
// Created by Ivan Kishchenko on 02.12.2021.
//

#pragma once

#if __has_include("MyConfig.h")
#include "MyConfig.h"
#endif

#ifdef MQTT_SERVICE
#ifndef WIFI_SERVICE
#define WIFI_SERVICE
#endif
#endif
