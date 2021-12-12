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

#define ADC_MAX_VAL 4096

#define PROP_JOYSTICK  "prop.joy"

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

template<adc1_channel_t lAxisX, adc1_channel_t lAxisY, uint8_t lBtn, adc1_channel_t rAxisX, adc1_channel_t rAxisY, uint8_t rBtn>
class JoystickService : public Service, public SensorContainer {
    JoystickEvent _event;
    JoystickProperties *_props{nullptr};
public:
    explicit JoystickService(IRegistry *registry)
            : Service(registry) {}


    [[nodiscard]] ServiceId getServiceId() const override {
        return LibServiceId::JOYSTICK;
    }

    void setup() override {
        auto axisLeft = createSensor<SensorContainer>();
        if (lAxisX) {
            axisLeft->createSensor<AxisSensor<lAxisX>>();
        }
        if (lAxisY) {
            axisLeft->createSensor<AxisSensor<lAxisY>>();
        }
        if (lBtn) {
            axisLeft->createSensor<ButtonSensor<lBtn>>();
        }

        auto axisRight = createSensor<SensorContainer>();
        if (rAxisX) {
            axisRight->createSensor<AxisSensor<rAxisX>>();
        }
        if (rAxisY) {
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
            if (event.pin == lBtn) {
                _event.leftAxis.btn = true;
            } else if (event.pin == rBtn) {
                _event.rightAxis.btn = true;
            }

            getMessageBus()->sendMessage(_event);

            _event.leftAxis.btn = false;
            _event.rightAxis.btn = false;
        });
        SensorContainer::setupSensors();
    }
};

#endif