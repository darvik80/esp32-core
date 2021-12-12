//
// Created by Ivan Kishchenko on 12.12.2021.
//
#include "ButtonSensor.h"

void btnHandleInterrupt(void *args) {
    static_cast<ISensor *>(args)->owner()->fireEventISR(static_cast<ISensorISP *>(args)->getISPEvent());
}
