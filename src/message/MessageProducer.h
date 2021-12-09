//
// Created by Ivan Kishchenko on 09.12.2021.
//

#pragma once

#include "Message.h"

class IMessageProducer {
public:
    virtual void sendMessage(const IMessage &msg) = 0;
    virtual void sendMessage(const IMessage::Ptr &msg) = 0;
    virtual void scheduleMessage(uint32_t delay, bool repeat, const IMessage::Ptr &msg) = 0;
};