//
// Created by Ivan Kishchenko on 16.09.2021.
//

#pragma once

#include <vector>
#include <fmt/format.h>
#include <HardwareSerial.h>

#define DECLARE_LEVEL(LEVEL) \
    template <class... A> \
    static void LEVEL(const char* fmt, A&&... args) { \
        CompositeLogger::instance().log(level::LEVEL, nullptr, fmt::format(fmt, std::forward<A>(args)...).c_str()); \
    }

namespace logging {
    enum class level {
        debug,
        info,
        warning,
        error,
        critical
    };

    class Logger {
    public:
        template<class... A>
        void log(level lvl, const std::string &fmt, A &&... args) {
            log(lvl, nullptr, fmt::format(fmt, std::forward<A>(args)...));
        }

        template<class... A>
        void log(level lvl, const char *module, const std::string &fmt, A &&... args) {
            log(lvl, module, fmt::format(fmt, std::forward<A>(args)...));
        }

        virtual void log(level lvl, const char *module, const char *message) = 0;

        virtual ~Logger() = default;
    };

    class SerialLogger : public Logger {
    public:
        void log(level lvl, const char *module, const char *message) override {
            Serial.printf("%08lu", millis());
            switch (lvl) {
                case level::debug:
                    Serial.print(" [debug]");
                    break;
                case level::info:
                    Serial.print(" [ info]");
                    break;
                case level::warning:
                    Serial.print(" [ warn]");
                    break;
                case level::error:
                    Serial.print(" [error]");
                    break;
                case level::critical:
                    Serial.print(" [ crit]");
                    break;
                default:
                    Serial.print(" [ unk]");
                    break;
            }
            if (module) {
                Serial.print(" [");
                Serial.print(module);
                Serial.print("]");
            }

            Serial.print(": ");

            Serial.println(message);
        }
    };

    class SerialColorLogger : public SerialLogger {
    public:
        void log(level lvl, const char *module, const char *message) override {
            Serial.printf("\033[38;5;15m%08lu [", millis());
            switch (lvl) {
                case level::debug:
                    Serial.print("\033[1;37mdebug");
                    break;
                case level::info:
                    Serial.print("\033[1;32m info");
                    break;
                case level::warning:
                    Serial.print("\033[1;33m warn");
                    break;
                case level::error:
                    Serial.print("\033[1;31merror");
                    break;
                case level::critical:
                    Serial.print("\033[1;31m crit");
                    break;
                default:
                    Serial.print("\033[1;31m  unk");
                    break;
            }
            Serial.print("\033[38;5;15m]");

            if (module) {
                Serial.print(" [\033[34m");
                Serial.print(module);
                Serial.print("\033[38;5;15m]");
            }

            Serial.print(": ");

            Serial.println(message);
        }
    };

    class CompositeLogger : public Logger {
        level _level{level::info};
        std::vector<Logger *> _loggers;
    private:
        CompositeLogger() = default;

        CompositeLogger(CompositeLogger &dup) = default;

    public:
        static CompositeLogger &instance() {
            static CompositeLogger inst;
            return inst;
        }

        void log(level lvl, const char *module, const char *message) override {
            if (lvl >= _level) {
                for (auto logger: _loggers) {
                    logger->log(lvl, module, message);
                }
            }
        }


        void setLevel(level lvl) {
            _level = lvl;
        }

        void addLogger(Logger *log) {
            _loggers.emplace_back(log);
        }

        ~CompositeLogger() override {
            for (auto *logger: _loggers) {
                delete logger;
            }
        }
    };

    void inline addLogger(Logger *log) {
        CompositeLogger::instance().addLogger(log);
    }

    void inline setLogLevel(level lvl) {
        CompositeLogger::instance().setLevel(lvl);
    }

    DECLARE_LEVEL(debug);

    DECLARE_LEVEL(info);

    DECLARE_LEVEL(warning);

    DECLARE_LEVEL(error);

    DECLARE_LEVEL(critical);
}

#define DECLARE_COMPONENT_LEVEL(COMPONENT, LEVEL)                                                                                   \
    template <class... A>                                                                                                           \
    static void LEVEL(const char* fmt, A&&... args) {                                                                               \
        logging::CompositeLogger::instance().log(logging::level::LEVEL, #COMPONENT, fmt::format(fmt, std::forward<A>(args)...).c_str());     \
    }

#define LOG_COMPONENT_SETUP(COMPONENT)                  \
namespace COMPONENT {                                   \
    namespace  log {                                    \
                                                        \
        DECLARE_COMPONENT_LEVEL(COMPONENT, debug)       \
                                                        \
        DECLARE_COMPONENT_LEVEL(COMPONENT, info)        \
                                                        \
        DECLARE_COMPONENT_LEVEL(COMPONENT, warning)     \
                                                        \
        DECLARE_COMPONENT_LEVEL(COMPONENT, error)       \
                                                        \
        DECLARE_COMPONENT_LEVEL(COMPONENT, critical)    \
    }                                                   \
}


