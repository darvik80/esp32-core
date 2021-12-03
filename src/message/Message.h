//
// Created by Ivan Kishchenko on 07.11.2021.
//

#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <string_view>

typedef uint_least8_t MsgId;

class IMessage {
public:
    [[nodiscard]] virtual MsgId getMsgId() const = 0;
};

template<MsgId id>
class Message : public IMessage {
public:
    enum {
        ID = id
    };

    [[nodiscard]] MsgId getMsgId() const override {
        return ID;
    }
};

enum MessageId {
    WIFI_CONNECTED,
    WIFI_DISCONNECTED,
    JOYSTICK_EVENT,
    MQTT_CONNECTED,
    MQTT_DISCONNECTED,
    MQTT_MESSAGE,
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

};

struct MqttDisconnected : public Message<MQTT_DISCONNECTED> {

};

struct MqttMessage : public Message<MQTT_MESSAGE> {

};