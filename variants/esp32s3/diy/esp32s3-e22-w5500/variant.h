#pragma once

// Button
#if !defined(E22_W5500_NO_BASEUI_BUTTON_PIN)
#define BUTTON_PIN 0 // BOOT button
#else
#undef HAS_BUTTON
#define HAS_BUTTON 1
#endif

// SX1262 / SX1268
#define USE_SX1262
#define USE_SX1268
#define SX126X_SCK 21
#define SX126X_MOSI 38
#define SX126X_MISO 39
#define SX126X_CS 14
#define SX126X_DIO1 42
#define SX126X_RESET 40
#define SX126X_BUSY 41
#define SX126X_TXEN 9
#define SX126X_RXEN 10
#define SX126X_MAX_POWER 22
#define SX126X_DIO3_TCXO_VOLTAGE 1.8

// Lora
#define LORA_SCK SX126X_SCK
#define LORA_MOSI SX126X_MOSI
#define LORA_MISO SX126X_MISO
#define LORA_CS SX126X_CS
#define LORA_DIO1 SX126X_DIO1
#define LORA_RESET SX126X_RESET

// LED
#define LED_POWER 13
#define LED_LORA 2
#define LED_STATE_ON 1

// Buzzer
#define PIN_BUZZER 11

// RGB LED
#define ENABLE_AMBIENTLIGHTING
#define HAS_NEOPIXEL
#define NEOPIXEL_COUNT 1
#define NEOPIXEL_DATA 48
#define NEOPIXEL_TYPE (NEO_GRB + NEO_KHZ800)

// I2C
#define I2C_SCL 18
#define I2C_SDA 8

// OLED
#if !defined(MESHTASTIC_EXCLUDE_SCREEN)
#define HAS_SCREEN 1
#define USE_SSD1306
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#endif

// E-Ink display
#define PIN_EINK_BUSY 1
#define PIN_EINK_SCLK LORA_SCK
#define PIN_EINK_MOSI LORA_MOSI
#define PIN_EINK_CS 15
#define PIN_EINK_DC 16
#define PIN_EINK_RES 17

// Ethernet
#define HAS_ETHERNET 1
#define USE_WS5500 1 // This driver uses the same stack as the ESP32 WiFi driver.
#define USE_ETHERNET_DEFAULT 1
#define ETH_ADDR 1
#define ETH_MISO_PIN 4
#define ETH_MOSI_PIN 5
#define ETH_SCLK_PIN 6
#define ETH_CS_PIN 7
#define ETH_INT_PIN 47
#define ETH_RST_PIN -1
