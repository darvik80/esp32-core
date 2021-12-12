//
// Created by Kishchenko, Ivan on 11/12/21.
//

#pragma once

#include "service/Config.h"

#ifdef MQTT_SERVICE

#include "logging/Logging.h"

LOG_COMPONENT_SETUP(mqtt);

#include <WiFi.h>
#include <string_view>

#include "message/Message.h"

#include "service/Service.h"
#include <AsyncMqttClient.hpp>

#define PROP_MQTT_SERVER_HOST "mqtt.host"
#define PROP_MQTT_SERVER_PORT "mqtt.port"
#define PROP_MQTT_USER "mqtt.user"
#define PROP_MQTT_PASS "mqtt.pass"
#define PROP_MQTT_CLIENT_ID "mqtt.client.id"

class MqttService : public Service, public TMessageSubscriber<MqttService, WifiConnected, WifiDisconnected> {
    AsyncMqttClient _mqttClient;

    std::string _clientId;
    std::string _username;
    std::string _password;
    std::string _host;
    uint16_t _port{};

public:
    explicit MqttService(IRegistry *registry)
            : Service(registry) {}
public:
    [[nodiscard]] ServiceId getServiceId() const override {
        return LibServiceId::MQTT;
    }

    void setup() override {
        Service::getMessageBus()->subscribe(this);

        _mqttClient.onMessage([this](auto &&PH1, auto &&PH2, auto &&PH3, auto &&PH4, auto &&PH5, auto &&PH6) {
            onMessage(std::forward<decltype(PH1)>(PH1), std::forward<decltype(PH2)>(PH2), std::forward<decltype(PH3)>(PH3),
                      std::forward<decltype(PH4)>(PH4), std::forward<decltype(PH5)>(PH5), std::forward<decltype(PH6)>(PH6));
        });
        _mqttClient.onConnect([this](auto &&PH1) { onMqttConnect(std::forward<decltype(PH1)>(PH1)); });
        _mqttClient.onDisconnect([this](auto &&PH1) { onMqttDisconnect(std::forward<decltype(PH1)>(PH1)); });
        _mqttClient.onSubscribe([this](auto &&PH1, auto &&PH2) {
            onMqttSubscribe(std::forward<decltype(PH1)>(PH1), std::forward<decltype(PH2)>(PH2));
        });
        _mqttClient.onPublish([this](auto &&PH1) { onMqttPublish(std::forward<decltype(PH1)>(PH1)); });

        auto props = getRegistry()->getProperties();
        _clientId = props->getStr(PROP_MQTT_CLIENT_ID, "");
        _username = props->getStr(PROP_MQTT_USER, "");
        _password = props->getStr(PROP_MQTT_PASS, "");
        _host = props->getStr(PROP_MQTT_SERVER_HOST, "");
        _port = props->getUint16(PROP_MQTT_SERVER_PORT, 1883);

        _mqttClient.setServer(_host.c_str(), _port);
        _mqttClient.setCredentials(_username.c_str(), _password.c_str());
        _mqttClient.setClientId(_clientId.c_str());
        _mqttClient.setKeepAlive(60000);

        mqtt::log::info(
                "user: {}, clientId: {}",
                props->getStr(PROP_MQTT_USER, ""),
                props->getStr(PROP_MQTT_CLIENT_ID, "")
        );
        mqtt::log::info(
                "server: {}:{}",
                props->getStr(PROP_MQTT_SERVER_HOST, ""),
                props->getUint16(PROP_MQTT_SERVER_PORT, 1883)
        );
    }

    void onMessage(const char *topic, const char *payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
        sendMessage(getMessageBus(), MqttMessage{});

        mqtt::log::info("msg: {}:{}", topic, std::string(payload, len));
    }

    void onMqttConnect(bool sessionPresent) {
        mqtt::log::info("connected");
        sendMessage(getMessageBus(), MqttConnected{_host+":"+std::to_string(_port)});

        _mqttClient.subscribe("v1/devices/me/rpc/request/+", 1);
    }

    void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
        mqtt::log::info("disconnected: {}", reason);
        sendMessage(getMessageBus(), MqttDisconnected{});
        if (WiFi.isConnected()) {
            _mqttClient.connect();
        }
    }

    void onMqttSubscribe(uint16_t packetId, uint8_t qos) {

    }

    void onMqttPublish(uint16_t packetId) {

    }

    void onMessage(const WifiConnected &msg) {
        mqtt::log::info("{} start connect", _mqttClient.getClientId());
        _mqttClient.connect();
    }

    void onMessage(const WifiDisconnected &msg) {
        _mqttClient.disconnect();
    }

    void publish(std::string_view topic, std::string_view data) {
        if (_mqttClient.connected()) {
            mqtt::log::debug("pub: {}:{}", topic, data);
            _mqttClient.publish(topic.data(), 0, true, data.data());
        }
    }
};

#endif