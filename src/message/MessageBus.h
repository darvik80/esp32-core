//
// Created by Ivan Kishchenko on 07.11.2021.
//

#pragma once

#include "Message.h"
#include "MessageSubscriber.h"

class MessageBus : public IMessageSubscriber, public IMessageProducer {
    struct MessageHolder {
        std::shared_ptr<IMessage> msg;
    };
    std::vector<IMessageSubscriber *> _subscribers;

    QueueHandle_t _queue{nullptr};
public:
    MessageBus() {
        _queue = xQueueCreate(10, sizeof(void*));
    }

    void subscribe(IMessageSubscriber *subscriber) {
        _subscribers.emplace_back(subscriber);
    }

    void onMessage(const IMessage &msg) override {
        for (const auto sub: _subscribers) {
            sub->onMessage(msg);
        }
    }

    void loop() {
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
        msg::log::debug("on msg: {}", msg.getMsgId());
        onMessage(msg);
    }

    void sendMessage(const std::shared_ptr<IMessage> &msg) override {
        if (_queue) {
            msg::log::debug("send msg: {}", msg->getMsgId());
            auto holder = new MessageHolder{msg};
            xQueueSendToBack(_queue, &holder, portMAX_DELAY);
        }
    }
};

inline static void sendMessage(IMessageProducer *pub, const IMessage &msg) {
    pub->sendMessage(msg);
}

inline static void sendMessage(IMessageProducer *pub, const std::shared_ptr<IMessage> &msg) {
    pub->sendMessage(msg);
}
