//
// Created by Kishchenko, Ivan on 11/11/21.
//

#pragma once

#ifdef JOY_SERVICE

#include "service/Config.h"

#include "logging/Logging.h"

LOG_COMPONENT_SETUP(joy);

#include <driver/adc.h>

#include "service/TService.h"
#include "message/Message.h"
#include "sensor/ButtonSensor.h"
#include "sensor/AxisSensor.h"

#include "esp_adc_cal.h"


struct JoystickEvent : public TMessage<JOYSTICK_EVENT> {
    struct Axis {
        int x{};
        int y{};
        bool btn{};
    };

    Axis leftAxis{};
    Axis rightAxis{};
};

struct ADCAvg {
    std::array<int, 16> buf{};
    int idx{0};
};


template<adc1_channel_t rAxisX, adc1_channel_t rAxisY = ADC1_CHANNEL_MAX, uint8_t rBtn = 0, adc1_channel_t lAxisX = ADC1_CHANNEL_MAX, adc1_channel_t lAxisY = ADC1_CHANNEL_MAX, uint8_t lBtn = 0>
class JoystickService : public TService, public SensorContainer {
    JoystickEvent _event;

    uint32_t _lastUpdate{0};
    ADCAvg rAxisXCal;
    ADCAvg rAxisYCal;
    ADCAvg lAxisXCal;
    ADCAvg lAxisYCal;
private:
    int32_t readADC_Avg(ADCAvg &cal, int raw) {
        cal.buf[cal.idx++] = raw;
        if (cal.idx == cal.buf.size()) {
            cal.idx = 0;
        }

        int sum = 0;
        for (int val: cal.buf) {
            sum += val;
        }

        return sum / (int) cal.buf.size();
    }

public:
    explicit JoystickService(Registry *registry)
            : TService(registry) {}


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
            auto val = adc1_get_raw(event.channel);
            bool updated = false;
            if (lAxisX == event.channel) {
                val = readADC_Avg(lAxisXCal, val);
                updated = std::abs(_event.leftAxis.x - val) > 8;
                _event.leftAxis.x = val;
            } else if (lAxisY == event.channel) {
                val = readADC_Avg(lAxisYCal, val);
                updated = std::abs(_event.leftAxis.y - val) > 8;
                _event.leftAxis.y = val;
            } else if (rAxisX == event.channel) {
                val = readADC_Avg(rAxisXCal, val);
                updated = std::abs(_event.rightAxis.x - val) > 8;
                _event.rightAxis.x = val;
            } else if (rAxisY == event.channel) {
                val = readADC_Avg(rAxisYCal, val);
                updated = std::abs(_event.rightAxis.y - val) > 8;
                _event.rightAxis.y = val;
            }

            if (updated || (millis()-_lastUpdate > 1000)) {
                getMessageBus()->sendMessage(_event);
                _lastUpdate = millis();
            }
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
        TService::loop();
    }
};

#endif