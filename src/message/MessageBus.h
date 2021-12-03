//
// Created by Ivan Kishchenko on 07.11.2021.
//

#pragma once

#include "Message.h"
#include "MessageSubscriber.h"

class MessageBus : public IMessageSubscriber {
    std::vector<IMessageSubscriber *> _subscribers;
public:
    void subscribe(IMessageSubscriber *subscriber) {
        _subscribers.emplace_back(subscriber);
    }

    void onMessage(const IMessage &msg) override {
        for (const auto sub: _subscribers) {
            sub->onMessage(msg);
        }
    }
};

inline static void sendMessage(IMessageSubscriber *sub, const IMessage &msg) {
    sub->onMessage(msg);
}
