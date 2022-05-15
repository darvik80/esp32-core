//
// Created by Kishchenko, Ivan on 11/12/21.
//

#pragma once

#include "service/Config.h"

#include "logging/Logging.h"

LOG_COMPONENT_SETUP(mqtt);

#include <WiFi.h>
#include <string_view>

#include "message/Message.h"

#include "service/Service.h"
#include "MqttProperties.h"

#include <mqtt_client.h>
#include <esp_tls.h>
#include <esp_crt_bundle.h>

#define PROP_MQTT_PROPS "mqtt.props"

class MqttService
        : public TService<Service_MQTT>,
          public TMessageSubscriber<MqttService, WifiConnected, MqttMessage, MqttSubscribe> {
    esp_mqtt_client_handle_t _client{};

    MqttProperties *_props{};
private:
    static void
    mqtt_event_callback(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
        ((MqttService *) event_handler_arg)->onEvent((esp_mqtt_event_handle_t) event_data);
    }

public:
    void onEvent(esp_mqtt_event_handle_t event) {
        switch (event->event_id) {
            case MQTT_EVENT_CONNECTED: {
                mqtt::log::info("connected");
                break;
                case MQTT_EVENT_DISCONNECTED:
                    mqtt::log::info("disconnected");
                break;
                case MQTT_EVENT_SUBSCRIBED:
                    mqtt::log::info("subscribed");
                break;
                case MQTT_EVENT_PUBLISHED:
                    mqtt::log::info("pub-ack: {}", event->msg_id);
                break;
                case MQTT_EVENT_ERROR:
                    mqtt::log::info("error");
                break;
                case MQTT_EVENT_DATA:
                    mqtt::log::info("data: {}:{}", event->topic, std::string(event->data, event->data_len));
                break;
                default:
                    break;

            }
        }
    }

    void setup(Registry &registry) override {
        registry.getMessageBus().subscribe(this);

        _props = registry.getProperties().get<MqttProperties>(PROP_MQTT_PROPS);
        mqtt::log::info(
                "user: {}, clientId: {}",
                _props->username,
                _props->clientId
        );
        mqtt::log::info(
                "server: {}",
                _props->uri
        );

        if (_props->rootCa != nullptr) {
            if (ESP_OK != esp_tls_init_global_ca_store()) {
                logging::error("can't inti global store");
            } else if (ESP_OK != esp_tls_set_global_ca_store((const unsigned char *) _props->rootCa,
                                                             strlen(_props->rootCa) + 1)) {
                logging::error("can't inti global store cert");
            }
        }
    }

    void onMessage(const WifiConnected &msg) {
        mqtt::log::info("handle wifi-conn, try connect to mqtt");
        const esp_mqtt_client_config_t config{
                .uri = _props->uri.c_str(),
                .client_id = _props->clientId.c_str(),
                .username = _props->username.c_str(),
                .password = _props->password.c_str(),
                .user_context = (void *) this,
                .cert_pem = _props->certDev,
                .use_global_ca_store = _props->rootCa != nullptr,
                .reconnect_timeout_ms = 10000
        };

        _client = esp_mqtt_client_init(&config);
        esp_mqtt_client_register_event(_client, MQTT_EVENT_ANY, mqtt_event_callback, this);
        esp_mqtt_client_start(_client);
    }

    void onMessage(const MqttMessage &msg) {
        publish(msg.topic, msg.data, msg.qos);
    }

    void onMessage(const MqttSubscribe &msg) {
        subscribe(msg.topic, msg.qos);
    }

    void subscribe(std::string_view topic, uint8_t qos = 0) {
        mqtt::log::debug("sub: {}:{}", topic);
        if (auto id = esp_mqtt_client_subscribe(_client, topic.data(), qos); id > 0) {
            mqtt::log::info("sub: {}:{}", topic, id);
        } else {
            mqtt::log::error("sub failed: {}", topic);
        }
    }

    void publish(std::string_view topic, std::string_view data, uint8_t qos = 0) {
        mqtt::log::debug("pub: {}:{}", topic, data);
        if (auto id = esp_mqtt_client_publish(_client, topic.data(), data.data(), (int) data.length(), qos, false);
                id > 0) {
            mqtt::log::info("sent: {}:{}", topic, id);
        } else {
            mqtt::log::error("send failed: {}", topic);
        }
    }
};