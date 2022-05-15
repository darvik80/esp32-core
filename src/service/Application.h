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

class Application : public TService<>, public TMessageSubscriber<Application, WifiConnected> {
public:
    explicit Application(logging::level lvl) {
        Serial.begin(115200);
        logging::setLogLevel(lvl);
        logging::addLogger(new logging::SerialLogger());
    }

    void setup(Registry &registry) override {
        logging::info("Starting...");

        registry.createSystem<WifiService>();
        registry.createSystem<MqttService>();
        registry.getMessageBus().subscribe(this);

        for (auto service: registry.getSystemServices()) {
            if (service) {
                logging::info("Setup {}", service->getServiceId());
                service->setup(registry);
            }
        }
        for (auto service: registry.getUserServices()) {
            if (service) {
                logging::info("Setup {}", service->getServiceId());
                service->setup(registry);
            }
        }

        logging::info("Started!");
    }

    void loop(Registry &registry) override {
        registry.getMessageBus().loop();
        for (auto service: registry.getSystemServices()) {
            if (service) {
                service->loop(registry);
            }
        }
        for (auto service: registry.getUserServices()) {
            if (service) {
                service->loop(registry);
            }
        }
    }

    virtual void onMessage(const WifiConnected &msg) {
        logging::info("WiFi connected: {}/{}, {}, {}", msg.ip, msg.mask, msg.gateway, msg.macAddress);
    }
};