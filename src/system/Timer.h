//
// Created by Ivan Kishchenko on 15.05.2022.
//


#pragma once

#include <functional>

extern "C" {
#include "esp_timer.h"
}

class Timer {
    std::function<void()> _callback;

    esp_timer_handle_t _timer{};
private:
    static void onCallback(void *arg) {
        auto *timer = static_cast<Timer *>(arg);
        timer->doCallback();
    }

    void doCallback() {
        _callback();
    }

public:
    void attach(uint32_t milliseconds, bool repeat, const std::function<void()> &callback) {
        _callback = callback;

        esp_timer_create_args_t _timerConfig;
        _timerConfig.arg = reinterpret_cast<void *>(this);
        _timerConfig.callback = onCallback;
        _timerConfig.dispatch_method = ESP_TIMER_TASK;
        _timerConfig.name = "Ticker";
        if (_timer) {
            esp_timer_stop(_timer);
            esp_timer_delete(_timer);
        }
        esp_timer_create(&_timerConfig, &_timer);
        if (repeat) {
            esp_timer_start_periodic(_timer, milliseconds * 1000ULL);
        } else {
            esp_timer_start_once(_timer, milliseconds * 1000ULL);
        }
    }

    bool isActive() {
        if (!_timer) {
            return false;
        }

        return esp_timer_is_active(_timer);
    }

    void detach() {
        if (_timer) {
            esp_timer_stop(_timer);
            esp_timer_delete(_timer);
            _timer = nullptr;
        }
    }

    virtual ~Timer() {
        detach();
    }
};
