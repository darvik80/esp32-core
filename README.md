# esp-core

Simple framework for ESP32/ESP8266

#Class Diagrams
## Message Bus
![alternative text](http://www.plantuml.com/plantuml/proxy?cache=no&src=https://raw.githubusercontent.com/darvik80/esp-core/sensor/puml/MessageBus.puml)

## TService
![alternative text](http://www.plantuml.com/plantuml/proxy?cache=no&src=https://raw.githubusercontent.com/darvik80/esp-core/sensor/puml/TService.puml)

## Sensor
![alternative text](http://www.plantuml.com/plantuml/proxy?cache=no&src=https://raw.githubusercontent.com/darvik80/esp-core/sensor/puml/Sensor.puml)

## Changelog

### v0.0.2

* *Sensors*

### v0.0.1
* *Wifi TService*

TService for connect to Wifi. Support auto-reconnect
```cpp
#define PROP_WIFI_SSID "wifi.ssid"
#define PROP_WIFI_PASS "wifi.pass"
```
Generate 2 events:
1. WifiConnected
2. WifiDisconnected

* *MQTT TService*

Integration with MQTT.

```cpp
#define PROP_MQTT_SERVER_HOST "mqtt.host"
#define PROP_MQTT_SERVER_PORT "mqtt.port"
#define PROP_MQTT_USER "mqtt.user"
#define PROP_MQTT_PASS "mqtt.pass"
#define PROP_MQTT_CLIENT_ID "mqtt.client.id"
```
Generate 3 events:
1. MqttConnected
2. MqttDisconnected
3. MqttMessage

* *Display TService* 

TService for work with OLED display via SDA/SCL

* *Joystick TService*
```cpp
struct JoystickProperties : Property {
    int adcMaxVal{ADC_MAX_VAL};
    adc_bits_width_t bitsWidth{ADC_WIDTH_BIT_12};
    struct JoystickAxis {
        adc1_channel_t channelX;
        adc1_channel_t channelY;
    };

    JoystickAxis leftAxis{
            .channelX = ADC1_CHANNEL_2,
            .channelY= ADC1_CHANNEL_3,
    };
    JoystickAxis rightAxis{
            .channelX = ADC1_CHANNEL_0,
            .channelY= ADC1_CHANNEL_1,
    };
};
```
Generate 1 event:
1. JoystickEvent

