//
// Created by Ivan Kishchenko on 10.12.2021.
//

#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include <functional>

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

#include "SensorLogger.h"

class ISensorEvent {
public:
    [[nodiscard]] virtual uint8_t type() const = 0;

};

class ISensorContainer;

#define SENSOR_ISP 0x0001

class ISensor {
public:
    [[nodiscard]] virtual ISensorContainer *owner() const = 0;

    virtual uint16_t flags() = 0;

    virtual void setup() = 0;

    virtual void loop() = 0;

    virtual uint32_t getISPEvent() { return 0; }

    virtual bool handleISPEvent(uint32_t event) { return false; }

    virtual ~ISensor() = default;
};

class ISensorContainer {
public:
    virtual void setupSensors() = 0;

    virtual void loopSensors() = 0;

    virtual void fireEvent(const ISensorEvent &event) = 0;

    virtual void fireEventISR(uint32_t packetEvent) = 0;

    virtual ~ISensorContainer() = default;
};

template<uint8_t sensorType>
struct TSensorEvent : ISensorEvent {
    enum {
        TYPE = sensorType
    };

    [[nodiscard]] uint8_t type() const override {
        return sensorType;
    };

    virtual uint32_t pack() {
        return type();
    }
};

template<typename T>
class TSensor : public ISensor {
    ISensorContainer *_owner;
public:
    explicit TSensor(ISensorContainer *owner)
            : _owner(owner) {}

    [[nodiscard]] ISensorContainer *owner() const override {
        return _owner;
    }

    uint16_t flags() override {
        return 0;
    }

    void setup() override {

    }

    void loop() override {

    }
};

class SensorContainer : public ISensorContainer, public ISensor {
    struct EventHolder {
        void *event;
    };
    typedef std::unique_ptr<ISensor> Ptr;
    typedef std::vector<Ptr> SensorVector;
    typedef std::vector<std::tuple<uint8_t, std::function<void(const ISensorEvent &event)>>> CallbackVector;

    SensorVector _sensors;
    CallbackVector _callbacks;

    ISensorContainer *_owner;
    QueueHandle_t _queue;
public:
    SensorContainer()
            : _owner(nullptr) {
        _queue = xQueueCreate(10, sizeof(uint32_t));
    }

    uint16_t flags() override {
        return 0;
    }

    explicit SensorContainer(ISensorContainer *owner)
            : _owner(owner) {
        _queue = xQueueCreate(10, sizeof(uint32_t));
    }

    template<typename C, typename ...T>
    C *createSensor(T &&... args) {
        auto sensor = new C(this, std::forward<T>(args)...);
        _sensors.push_back(Ptr(sensor));
        return sensor;
    }

    void setupSensors() override {
        for (auto &sensor: _sensors) {
            sensor->setup();
        }
    }

    void loopSensors() override {
        for (auto &sensor: _sensors) {
            sensor->loop();
        }
    }

    [[nodiscard]] ISensorContainer *owner() const override {
        return _owner;
    }

    void setup() override {
        setupSensors();
    }

    void loop() override {
        if (_queue) {
            uint32_t event = 0;
            while (pdPASS == xQueueReceive(_queue, &event, 0)) {
                for (auto &sensor: _sensors) {
                    if (sensor->flags() & SENSOR_ISP) {
                        if (sensor->handleISPEvent(event)) {
                            break;
                        }
                    }
                }
            }
        }
        loopSensors();
    }

    void fireEvent(const ISensorEvent &event) override {
        for (auto[id, fn]: _callbacks) {
            if (event.type() == id) {
                fn(event);
                return;
            }
        }

        if (owner()) {
            owner()->fireEvent(event);
        }
    }

    void fireEventISR(uint32_t packetEvent) override {
        if (_queue) {
            xQueueSendFromISR(_queue, &packetEvent, nullptr);
        }
    }

    template<typename E>
    void setCallback(std::function<void(const E &event)> callback) {
        for (auto &_callback: _callbacks) {
            auto[id, fn] = _callback;
            if (E::TYPE == id) {
                _callback = std::make_tuple(E::TYPE, [callback](const ISensorEvent &event) {
                    callback(static_cast<const E &>(event));
                });

                return;
            }
        }

        _callbacks.emplace_back(E::TYPE, [callback](const ISensorEvent &event) {
            callback(static_cast<const E &>(event));
        });
    }

    ~SensorContainer() override {
        vQueueDelete(_queue);
    }
};
