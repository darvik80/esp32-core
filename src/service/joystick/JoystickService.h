//
// Created by Kishchenko, Ivan on 11/11/21.
//

#pragma once

#include "service/Config.h"

#ifdef JOY_SERVICE

#include "logging/Logging.h"

LOG_COMPONENT_SETUP(joy);

#include <driver/adc.h>

#include "service/Service.h"
#include "message/Message.h"
#include "sensor/ButtonSensor.h"
#include "sensor/AxisSensor.h"

template<adc1_channel_t rAxisX, adc1_channel_t rAxisY = ADC1_CHANNEL_MAX, uint8_t rBtn = 0, adc1_channel_t lAxisX = ADC1_CHANNEL_MAX, adc1_channel_t lAxisY = ADC1_CHANNEL_MAX, uint8_t lBtn = 0>
class JoystickService : public Service, public SensorContainer {
    JoystickEvent _event;
public:
    explicit JoystickService(IRegistry *registry)
            : Service(registry) {}


    [[nodiscard]] ServiceId getServiceId() const override {
        return LibServiceId::JOYSTICK;
    }

    void setup() override {
        adc1_config_width(ADC_WIDTH_BIT_12);
        auto axisLeft = createSensor<SensorContainer>();
        if (lAxisX != ADC1_CHANNEL_MAX) {
            axisLeft->createSensor<AxisSensor<lAxisX>>();
        }
        if (lAxisY != ADC1_CHANNEL_MAX) {
            axisLeft->createSensor<AxisSensor<lAxisY>>();
        }
        if (lBtn) {
            axisLeft->createSensor<ButtonSensor<lBtn>>();
        }

        auto axisRight = createSensor<SensorContainer>();
        if (rAxisX != ADC1_CHANNEL_MAX) {
            axisRight->createSensor<AxisSensor<rAxisX>>();
        }
        if (rAxisY != ADC1_CHANNEL_MAX) {
            axisRight->createSensor<AxisSensor<rAxisY>>();
        }
        if (rBtn) {
            axisRight->createSensor<ButtonSensor<rBtn>>();
        }

        setCallback<AxisEvent>([this](const AxisEvent &event) {
            if (lAxisX == event.channel) {
                _event.leftAxis.x = adc1_get_raw(lAxisX);
            } else if (lAxisY == event.channel) {
                _event.leftAxis.y = adc1_get_raw(lAxisY);
            } else if (rAxisX == event.channel) {
                _event.rightAxis.x = adc1_get_raw(rAxisX);
            } else if (rAxisY == event.channel) {
                _event.rightAxis.y = adc1_get_raw(rAxisY);
            }

            getMessageBus()->sendMessage(_event);

        });

        setCallback<ButtonEvent>([this](const ButtonEvent &event) {
            bool status = !event.state;
            if (event.pin == lBtn) {
                if (_event.leftAxis.btn != status) {
                    _event.leftAxis.btn = status;
                    getMessageBus()->sendMessage(_event);
                }
            } else if (event.pin == rBtn) {
                if (_event.rightAxis.btn != status) {
                    _event.rightAxis.btn = status;
                    getMessageBus()->sendMessage(_event);
                }
            }
        });

        SensorContainer::setupSensors();
        getMessageBus()->sendMessage(_event);
    }

    void loop() override {
        SensorContainer::loop();
        Service::loop();
    }
};

#endif