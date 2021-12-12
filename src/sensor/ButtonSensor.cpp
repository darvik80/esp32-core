//
// Created by Ivan Kishchenko on 12.12.2021.
//
#include "ButtonSensor.h"

void IRAM_ATTR btnHandleInterrupt(void *args) {
    auto sensor = static_cast<ISensor *>(args);
    auto event = sensor->getISPEvent() ;
    vTaskDelay(0);
    sensor->owner()->fireEventISR(event);
    vTaskDelay(0);
}

