//
// Created by Kishchenko, Ivan on 11/11/21.
//

#pragma once

#include "service/Config.h"

#ifdef WIFI_SERVICE

#include "logging/Logging.h"

LOG_COMPONENT_SETUP(wifi);

#include "service/Service.h"
#include <Ticker.h>
#include <WiFi.h>

#define PROP_WIFI_SSID "wifi.ssid"
#define PROP_WIFI_PASS "wifi.pass"

class WifiService : public Service {
    Ticker _reconnectTimer;
private:
    void raiseConnect() {
        auto props = getRegistry()->getProperties();
        WiFi.begin(
                props->getStr(PROP_WIFI_SSID, "").c_str(),
                props->getStr(PROP_WIFI_PASS, "").c_str()
        );

        wifi::log::info("init connection: {}", props->getStr(PROP_WIFI_SSID, ""));

    }
public:
    explicit WifiService(IRegistry *registry)
            : Service(registry) {}

    [[nodiscard]] ServiceId getServiceId() const override {
        return LibServiceId::WIFI;
    }

    static void onTimer(WifiService *service) {
        service->raiseConnect();
    }

    void setup() override {
        Service::setup();

        WiFi.onEvent([this](arduino_event_id_t event, arduino_event_info_t info) {
            onWifiEvent(event, info);
        }, ARDUINO_EVENT_WIFI_STA_GOT_IP);

        WiFi.onEvent([this](arduino_event_id_t event, arduino_event_info_t info) {
            onWifiEvent(event, info);
        }, ARDUINO_EVENT_WIFI_STA_DISCONNECTED);

        raiseConnect();
    }

    void onWifiEvent(arduino_event_id_t event, arduino_event_info_t info) {
        switch (event) {
            case ARDUINO_EVENT_WIFI_STA_GOT_IP:
                wifi::log::info("WiFi connected");
                wifi::log::info("Got IP address: {}", WiFi.localIP().toString().c_str());
                sendMessage(
                        Service::getMessageBus(),
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
                _reconnectTimer.once_ms(2000, onTimer, this);
                sendMessage(Service::getMessageBus(), WifiDisconnected{});
                break;
            default:
                break;
        }
    }
};

#endif