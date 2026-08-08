#include "variant.h"
#include <Arduino.h>

void earlyInitVariant()
{
    pinMode(LED_LORA, OUTPUT);
    digitalWrite(LED_LORA, !LED_STATE_ON);
}
