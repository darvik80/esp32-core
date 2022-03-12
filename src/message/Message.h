//
// Created by Ivan Kishchenko on 07.11.2021.
//

#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <memory>

typedef uint_least8_t MsgId;

struct IMessage {
public:
    typedef std::shared_ptr<IMessage> Ptr;
    [[nodiscard]] virtual MsgId getMsgId() const = 0;
};

template<MsgId id>
struct TMessage : IMessage {
public:
    enum {
        ID = id
    };

    [[nodiscard]] MsgId getMsgId() const override {
        return ID;
    }
};
