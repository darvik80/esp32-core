//
// Created by Ivan Kishchenko on 07.11.2021.
//

#pragma once

#include "Registry.h"
#include "ServiceId.h"

class Service : public IService {
    Registry* _registry;
public:
    Service(ServiceId serviceId, Registry *registry)
            : IService(serviceId), _registry(registry) {}

    Registry* getRegistry() override {
        return _registry;
    }

    void setup() override { }

    void loop() override { }

    MessageBus *getMessageBus() override {
        return _registry->getMessageBus();
    }
};


