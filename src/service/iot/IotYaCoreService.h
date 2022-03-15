//
// Created by Ivan Kishchenko on 06.03.2022.
//

#pragma once

#ifdef MQTT_SERVICE

#include "service/Service.h"
#include "UserMessage.h"
#include "MyProps.h"
#include "IotService.h"

LOG_COMPONENT_SETUP(iot)

class IotYaCoreService : public IotService, public TMessageSubscriber<IotYaCoreService, MqttConnected> {
    const char *TELEMETRY = "$me/device/events";
    const char *STATE = "$me/device/events";
    const char *COMMANDS = "$me/device/commands";
public:
    explicit IotYaCoreService(IRegistry *registry) : IotService(registry) {}

    [[nodiscard]] ServiceId getServiceId() const override {
        return IOT;
    }

    void setup() override {
        getMessageBus()->subscribe(this);

        Service::setup();
    }

    void telemetry(std::string_view data) override {
        sendMessage(
                Service::getMessageBus(),
                MqttMessage{
                        .topic = TELEMETRY,
                        .data = data.data(),
                        .qos = 1
                }
        );
    }

    void command(std::string_view cmd, std::string_view data) override {
        sendMessage(
                Service::getMessageBus(),
                MqttMessage{
                        .topic = COMMANDS,
                        .data = fmt::format(R"({ "cmd": "{}", "args": {} })", cmd, data).c_str(),
                        .qos = 1
                }
        );
    }

    void onMessage(const MqttConnected& msg) {
        sendMessage(Service::getMessageBus(),MqttSubscribe{COMMANDS, 1});
    }
};

#endif