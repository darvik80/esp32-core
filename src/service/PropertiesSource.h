//
// Created by Kishchenko, Ivan on 11/11/21.
//

#pragma once

#include <string>
#include <string_view>
#include <cstdint>
#include <unordered_map>
#include <variant>

struct Property {
    virtual ~Property() = default;
};

class PropertiesSource {
public:
    virtual std::string getStr(const std::string &name, const std::string &def) = 0;

    virtual void setStr(const std::string &name, const std::string &value) = 0;

    virtual uint16_t getUint16(const std::string &name, uint16_t def) = 0;

    virtual void setUint16(const std::string &name, uint16_t value) = 0;

    virtual Property *getProperty(const std::string &name) = 0;
    virtual void setProperty(const std::string &name, Property* props) = 0;

    template<class T>
    T *get(const std::string &name) {
        return static_cast<T *>(getProperty(name));
    }
};


class InCodePropertiesSource : public PropertiesSource {
    typedef std::variant<std::string, uint16_t, Property*> PropertyStorage;
    std::unordered_map<std::string, PropertyStorage> _props;
public:
    std::string getStr(const std::string &name, const std::string &def) override {
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

    void setProperty(const std::string &name, Property* props)  override {
        _props.emplace(name, props);
    }

    Property *getProperty(const std::string &name) override {
        if (auto iter = _props.find(name); iter != _props.end()) {
            return std::get<Property*>(iter->second);
        }

        return nullptr;
    }

};

