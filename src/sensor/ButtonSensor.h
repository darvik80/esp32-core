//
// Created by Ivan Kishchenko on 12.12.2021.
//

#pragma once

#include "sensor/Sensor.h"
#include "sensor/SensorId.h"

#include <esp32-hal-gpio.h>

struct ButtonEvent : TSensorEvent<S_Button> {
    ButtonEvent(uint8_t pin, uint8_t state)
            : pin(pin), state(state) {}

    uint8_t pin{};
    bool state{};
};

void IRAM_ATTR btnHandleInterrupt(void *args);

template<uint8_t pin, uint8_t mode = INPUT_PULLUP>
class ButtonSensor : public TSensor<ButtonEvent> {
public:
    explicit ButtonSensor(ISensorContainer *owner)
            : TSensor(owner) {}

    uint16_t flags() override {
        return SENSOR_ISP;
    }

    void setup() override {
        pinMode(pin, mode);
        attachInterruptArg(pin, btnHandleInterrupt, this, RISING);
    }

    IRAM_ATTR uint32_t getISPEvent() override {
        return ((uint32_t) S_Button << 24) | (pin << 8);
    }

    bool handleISPEvent(uint32_t event) override {
        if ((uint8_t) (event >> 24) == S_Button) {
            this->owner()->fireEvent(ButtonEvent{(uint8_t) (event >> 8), digitalRead(pin)});
            return true;
        }

        return false;
    }


};