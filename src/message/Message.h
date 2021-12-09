//
// Created by Ivan Kishchenko on 07.11.2021.
//

#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <string_view>
#include <memory>

typedef uint_least8_t MsgId;

class IMessage {
public:
    typedef std::shared_ptr<IMessage> Ptr;
    [[nodiscard]] virtual MsgId getMsgId() const = 0;
};

template<MsgId id>
class Message : public IMessage {
public:
    enum {
        ID = id
    };

    [[nodiscard]] MsgId getMsgId() const override {
        return ID;
    }
};
