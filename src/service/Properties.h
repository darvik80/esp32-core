//
// Created by Kishchenko, Ivan on 11/11/21.
//

#pragma once

#include <string>
#include <string_view>
#include <cstdint>
#include <unordered_map>
#include <variant>

class Properties {
public:
    virtual std::string getStr(const std::string& name, const std::string& def) = 0;
    virtual void setStr(const std::string& name, const std::string& value) = 0;
    virtual uint16_t getUint16(const std::string& name, uint16_t def) = 0;
    virtual void setUint16(const std::string& name, uint16_t value) = 0;
};

class InCodeProperties : public Properties {
    typedef std::variant<std::string, uint16_t> Property;
    std::unordered_map<std::string, Property> _props;
public:
    std::string getStr(const std::string &name, const std::string& def) override {
        if (auto iter = _props.find(name); iter != _props.end()) {
            return std::get<std::string>(iter->second);
        }

        return def;
    }

    void setStr(const std::string &name, const std::string &value) override {
        _props.emplace(name, value);
    }

    void setUint16(const std::string &name, uint16_t value) override {
        _props.emplace(name, value);
    }

    uint16_t getUint16(const std::string &name, uint16_t def) override {
        if (auto iter = _props.find(name); iter != _props.end()) {
            return std::get<uint16_t>(iter->second);
        }

        return def;
    }
};

