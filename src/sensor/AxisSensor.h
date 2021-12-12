//
// Created by Ivan Kishchenko on 13.12.2021.
//

#pragma once

#include "sensor/Sensor.h"
#include "sensor/SensorId.h"
#include <driver/adc.h>

struct AxisEvent : TSensorEvent<S_Axis> {
    AxisEvent(adc1_channel_t channel, int value)
            : channel(channel), value(value) {}

    adc1_channel_t channel;
    int value;
};

template<adc1_channel_t channel>
class AxisSensor : public TSensor<AxisEvent> {
    uint32_t _lastUpdate{};
public:
    explicit AxisSensor(ISensorContainer *owner)
            : TSensor(owner) {}

    void setup() override {
        adc1_config_channel_atten(channel, ADC_ATTEN_DB_11);
    }

    void loop() override {
        if (millis()-_lastUpdate > 50) {
            auto value = adc1_get_raw(channel);
            this->owner()->fireEvent(AxisEvent{channel, value});
            _lastUpdate = millis();
        }
    }
};