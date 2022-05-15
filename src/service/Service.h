//
// Created by Ivan Kishchenko on 07.11.2021.
//

#pragma once

#include "ServiceMessage.h"

typedef uint_least8_t ServiceId;

class Registry;

enum SystemServiceId {
    Service_WIFI,
    Service_MQTT,
    Service_IoT,
    Service_Display,
};

class Service {
public:
    [[nodiscard]] virtual ServiceId getServiceId() const = 0;

    virtual void setup(Registry &registry) = 0;

    virtual void loop(Registry &registry) = 0;

    virtual ~Service() = default;
};

typedef std::vector<Service *> ServiceArray;

template<ServiceId Id = 0>
class TService : public Service {
public:
    [[nodiscard]] ServiceId getServiceId() const override {
        return Id;
    }

    void setup(Registry &registry) override { }

    void loop(Registry &registry) override { }
};
