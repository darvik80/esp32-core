//
// Created by Kishchenko, Ivan on 11/11/21.
//

#pragma once

#include <service/Service.h>
#include "message/Message.h"
#include <message/MessageBus.h>

#include "service/Config.h"
#include <service/wifi/WifiService.h>
#include <service/display/DisplayService.h>
#include <service/joystick/JoystickService.h>
#include <service/mqtt/MqttService.h>
#include <service/iot/IotYaCoreService.h>

#include <ArduinoJson.h>

class Application : public IService, public TMessageSubscriber<Application, WifiConnected, WifiDisconnected, JoystickEvent> {
    Registry _registry;
public:
    explicit Application(logging::level lvl, IMessageBus *bus)
            : _registry(bus, new InCodePropertiesSource()) {
        Serial.begin(115200);
        logging::setLogLevel(lvl);
        logging::addLogger(new logging::SerialLogger());
    }

    [[nodiscard]] ServiceId getServiceId() const override {
        return LibServiceId::APP;
    }

    void setup() override {
        logging::info("Starting...");

        getMessageBus()->subscribe(this);

#ifdef OLED_SERVICE
        getRegistry()->create<DisplayService>();
#endif

#ifdef DEBUG_SERVICES
        getRegistry()->create<JoystickService<ADC1_CHANNEL_1, ADC1_CHANNEL_2, 4 >>();
#endif

#ifdef WIFI_SERVICE
        getRegistry()->create<WifiService>();
#endif

#ifdef MQTT_SERVICE
        getRegistry()->create<IotYaCoreService>();
        getRegistry()->create<MqttService>();
#endif

        for (auto service: getRegistry()->getServices()) {
            if (service) {
                logging::info("Setup {}", service->getServiceId());
                service->setup();
            }
        }

        logging::info("Started!");
#ifdef OLED_SERVICE
        getRegistry()->getService<DisplayService>(LibServiceId::OLED)->setText(4, "Device is ready");
#endif
    }

    void loop() override {
        getMessageBus()->loop();
        for (auto service: getRegistry()->getServices()) {
            if (service) {
                service->loop();
            }
        }
    }

    IRegistry *getRegistry() override {
        return &_registry;
    }

    IMessageBus *getMessageBus() override {
        return _registry.getMessageBus();
    }

    virtual void onMessage(const WifiConnected &msg) {
        logging::info("WiFi connected: {}/{}, {}, {}", msg.ip, msg.mask, msg.gateway, msg.macAddress);

#ifdef OLED_SERVICE
        if (auto oled = getRegistry()->getService<DisplayService>(LibServiceId::OLED); oled) {
            oled->setText(2, "WiFi connected");
            oled->setText(3, "IP: " + msg.ip);
        }
#endif
    }

    virtual void onMessage(const WifiDisconnected &msg) {
#ifdef OLED_SERVICE
        if (auto oled = getRegistry()->getService<DisplayService>(LibServiceId::OLED); oled) {
            oled->setText(2, "WiFi disconnected");
            oled->setText(3, "");
        }
#endif
    }

    virtual void onMessage(const JoystickEvent &msg) {
#ifdef OLED_SERVICE
        if (auto oled = getRegistry()->getService<DisplayService>(LibServiceId::OLED); oled) {
            oled->setText(0, "Joystick:");
            oled->setText(
                    1,
                    fmt::format(
                            "{:04d}:{:04d}-{:04d}:{:04d}",
                            msg.leftAxis.x,
                            msg.leftAxis.y,
                            msg.rightAxis.x,
                            msg.rightAxis.y
                    )
            );
        }
#endif

#if defined(DEBUG_SERVICES) && defined(MQTT_SERVICE)
        if (auto mqtt = getRegistry()->getService<MqttService>(LibServiceId::MQTT); mqtt) {
            DynamicJsonDocument doc(1024);
            doc["leftAxisX"] = msg.leftAxis.x;
            doc["leftAxisY"] = msg.leftAxis.y;
            doc["leftAxisBtn"] = msg.leftAxis.btn;
            doc["rightAxisX"] = msg.rightAxis.x;
            doc["rightAxisY"] = msg.rightAxis.y;
            doc["rightAxisBtn"] = msg.rightAxis.btn;
            String event;
            serializeJson(doc, event);

            //mqtt::log::info("event: {}", event.c_str());
            mqtt->publish("v1/devices/me/telemetry", event.c_str());
        }
#endif
    }
};


