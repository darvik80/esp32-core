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
};

class Service {
public:
    [[nodiscard]] virtual ServiceId getServiceId() const = 0;

    virtual Registry& getRegistry() = 0;

    virtual void setup() = 0;

    virtual void loop() = 0;

    virtual ~Service() = default;
};

typedef std::vector<Service *> ServiceArray;

template<ServiceId Id = 0>
class TService : public Service {
    Registry &_registry;
public:
    explicit TService(Registry &registry) : _registry(registry) {}

    [[nodiscard]] ServiceId getServiceId() const override {
        return Id;
    }

    Registry& getRegistry() override {
        return _registry;
    }

    void setup() override { }

    void loop() override { }
};
