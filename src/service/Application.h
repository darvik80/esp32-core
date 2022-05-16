//
// Created by Kishchenko, Ivan on 11/11/21.
//

#pragma once

#include <service/Registry.h>
#include <service/Service.h>
#include "message/Message.h"
#include <message/MessageBus.h>

#include "service/Config.h"
#include <service/wifi/WifiService.h>
#include <service/display/DisplayService.h>
#include <service/joystick/JoystickService.h>
#include <service/mqtt/MqttService.h>
#include <service/iot/IotYaCoreService.h>

//#include <ArduinoJson.h>

template <typename Reg>
class TApplication : public Service, public TMessageSubscriber<TApplication<Reg>, WifiConnected> {
    Reg _registry;
protected:
    Reg& getRegistry() override {
        return _registry;
    }
public:
    [[nodiscard]] ServiceId getServiceId() const override {
        return 0;
    }

    explicit TApplication(logging::level lvl) {
        Serial.begin(115200);
        logging::setLogLevel(lvl);
        logging::addLogger(new logging::SerialLogger());
    }

    void setup() override {
        auto startTime = millis();
        logging::info("starting...");

        getRegistry().template createSystem<WifiService>();
        getRegistry().template createSystem<MqttService>();

        getRegistry().getMessageBus().subscribe(this);

        for (auto service: _registry.getSystemServices()) {
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

    void loop() override {
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

    virtual void onMessage(const WifiConnected &msg) {
        logging::info("WiFi connected: {}/{}, {}, {}", msg.ip, msg.mask, msg.gateway, msg.macAddress);
    }
};