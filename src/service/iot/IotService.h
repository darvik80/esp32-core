//
// Created by Ivan Kishchenko on 15.03.2022.
//


#pragma once

#include <string_view>

#include "service/Service.h"

class IotService : public TService<Service_IoT>, public TMessageSubscriber<IotService, MqttConnected, IoTTelemetry, IoTCommand> {
public:
    explicit IotService(Registry *registry) : TService(registry) {}

    void setup() override {
        getRegistry().getMessageBus().subscribe(this);
    }

    virtual void onMessage(const MqttConnected& event) = 0;
    virtual void onMessage(const IoTTelemetry& event) = 0;
    virtual void onMessage(const IoTCommand& event) = 0;
};