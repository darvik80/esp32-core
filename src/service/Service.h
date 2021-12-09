//
// Created by Ivan Kishchenko on 07.11.2021.
//

#pragma once

#include "Registry.h"
#include "LibServiceId.h"
#include "ServiceMessage.h"

class Service : public IService {
    IRegistry* _registry;
public:
    explicit Service(IRegistry *registry)
            : _registry(registry) {}

    IRegistry* getRegistry() override {
        return _registry;
    }

    void setup() override { }

    void loop() override { }

    IMessageBus *getMessageBus() override {
        return _registry->getMessageBus();
    }
};


