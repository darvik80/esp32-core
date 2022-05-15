//
// Created by Kishchenko, Ivan on 11/11/21.
//

#pragma once

#include "service/Config.h"

#include "logging/Logging.h"

LOG_COMPONENT_SETUP(wifi);

#include "service/Service.h"
#include "system/Timer.h"
#include <WiFi.h>

#define PROP_WIFI_SSID "wifi.ssid"
#define PROP_WIFI_PASS "wifi.pass"

class WifiService : public TService<Service_WIFI> {
private:
    void raiseConnect(Registry &registry) {
        auto& props = registry.getProperties();
        WiFi.begin(
                props.getStr(PROP_WIFI_SSID, "").c_str(),
                props.getStr(PROP_WIFI_PASS, "").c_str()
        );

        wifi::log::info("init connection: {}", props.getStr(PROP_WIFI_SSID, ""));
    }

public:
    void setup(Registry &registry) override {
        WiFi.setAutoReconnect(false);
        WiFi.onEvent([this, &registry](arduino_event_id_t event, arduino_event_info_t info) {
            onWifiEvent(registry, event, info);
        }, ARDUINO_EVENT_WIFI_STA_GOT_IP);

        WiFi.onEvent([this, &registry](arduino_event_id_t event, arduino_event_info_t info) {
            onWifiEvent(registry, event, info);
        }, ARDUINO_EVENT_WIFI_STA_DISCONNECTED);

        raiseConnect(registry);
    }

    void onWifiEvent(Registry &registry, arduino_event_id_t event, arduino_event_info_t info) {
        switch (event) {
            case ARDUINO_EVENT_WIFI_STA_GOT_IP:
                wifi::log::info("WiFi connected");
                wifi::log::info("Got IP address: {}", WiFi.localIP().toString().c_str());
                sendMessage(
                        registry.getMessageBus(),
                        std::make_shared<WifiConnected>(
                                WiFi.localIP().toString().c_str(),
                                WiFi.subnetMask().toString().c_str(),
                                WiFi.gatewayIP().toString().c_str(),
                                WiFi.macAddress().c_str()
                        )
                );
                break;
            case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
                wifi::log::info("WiFi lost connection");

                schedule(registry.getMessageBus(), 5000, false, [&registry, this]() {
                    raiseConnect(registry);
                });

                sendMessage(registry.getMessageBus(), WifiDisconnected{});
                break;
            default:
                break;
        }
    }
};
