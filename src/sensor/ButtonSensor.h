//
// Created by Ivan Kishchenko on 12.12.2021.
//

#pragma once

#include "sensor/Sensor.h"
#include "sensor/SensorId.h"

#include <esp32-hal-gpio.h>

struct ButtonEvent : TSensorEvent<S_Button> {
    explicit ButtonEvent(uint8_t pin) : pin(pin) {}

    uint8_t pin{};
};

void IRAM_ATTR btnHandleInterrupt(void* args);

template<uint8_t pin, uint8_t mode = INPUT_PULLUP>
class ButtonSensor : public TSensorISP<ButtonEvent> {
public:
    explicit ButtonSensor(ISensorContainer *owner)
            : TSensorISP(owner) {}

    uint16_t flags() override {
        return SENSOR_ISP;
    }

    void setup() override {
        pinMode(pin, mode);
        attachInterruptArg(pin, btnHandleInterrupt, this, FALLING);
    }

    uint32_t getISPEvent() override {
        return ((uint32_t)S_Button << 24) | pin;
    }

    bool handleISPEvent(uint32_t event) override {
        if (((event&0xff000000) >> 24) == S_Button) {
            this->owner()->fireEvent(ButtonEvent{(uint8_t)event});
            return true;
        }

        return false;
    }


};