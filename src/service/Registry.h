//
// Created by Ivan Kishchenko on 07.11.2021.
//

#pragma once

#include "Properties.h"
#include "message/MessageBus.h"
#include "ServiceId.h"
#include <vector>
#include <array>
#include <string>
#include <algorithm>

class Registry;

class MessageBus;

class IService {
    ServiceId _serviceId;
public:
    explicit IService(ServiceId serviceId) : _serviceId(serviceId) {}

    [[nodiscard]] ServiceId getServiceId() const {
        return _serviceId;
    }

    virtual void setup() = 0;

    virtual void loop() = 0;

    virtual Registry *getRegistry() = 0;

    virtual MessageBus *getMessageBus() = 0;

    virtual ~IService() = default;
};

typedef std::array<IService *, (size_t) ServiceId::MAX_ID> ServiceArray;

class Registry {
    MessageBus _bus;
    ServiceArray _services{};

    Properties *_props;
public:
    explicit Registry(Properties *props) : _props(props) {}

    virtual void add(IService *service) {
        if ((size_t) service->getServiceId() < _services.size()) {
            _services[(size_t) service->getServiceId()] = service;
        }
    }

    template<typename C>
    C *getService(ServiceId id) {
        if ((size_t) id < _services.size()) {
            return static_cast<C *>(_services[(size_t) id]);
        }

        return nullptr;
    }

    ServiceArray &getServices() {
        return _services;
    }

    virtual MessageBus *getMessageBus() {
        return &_bus;
    }

    virtual Properties *getProperties() {
        return _props;
    }
};


