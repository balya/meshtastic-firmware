#pragma once

#ifdef MESHTASTIC_INCLUDE_NICHE_GRAPHICS

#include "SPILock.h"
#include "graphics/niche/Drivers/EInk/EInk.h"

namespace NicheGraphics::Drivers
{

class GDEY037T03 : public EInk
{
  public:
    GDEY037T03() : EInk(width, height, (UpdateTypes)(FULL | FAST)) {}

    void begin(SPIClass *spi, uint8_t pinDc, uint8_t pinCs, uint8_t pinBusy, uint8_t pinRst = -1) override
    {
        this->spi = spi;
        this->pinDc = pinDc;
        this->pinCs = pinCs;
        this->pinBusy = pinBusy;
        this->pinRst = pinRst;

        pinMode(pinDc, OUTPUT);
        pinMode(pinCs, OUTPUT);
        pinMode(pinBusy, INPUT);
        digitalWrite(pinCs, HIGH);
        digitalWrite(pinDc, HIGH);

        if (pinRst != 0xFF) {
            pinMode(pinRst, OUTPUT);
            digitalWrite(pinRst, HIGH);
        }
    }

    void update(uint8_t *imageData, UpdateTypes type) override
    {
        buffer = imageData;
        updateType = type;

        reset();
        initDisplay();

        if (updateType == FULL)
            writeImage(0x10);
        writeImage(0x13);

        configureUpdate();
        powerOn();
        sendCommand(0x12);

        beginPolling(25, updateType == FAST ? 350 : 1000);
    }

  protected:
    bool isUpdateDone() override { return digitalRead(pinBusy) == HIGH; }

    void finalizeUpdate() override
    {
        powerOff();

        if (updateType == FAST) {
            initDisplay();
            writeImage(0x10);
        }
    }

  private:
    static constexpr uint16_t width = 240;
    static constexpr uint16_t height = 416;
    static constexpr uint32_t bufferSize = width * height / 8;

    void reset()
    {
        if (pinRst == 0xFF)
            return;

        digitalWrite(pinRst, LOW);
        delay(10);
        digitalWrite(pinRst, HIGH);
        delay(10);
        wait();
    }

    void initDisplay()
    {
        sendCommand(0x00);
        sendData(0x1E);
        sendData(0x0D);
        delay(1);

        sendCommand(0x00);
        sendData(0x1F);
        sendData(0x0D);
    }

    void configureUpdate()
    {
        sendCommand(0xE0);
        sendData(0x02);
        sendCommand(0xE5);
        sendData(updateType == FAST ? 0x6E : 0x5A);

        sendCommand(0x50);
        sendData(updateType == FAST ? 0xD7 : 0x97);
    }

    void configureFullWindow()
    {
        sendCommand(0x91);
        sendCommand(0x90);
        sendData(0x00);
        sendData(width - 1);
        sendData(0x00);
        sendData(0x00);
        sendData((height - 1) >> 8);
        sendData((height - 1) & 0xFF);
        sendData(0x01);
    }

    void writeImage(uint8_t command)
    {
        configureFullWindow();
        sendCommand(command);
        sendData(buffer, bufferSize);
        sendCommand(0x92);
    }

    void powerOn()
    {
        sendCommand(0x04);
        wait();
    }

    void powerOff()
    {
        sendCommand(0x02);
        wait();
    }

    void wait(uint32_t timeout = 1000)
    {
        uint32_t startedAt = millis();
        while (digitalRead(pinBusy) == LOW) {
            if (millis() - startedAt > timeout) {
                failed = true;
                return;
            }
            yield();
        }
    }

    void sendCommand(uint8_t command)
    {
        if (failed)
            return;

        spiLock->lock();
        spi->beginTransaction(spiSettings);
        digitalWrite(pinDc, LOW);
        digitalWrite(pinCs, LOW);
        spi->transfer(command);
        digitalWrite(pinCs, HIGH);
        digitalWrite(pinDc, HIGH);
        spi->endTransaction();
        spiLock->unlock();
    }

    void sendData(uint8_t data) { sendData(&data, 1); }

    void sendData(const uint8_t *data, uint32_t size)
    {
        if (failed)
            return;

        spiLock->lock();
        spi->beginTransaction(spiSettings);
        digitalWrite(pinDc, HIGH);
        digitalWrite(pinCs, LOW);
        spi->transferBytes(data, nullptr, size);
        digitalWrite(pinCs, HIGH);
        spi->endTransaction();
        spiLock->unlock();
    }

    uint8_t *buffer = nullptr;
    UpdateTypes updateType = UNSPECIFIED;

    uint8_t pinDc = 0xFF;
    uint8_t pinCs = 0xFF;
    uint8_t pinBusy = 0xFF;
    uint8_t pinRst = 0xFF;
    SPIClass *spi = nullptr;
    SPISettings spiSettings = SPISettings(4000000, MSBFIRST, SPI_MODE0);
};

} // namespace NicheGraphics::Drivers

#endif
