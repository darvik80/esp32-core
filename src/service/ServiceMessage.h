//
// Created by Ivan Kishchenko on 09.12.2021.
//

#pragma once

#include "message/Message.h"

enum MessageId {
    WIFI_CONNECTED,
    WIFI_DISCONNECTED,
    MQTT_CONNECTED,
    MQTT_DISCONNECTED,
    MQTT_MESSAGE,
    JOYSTICK_EVENT,

    USER_EVENT
};

class WifiConnected : public TMessage<WIFI_CONNECTED> {
public:
    WifiConnected(std::string_view ip, std::string_view mask, std::string_view gateway, std::string_view macAddress)
            : ip(ip), mask(mask), gateway(gateway), macAddress(macAddress) {}

    std::string ip;
    std::string mask;
    std::string gateway;
    std::string macAddress;
};

class WifiDisconnected : public TMessage<WIFI_DISCONNECTED> {
};

struct JoystickEvent : public TMessage<JOYSTICK_EVENT> {
    struct Axis {
        int x{};
        int y{};
        bool btn{};
    };

    Axis leftAxis{};
    Axis rightAxis{};
};

struct MqttConnected : public TMessage<MQTT_CONNECTED> {
    explicit MqttConnected(std::string_view address)
            : address(address) {}

    std::string address;
};

struct MqttDisconnected : public TMessage<MQTT_DISCONNECTED> {

};

struct MqttMessage : public TMessage<MQTT_MESSAGE> {

};