//
// Created by Ivan Kishchenko on 07.11.2021.
//

#pragma once

#include "Message.h"
#include "MessageSubscriber.h"
#include "MessageProducer.h"

#include <set>
#include <vector>
#include <freertos/queue.h>
#include <freertos/timers.h>

class IMessageBus : public IMessageSubscriber, public IMessageProducer {
public:
    virtual void subscribe(IMessageSubscriber *subscriber) = 0;

    virtual void loop() = 0;
};

template<size_t queueSize = 10, size_t timerSize = 10>
class TMessageBus : public IMessageBus {
    struct MessageHolder {
        std::shared_ptr<IMessage> msg;
        TimerHandle_t handler{};
        TMessageBus *bus{nullptr};
        bool repeat{false};
    };
    std::array<MessageHolder, timerSize> _timers;

    QueueHandle_t _queue;

    std::vector<IMessageSubscriber *> _subscribers;
public:
    TMessageBus() {
        _queue = xQueueCreate(queueSize, sizeof(void *));
    }

    void subscribe(IMessageSubscriber *subscriber) override {
        _subscribers.emplace_back(subscriber);
    }

    void onMessage(const IMessage &msg) override {
        msg::log::debug("on msg: {}:{}", msg.getMsgId(), _subscribers.size());
        for (const auto sub: _subscribers) {
            sub->onMessage(msg);
        }
    }

    void loop() override {
        if (_queue) {
            MessageHolder *holder = nullptr;
            while (pdPASS == xQueueReceive(_queue, &holder, 0)) {
                msg::log::debug("recv msg: {}", holder->msg->getMsgId());
                sendMessage(*holder->msg.get());
                delete holder;
            }
        }
    }

    void sendMessage(const IMessage &msg) override {
        onMessage(msg);
    }

    void sendMessage(const std::shared_ptr<IMessage> &msg) override {
        if (_queue) {
            auto holder = new MessageHolder{msg};
            xQueueSendToBack(_queue, &holder, portMAX_DELAY);
        }
    }

    void sendMessageISR(const std::shared_ptr<IMessage> &msg) override {
        if (_queue) {
            auto holder = new MessageHolder{msg};
            xQueueSendFromISR(_queue, &holder, nullptr);
        }
    }

    void scheduleMessage(uint32_t delay, bool repeat, const std::shared_ptr<IMessage> &msg) override {
        for (auto &timer: _timers) {
            if (timer.handler) {
                if (!xTimerIsTimerActive(timer.handler)) {
                    xTimerStop(timer.handler, 0);
                    xTimerDelete(timer.handler, 0);
                } else {
                    continue;
                }
            }

            msg::log::debug("schedule msg: {} {}", msg->getMsgId(), delay);

            timer.msg = msg;
            timer.repeat = repeat;
            timer.bus = this;
            timer.handler = xTimerCreate("timer", pdMS_TO_TICKS(delay), repeat, &timer, [](TimerHandle_t timer) {
                auto holder = (MessageHolder *) pvTimerGetTimerID(timer);
                msg::log::debug("delayed msg: {}", holder->msg->getMsgId());
                holder->bus->onMessage(*holder->msg.get());
            });
            xTimerStart(timer.handler, 0);

            break;
        }
    }

    virtual ~TMessageBus() {
        vQueueDelete(_queue);
    }
};

inline static void sendMessage(IMessageProducer *pub, const IMessage &msg) {
    pub->sendMessage(msg);
}

inline static void sendMessage(IMessageProducer *pub, const std::shared_ptr<IMessage> &msg) {
    pub->sendMessage(msg);
}
