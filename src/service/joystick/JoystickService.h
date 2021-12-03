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

class JoystickService : public Service {
    JoystickEvent _event;
public:
    explicit JoystickService(Registry *registry)
            : Service(ServiceId::JOYSTICK, registry) {}

    void setup() override {
        adc1_config_width(ADC_WIDTH_BIT_12);
        adc1_config_channel_atten(ADC1_CHANNEL_1, ADC_ATTEN_DB_11);
        adc1_config_channel_atten(ADC1_CHANNEL_2, ADC_ATTEN_DB_11);

        adc1_config_channel_atten(ADC1_CHANNEL_3, ADC_ATTEN_DB_11);
        adc1_config_channel_atten(ADC1_CHANNEL_4, ADC_ATTEN_DB_11);
    }

    void loop() override {
        int leftX = ADC_MAX_VAL - adc1_get_raw(ADC1_CHANNEL_2);
        int leftY = ADC_MAX_VAL - adc1_get_raw(ADC1_CHANNEL_3);

        int rightX = ADC_MAX_VAL - adc1_get_raw(ADC1_CHANNEL_0);
        int rightY = ADC_MAX_VAL - adc1_get_raw(ADC1_CHANNEL_1);

        if (leftX != _event.leftAxis.x || leftY != _event.leftAxis.y || rightX != _event.rightAxis.x || rightY != _event.rightAxis.y) {
            _event.leftAxis.x = leftX;
            _event.leftAxis.y = leftY;

            _event.rightAxis.x = rightX;
            _event.rightAxis.y = rightY;

            sendMessage(getMessageBus(), _event);
        }
    }
};

#endif