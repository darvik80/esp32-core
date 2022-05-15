//
// Created by Kishchenko, Ivan on 11/11/21.
//

#pragma once

#include <string>
#include <cstdint>
#include <unordered_map>
#include <variant>

typedef void* PropertyPtr;

class PropertiesSource {
protected:
    virtual PropertyPtr getProperty(const std::string &name) = 0;

public:
    virtual std::string getStr(const std::string &name, const std::string &def) = 0;

    virtual void setStr(const std::string &name, const std::string &value) = 0;

    virtual uint16_t getUint16(const std::string &name, uint16_t def) = 0;

    virtual void setUint16(const std::string &name, uint16_t value) = 0;

    virtual void setProperty(const std::string &name, PropertyPtr props) = 0;

    template<class T>
    T *get(const std::string &name) {
        return static_cast<T *>(getProperty(name));
    }
};

class InCodePropertiesSource : public PropertiesSource {
    typedef std::variant<std::string, uint16_t, PropertyPtr> PropertyStorage;
    std::unordered_map<std::string, PropertyStorage> _props;
public:
    std::string getStr(const std::string &name, const std::string &def) override {
        auto iter = _props.find(name);
        if (iter != _props.end()) {
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
        auto iter = _props.find(name);
        if (iter != _props.end()) {
            return std::get<uint16_t>(iter->second);
        }

        return def;
    }

    void setProperty(const std::string &name, PropertyPtr props) override {
        _props.emplace(name, props);
    }

protected:
    PropertyPtr getProperty(const std::string &name) override {
        auto iter = _props.find(name);
        if (iter != _props.end()) {
            return std::get<PropertyPtr>(iter->second);
        }

        return nullptr;
    }

};

