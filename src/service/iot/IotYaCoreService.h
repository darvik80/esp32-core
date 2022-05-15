//
// Created by Ivan Kishchenko on 06.03.2022.
//

#pragma once

#include "service/Service.h"
#include "MyProps.h"
#include "IotService.h"

class IotYaCoreService : public IotService {
    bool _isConn{false};
    const char *TELEMETRY = "$me/device/events";
    const char *COMMANDS = "$me/device/commands";
public:
    void onMessage(const MqttConnected &event) override {
        sendMessage(_bus, MqttSubscribe{COMMANDS, 1});
        _isConn = true;
    }

    void onMessage(const IoTTelemetry &event) override {
        if (!_isConn) {
            return;
        }
        sendMessage(
                _bus,
                MqttMessage{
                        .topic = TELEMETRY,
                        .data = event.data,
                        .qos = 1
                }
        );
    }

    void onMessage(const IoTCommand &event) override {
        if (!_isConn) {
            return;
        }

        sendMessage(
                _bus,
                MqttMessage{
                        .topic = COMMANDS,
                        .data = fmt::format(R"({ "cmd": "{}", "args": {} })", event.cmd, event.data).c_str(),
                        .qos = 1
                }
        );
    }
};
