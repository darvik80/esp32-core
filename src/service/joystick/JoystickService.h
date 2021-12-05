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

class JoystickService : public Service {
    JoystickEvent _event;
    JoystickProperties *_props{nullptr};
public:
    explicit JoystickService(IRegistry *registry)
            : Service(ServiceId::JOYSTICK, registry) {}

    void setup() override {
        _props = getRegistry()->getProperties()->get<JoystickProperties>(PROP_JOYSTICK);
        if (_props) {
            adc1_config_width(_props->bitsWidth);
            adc1_config_channel_atten(_props->leftAxis.channelX, ADC_ATTEN_DB_11);
            adc1_config_channel_atten(_props->leftAxis.channelY, ADC_ATTEN_DB_11);

            adc1_config_channel_atten(_props->rightAxis.channelX, ADC_ATTEN_DB_11);
            adc1_config_channel_atten(_props->rightAxis.channelY, ADC_ATTEN_DB_11);
        }
    }

    void loop() override {
        if (_props) {
            int leftX = _props->adcMaxVal - adc1_get_raw(_props->leftAxis.channelX);
            int leftY = _props->adcMaxVal - adc1_get_raw(_props->leftAxis.channelY);

            int rightX = _props->adcMaxVal - adc1_get_raw(_props->rightAxis.channelX);
            int rightY = _props->adcMaxVal - adc1_get_raw(_props->rightAxis.channelY);

            if (leftX != _event.leftAxis.x || leftY != _event.leftAxis.y || rightX != _event.rightAxis.x || rightY != _event.rightAxis.y) {
                _event.leftAxis.x = leftX;
                _event.leftAxis.y = leftY;

                _event.rightAxis.x = rightX;
                _event.rightAxis.y = rightY;

                sendMessage(getMessageBus(), _event);
            }
        }
    }
};

#endif