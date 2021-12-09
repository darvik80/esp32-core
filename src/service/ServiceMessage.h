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
};

class WifiConnected : public Message<WIFI_CONNECTED> {
public:
    WifiConnected(std::string_view ip, std::string_view mask, std::string_view gateway, std::string_view macAddress)
            : ip(ip), mask(mask), gateway(gateway), macAddress(macAddress) {}

    std::string ip;
    std::string mask;
    std::string gateway;
    std::string macAddress;
};

class WifiDisconnected : public Message<WIFI_DISCONNECTED> {
};

struct JoystickEvent : public Message<JOYSTICK_EVENT> {
    struct Axis {
        int x{};
        int y{};
        bool btn{};
    };

    Axis leftAxis{};
    Axis rightAxis{};
};

struct MqttConnected : public Message<MQTT_CONNECTED> {
    explicit MqttConnected(std::string_view address)
            : address(address) {}

    std::string address;
};

struct MqttDisconnected : public Message<MQTT_DISCONNECTED> {

};

struct MqttMessage : public Message<MQTT_MESSAGE> {

};