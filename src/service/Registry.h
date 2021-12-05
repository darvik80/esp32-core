//
// Created by Ivan Kishchenko on 07.11.2021.
//

#pragma once

#include "PropertiesSource.h"
#include "message/MessageBus.h"
#include "ServiceId.h"
#include <vector>
#include <array>
#include <string>
#include <algorithm>

class IRegistry;

class IMessageBus;

class IService {
    ServiceId _serviceId;
public:
    explicit IService(ServiceId serviceId)
            : _serviceId(serviceId) {}

    [[nodiscard]] ServiceId getServiceId() const {
        return _serviceId;
    }

    virtual void setup() = 0;

    virtual void loop() = 0;

    virtual IRegistry *getRegistry() = 0;

    virtual IMessageBus *getMessageBus() = 0;

    virtual ~IService() = default;
};

typedef std::array<IService *, (size_t) ServiceId::MAX_ID> ServiceArray;

class IRegistry {
public:
    virtual void add(IService *service) = 0;

    virtual ServiceArray &getServices() = 0;

    template<typename C>
    C *getService(ServiceId id) {
        auto services = getServices();
        if ((size_t) id < services.size()) {
            return static_cast<C *>(services[(size_t) id]);
        }

        return nullptr;
    }

    virtual IMessageBus *getMessageBus() = 0;

    virtual IPropertiesSource *getProperties() = 0;
};

class Registry : public IRegistry {
    ServiceArray _services{};

    IMessageBus *_bus;
    IPropertiesSource *_props;
public:
    explicit Registry(IMessageBus *bus, IPropertiesSource *props)
            : _bus(bus), _props(props) {}

    void add(IService *service) override {
        if ((size_t) service->getServiceId() < _services.size()) {
            _services[(size_t) service->getServiceId()] = service;
        }
    }

    ServiceArray &getServices() override {
        return _services;
    }

    IMessageBus *getMessageBus() override {
        return _bus;
    }

    IPropertiesSource *getProperties() override {
        return _props;
    }
};


