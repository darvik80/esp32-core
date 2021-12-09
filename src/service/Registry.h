//
// Created by Ivan Kishchenko on 07.11.2021.
//

#pragma once

#include "PropertiesSource.h"
#include "message/MessageBus.h"
#include "LibServiceId.h"
#include <vector>
#include <array>
#include <string>
#include <algorithm>

class IRegistry;

class IMessageBus;

class IService {
public:
    [[nodiscard]] virtual ServiceId getServiceId() const = 0;

    virtual void setup() = 0;

    virtual void loop() = 0;

    virtual IRegistry *getRegistry() = 0;

    virtual IMessageBus *getMessageBus() = 0;

    virtual ~IService() = default;
};

typedef std::vector<IService *> ServiceArray;

class IRegistry {
public:
    template<typename C, typename ... T>
    void create(T &&... all) {
        add(new C(this, std::forward<T>(all)...));
    }

    virtual void add(IService *service) = 0;

    virtual ServiceArray &getServices() = 0;

    template<typename C>
    C *getService(ServiceId id) {
        auto services = getServices();
        if (id < services.size()) {
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
            : _bus(bus), _props(props) {
        _services.resize(USER_SERVICES);
    }

    void add(IService *service) override {
        if (service->getServiceId() >= _services.size()) {
            _services.resize(service->getServiceId() + 1);
        }

        _services[service->getServiceId()] = service;
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


