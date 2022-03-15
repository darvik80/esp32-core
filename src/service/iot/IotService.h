//
// Created by Ivan Kishchenko on 15.03.2022.
//


#pragma once

#include <string_view>

class IotService : public Service {
public:
    explicit IotService(IRegistry *registry) : Service(registry) {}

    [[nodiscard]] ServiceId getServiceId() const override {
        return IOT;
    }
    virtual void telemetry(std::string_view data) = 0;
    virtual void command(std::string_view cmd, std::string_view data) = 0;
};