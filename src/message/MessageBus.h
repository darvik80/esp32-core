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
#include "system/Timer.h"

class MessageBus : public MessageSubscriber, public MessageProducer {
public:
    inline void subscribe(MessageSubscriber &subscriber) {
        subscribe(&subscriber);
    }

    virtual void subscribe(MessageSubscriber *subscriber) = 0;

    virtual void loop() = 0;
};

template<size_t queueSize = 10, size_t timerSize = 10>
class TMessageBus : public MessageBus {
    struct MessageHolder {
        std::shared_ptr<Message> msg;
    };

    std::array<Timer, timerSize> _timers;
    QueueHandle_t _queue;

    std::vector<MessageSubscriber *> _subscribers;
public:
    TMessageBus() {
        _queue = xQueueCreate(queueSize, sizeof(void *));
    }

    void subscribe(MessageSubscriber *subscriber) override {
        _subscribers.emplace_back(subscriber);
    }

    void onMessage(const Message &msg) override {
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
                postMessage(*holder->msg.get());
                delete holder;
                holder = nullptr;
            }
        }
    }

    void sendMessage(const Message &msg) override {
        onMessage(msg);
    }

    void postMessage(const Message::Ptr &msg) override {
        if (_queue) {
            auto* holder = new MessageHolder{msg};
            xQueueSendToBack(_queue, &holder, portMAX_DELAY);
        }
    }

    void postMessageISR(const std::shared_ptr<Message> &msg) override {
        if (_queue) {
            auto holder = new MessageHolder{msg};
            xQueueSendFromISR(_queue, &holder, nullptr);
        }
    }

    bool schedule(uint32_t delay, bool repeat, const std::function<void()> &callback) override {
        for (auto &timer: _timers) {
            if (timer.isActive()) {
                continue;
            }

            timer.attach(delay, repeat, callback);
            return true;
        }

        return false;
    }

    virtual ~TMessageBus() {
        vQueueDelete(_queue);
    }
};

inline static void sendMessage(MessageProducer &pub, const Message &msg) {
    pub.sendMessage(msg);
}

inline static void sendMessage(MessageProducer *pub, const Message &msg) {
    sendMessage(*pub, msg);
}

inline static bool schedule(MessageProducer &pub, uint32_t delay, bool repeat, const std::function<void()> &callback) {
    return pub.schedule(delay, repeat, callback);
}

inline static bool schedule(MessageProducer *pub, uint32_t delay, bool repeat, const std::function<void()> &callback) {
    return schedule(*pub, delay, repeat, callback);
}

inline static void sendMessage(MessageProducer &pub, const std::shared_ptr<Message> &msg) {
    pub.postMessage(msg);
}

inline static void sendMessage(MessageProducer *pub, const std::shared_ptr<Message> &msg) {
    sendMessage(*pub, msg);
}
