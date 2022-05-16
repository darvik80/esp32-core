//
// Created by Ivan Kishchenko on 09.12.2021.
//

#pragma once

#include "Message.h"

#include <functional>

class MessageProducer {
public:
    virtual void sendMessage(const Message &msg) = 0;
    virtual void postMessage(const Message::Ptr &msg) = 0;
    virtual void postMessageISR(const Message::Ptr &msg) = 0;
    virtual bool schedule(uint32_t delay, bool repeat, const std::function<void()>& callback) = 0;
};