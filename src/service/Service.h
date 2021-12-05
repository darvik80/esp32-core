//
// Created by Ivan Kishchenko on 07.11.2021.
//

#pragma once

#include "Registry.h"
#include "ServiceId.h"

class Service : public IService {
    IRegistry* _registry;
public:
    Service(ServiceId serviceId, IRegistry *registry)
            : IService(serviceId), _registry(registry) {}

    IRegistry* getRegistry() override {
        return _registry;
    }

    void setup() override { }

    void loop() override { }

    IMessageBus *getMessageBus() override {
        return _registry->getMessageBus();
    }
};


