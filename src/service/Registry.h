//
// Created by Ivan Kishchenko on 07.11.2021.
//

#pragma once

#include <utility>

#include "PropertiesSource.h"
#include "Service.h"
#include "message/MessageBus.h"

class Registry {
public:
    template<typename C, typename... T>
    void createSystem(T &&... all) {
        addSystemService(new C(std::forward<T>(all)...));
    }

    template<typename C, typename... T>
    void create(T &&... all) {
        addUserService(new C(std::forward<T>(all)...));
    }

    virtual void addSystemService(Service *service) = 0;

    virtual ServiceArray &getSystemServices() = 0;

    virtual void addUserService(Service *service) = 0;

    virtual ServiceArray &getUserServices() = 0;

    template<typename C>
    C *getSystemService(ServiceId id) {
        if (!id) {
            return nullptr;
        }
        for (auto service: getSystemServices()) {
            if (service->getServiceId() == id) {
                return service;
            }
        }

        return nullptr;
    }

    template<typename C>
    C *getUserService(ServiceId id) {
        if (!id) {
            return nullptr;
        }
        for (auto service: getUserServices()) {
            if (service->getServiceId() == id) {
                return service;
            }
        }

        return nullptr;
    }

    virtual PropertiesSource &getProperties() = 0;

    virtual MessageBus &getMessageBus() = 0;
};

template<typename MsgBus, typename PropSource = InCodePropertiesSource>
class TRegistry : public Registry {
    ServiceArray _sysServices;
    ServiceArray _userServices;
    MsgBus _bus;

    PropSource _source;
public:
    void addSystemService(Service *service) override {
        _sysServices.push_back(service);
    }

    ServiceArray &getSystemServices() override {
        return _sysServices;
    }

    void addUserService(Service *service) override {
        _userServices.push_back(service);
    }

    ServiceArray &getUserServices() override {
        return _userServices;
    }

    PropertiesSource &getProperties() override {
        return _source;
    }

    MessageBus &getMessageBus() override {
        return _bus;
    }
};


