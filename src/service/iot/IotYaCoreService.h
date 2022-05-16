//
// Created by Ivan Kishchenko on 06.03.2022.
//

#pragma once

#include "MyProps.h"
#include "IotService.h"

class IotYaCoreService : public IotService {
    bool _isConn{false};
    const char *TELEMETRY = "$me/device/state";
    const char *COMMANDS = "$me/device/commands";
public:
    using IotService::IotService;

    void onMessage(const MqttConnected &event) override {
        sendMessage(getRegistry().getMessageBus(), MqttSubscribe{COMMANDS, 1});
        _isConn = true;
    }

    void onMessage(const IoTTelemetry &event) override {
        if (!_isConn) {
            return;
        }

        sendMessage(
                getRegistry().getMessageBus(),
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
                getRegistry().getMessageBus(),
                MqttMessage{
                        .topic = COMMANDS,
                        .data = fmt::format(R"({ "cmd": "{}", "args": {} })", event.cmd, event.data).c_str(),
                        .qos = 1
                }
        );
    }
};
