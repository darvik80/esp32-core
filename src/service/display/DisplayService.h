//
// Created by Kishchenko, Ivan on 11/11/21.
//

#pragma once

#ifdef OLED_SERVICE

#include "service/Config.h"

#include "service/Service.h"
#include <U8g2lib.h>
#include <array>
#include <memory>
#include <pins_arduino.h>

struct DisplayText : TMessage<DISPLAY_TEXT> {
    DisplayText(int line, const std::string_view &text) : line(line), text(text) {}

    DisplayText() = default;

    int line{};
    std::string_view text;
};

class DisplayService : public TService<Service_Display> , public TMessageSubscriber<DisplayService, DisplayText>{
    std::unique_ptr<U8G2> _display;
    unsigned long _lastUpdate{0};
    std::array<std::string, 5> _lines;
private:
    void refresh() {
        if (_display) {
            _display->firstPage();
            do {
                _display->clearBuffer();                    // clear the internal memory
                _display->setFont(u8g2_font_6x12_t_cyrillic);    // choose a suitable font
                for (auto idx = 0; idx < _lines.size(); ++idx) {
                    _display->drawStr(10, 10 + (idx * 10), _lines[idx].c_str());
                }
            } while (_display->nextPage());
        }
    }

public:
    void setText(int line, std::string_view text) {
        if (line >= _lines.size()) {
            return;
        }

        _lines[line] = text;
    }

    void setup() override {
        getRegistry().getMessageBus().subscribe(this);

        _display = std::make_unique<U8G2_SSD1306_128X64_NONAME_1_SW_I2C>(U8G2_R0, SCL, SDA);
        _display->begin();
    }

    void loop() override {
        if (millis() - _lastUpdate > 200) {
            refresh();
            _lastUpdate = millis();
        }
    }

    void onMessage(const DisplayText& msg) {
        setText(msg.line, msg.text);
    }
};

#endif