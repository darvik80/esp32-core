//
// Created by Kishchenko, Ivan on 11/11/21.
//

#pragma once

#include "logging/Logging.h"
#include "service/Config.h"

#include "message/MessageBus.h"

#include "service/Registry.h"
#include "service/wifi/WifiService.h"
#include "service/mqtt/MqttService.h"
#include "service/iot/IotYaCoreService.h"

template <class Reg>
class TApplication {
    Reg _reg;
public:
    Registry& getRegistry() {
        return _reg;
    }

    explicit TApplication(logging::level lvl) {
        Serial.begin(115200);
        logging::setLogLevel(lvl);
        logging::addLogger(new logging::SerialLogger());
    }

    virtual void setup() {
        auto startTime = millis();
        logging::info("starting...");

        getRegistry().template createSystem<WifiService>();
        getRegistry().template createSystem<MqttService>();

        for (auto service: getRegistry().getSystemServices()) {
            if (service) {
                logging::info("Setup {}", service->getServiceId());
                service->setup();
            }
        }
        for (auto service: getRegistry().getUserServices()) {
            if (service) {
                service->setup();
            }
        }

        logging::info("started, {}ms", millis()-startTime);
    }

    virtual void loop() {
        getRegistry().getMessageBus().loop();
        for (auto service: getRegistry().getSystemServices()) {
            if (service) {
                service->loop();
            }
        }
        for (auto service: getRegistry().getUserServices()) {
            if (service) {
                service->loop();
            }
        }
    }
};