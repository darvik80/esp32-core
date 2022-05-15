//
// Created by Ivan Kishchenko on 09.12.2021.
//

#pragma once

#include "Message.h"

#include <functional>

class IMessageProducer {
public:
    virtual void sendMessage(const Message &msg) = 0;
    virtual void sendMessage(const Message::Ptr &msg) = 0;
    virtual void sendMessageISR(const Message::Ptr &msg) = 0;
    virtual bool schedule(uint32_t delay, bool repeat, const std::function<void()>& callback) = 0;
};