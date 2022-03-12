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

#include <mqtt_client.h>


#define PROP_MQTT_SERVER_HOST "mqtt.host"
#define PROP_MQTT_SERVER_PORT "mqtt.port"
#define PROP_MQTT_USER "mqtt.user"
#define PROP_MQTT_PASS "mqtt.pass"
#define PROP_MQTT_CLIENT_ID "mqtt.client.id"

class MqttService
        : public Service, public TMessageSubscriber<MqttService, WifiConnected, MqttMessage> {
    esp_mqtt_client_handle_t _client{};

    std::string _clientId;
    std::string _username;
    std::string _password;
    std::string _uri;
private:
    static void
    mqtt_event_callback(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
        ((MqttService *) event_handler_arg)->onEvent((esp_mqtt_event_handle_t) event_data);
    }

public:
    explicit MqttService(IRegistry *registry)
            : Service(registry) {}

    [[nodiscard]] ServiceId getServiceId() const override {
        return LibServiceId::MQTT;
    }


    void onEvent(esp_mqtt_event_handle_t event) {
        switch (event->event_id) {
            case MQTT_EVENT_CONNECTED:
                esp_mqtt_client_subscribe(_client, "v1/devices/me/rpc/request/+", 1);
                mqtt::log::info("connected");
#if defined OLED_SERVICE
                getRegistry()->getService<DisplayService>(LibServiceId::OLED)->setText(1, "MQTT connected");
#endif
                break;
            case MQTT_EVENT_DISCONNECTED:
                mqtt::log::info("disconnected");
                break;
            case MQTT_EVENT_SUBSCRIBED:
                mqtt::log::info("subscribed");
                break;
            case MQTT_EVENT_ERROR:
                mqtt::log::info("error");
                break;
            default:
                break;

        }
    }

    void setup() override {
        Service::getMessageBus()->subscribe(this);

        auto props = getRegistry()->getProperties();
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

        _clientId = props->getStr(PROP_MQTT_CLIENT_ID, "");
        _username = props->getStr(PROP_MQTT_USER, "");
        _password = props->getStr(PROP_MQTT_PASS, "");
        _uri = fmt::format(
                "mqtt://{}:{}",
                props->getStr(PROP_MQTT_SERVER_HOST, ""),
                props->getUint16(PROP_MQTT_SERVER_PORT, 1883)
        );
    }

    void onMessage(const WifiConnected &msg) {
        mqtt::log::info("handle wifi-conn, try connect to mqtt");
        const esp_mqtt_client_config_t config{
                .uri = _uri.c_str(),
                .client_id = _clientId.c_str(),
                .username = _username.c_str(),
                .user_context = (void *) this,
        };

        _client = esp_mqtt_client_init(&config);
        esp_mqtt_client_register_event(_client, MQTT_EVENT_ANY, mqtt_event_callback, this);
        esp_mqtt_client_start(_client);

        getRegistry()->getService<DisplayService>(LibServiceId::OLED)->setText(1, "MQTT configured");

    }

    void onMessage(const MqttMessage &msg) {
        mqtt::log::debug("pub: {}:{}", msg.topic, msg.data);
        publish(msg.topic, msg.data, msg.qos);
    }

    void publish(std::string_view topic, std::string_view data, uint8_t qos = 0) {
        esp_mqtt_client_publish(_client, topic.data(), data.data(), (int) data.length(), qos, false);
    }
};

#endif