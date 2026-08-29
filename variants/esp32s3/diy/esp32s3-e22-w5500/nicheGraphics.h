#pragma once

#include "configuration.h"

#ifdef MESHTASTIC_INCLUDE_NICHE_GRAPHICS

#include "GDEY037T03.h"
#include "graphics/niche/Drivers/EInk/HINK_E0213A289.h"
#include "graphics/niche/Drivers/EInk/HINK_E042A87.h"
#include "graphics/niche/Drivers/EInk/ZJY128296_029EAAMFGN.h"
#include "graphics/niche/InkHUD/InkHUD.h"
#include "graphics/niche/InkHUD/SystemApplet.h"

#include "graphics/niche/InkHUD/Applets/User/AllMessage/AllMessageApplet.h"
#include "graphics/niche/InkHUD/Applets/User/DM/DMApplet.h"
#include "graphics/niche/InkHUD/Applets/User/FavoritesMap/FavoritesMapApplet.h"
#include "graphics/niche/InkHUD/Applets/User/Heard/HeardApplet.h"
#include "graphics/niche/InkHUD/Applets/User/Positions/PositionsApplet.h"
#include "graphics/niche/InkHUD/Applets/User/RecentsList/RecentsListApplet.h"
#include "graphics/niche/InkHUD/Applets/User/ThreadedMessage/ThreadedMessageApplet.h"
#include "graphics/niche/Inputs/TwoButton.h"
#if !MESHTASTIC_EXCLUDE_INPUTBROKER
#include "input/InputBroker.h"
#include "input/RotaryEncoderInterruptBase.h"
#endif

#if !defined(INKHUD_BUILDCONF_DRIVER) || !defined(INKHUD_BUILDCONF_DISPLAYRESILIENCE) || !defined(INKHUD_BUILDCONF_MAX_TILES)
#error InkHUD display model, resilience, and tile count must be selected by the PlatformIO environment
#endif

#if !MESHTASTIC_EXCLUDE_INPUTBROKER
class E22W5500RotaryEncoder : public RotaryEncoderInterruptBase
{
  public:
    E22W5500RotaryEncoder() : RotaryEncoderInterruptBase("rotEnc1") { instance = this; }

    void begin()
    {
        RotaryEncoderInterruptBase::init(E22_W5500_ROTARY_A, E22_W5500_ROTARY_B, E22_W5500_ROTARY_PRESS, INPUT_BROKER_RIGHT,
                                         INPUT_BROKER_LEFT, INPUT_BROKER_SELECT, INPUT_BROKER_SELECT_LONG, handleA, handleB,
                                         handlePress);
    }

  private:
    inline static E22W5500RotaryEncoder *instance = nullptr;

    static void handleA() { instance->intAHandler(); }
    static void handleB() { instance->intBHandler(); }
    static void handlePress() { instance->intPressHandler(); }
};

class E22W5500InkHUDRotaryBridge : public Observer<const InputEvent *>
{
  public:
    void shortPress()
    {
        NicheGraphics::InkHUD::InkHUD *inkhud = NicheGraphics::InkHUD::InkHUD::getInstance();
        updateKeyboardState(inkhud);
        inkhud->shortpress();
        updateKeyboardState(inkhud);
    }

    void longPress()
    {
        NicheGraphics::InkHUD::InkHUD *inkhud = NicheGraphics::InkHUD::InkHUD::getInstance();
        updateKeyboardState(inkhud);
        inkhud->longpress();
        updateKeyboardState(inkhud);
    }

  protected:
    int onNotify(const InputEvent *event) override
    {
        NicheGraphics::InkHUD::InkHUD *inkhud = NicheGraphics::InkHUD::InkHUD::getInstance();
        updateKeyboardState(inkhud);

        switch (event->inputEvent) {
        case INPUT_BROKER_LEFT:
            if (keyboardActive)
                navigateKeyboard(inkhud, false);
            else if (systemAppletHasInput(inkhud))
                inkhud->touchNavUp();
            else
                inkhud->prevApplet();
            break;
        case INPUT_BROKER_RIGHT:
            if (keyboardActive)
                navigateKeyboard(inkhud, true);
            else if (systemAppletHasInput(inkhud))
                inkhud->touchNavDown();
            else
                inkhud->nextApplet();
            break;
        case INPUT_BROKER_SELECT:
            shortPress();
            break;
        case INPUT_BROKER_SELECT_LONG:
            longPress();
            break;
        default:
            break;
        }

        return 0;
    }

  private:
    static constexpr uint8_t KEYBOARD_ROWS = 4;
    static constexpr uint8_t KEYBOARD_COLUMNS = 11;

    bool keyboardActive = false;
    uint8_t keyboardRow = 0;
    uint8_t keyboardColumn = 0;

    void updateKeyboardState(NicheGraphics::InkHUD::InkHUD *inkhud)
    {
        bool active = inkhud->getSystemApplet("Keyboard")->handleInput;
        if (active && !keyboardActive) {
            keyboardRow = 0;
            keyboardColumn = 0;
        }
        keyboardActive = active;
    }

    void navigateKeyboard(NicheGraphics::InkHUD::InkHUD *inkhud, bool forward)
    {
        NicheGraphics::InkHUD::SystemApplet *keyboard = inkhud->getSystemApplet("Keyboard");

        if (forward) {
            if (keyboardColumn < KEYBOARD_COLUMNS - 1) {
                inkhud->navRight();
                keyboardColumn++;
            } else {
                keyboard->onNavDown();
                keyboard->onNavRight();
                keyboardRow = (keyboardRow + 1) % KEYBOARD_ROWS;
                keyboardColumn = 0;
            }
        } else if (keyboardColumn > 0) {
            inkhud->navLeft();
            keyboardColumn--;
        } else {
            keyboard->onNavUp();
            keyboard->onNavLeft();
            keyboardRow = (keyboardRow + KEYBOARD_ROWS - 1) % KEYBOARD_ROWS;
            keyboardColumn = KEYBOARD_COLUMNS - 1;
        }
    }

    static bool systemAppletHasInput(const NicheGraphics::InkHUD::InkHUD *inkhud)
    {
        for (const NicheGraphics::InkHUD::SystemApplet *applet : inkhud->systemApplets) {
            if (applet->handleInput)
                return true;
        }
        return false;
    }
};
#endif

void setupNicheGraphics()
{
    using namespace NicheGraphics;

    // Keep both devices deselected before sharing the LoRa SPI bus.
    pinMode(LORA_CS, OUTPUT);
    digitalWrite(LORA_CS, HIGH);
    pinMode(PIN_EINK_CS, OUTPUT);
    digitalWrite(PIN_EINK_CS, HIGH);

    Drivers::EInk *driver = new Drivers::INKHUD_BUILDCONF_DRIVER;
    driver->begin(&SPI, PIN_EINK_DC, PIN_EINK_CS, PIN_EINK_BUSY, PIN_EINK_RES);

    InkHUD::InkHUD *inkhud = InkHUD::InkHUD::getInstance();
    inkhud->setDriver(driver);
    inkhud->setDisplayResilience(INKHUD_BUILDCONF_DISPLAYRESILIENCE);
    inkhud->twoWayRocker = false;

    inkhud->persistence->loadSettings();
    bool saveInputSettings = !inkhud->persistence->settings.joystick.enabled || !inkhud->persistence->settings.joystick.aligned ||
                             inkhud->persistence->settings.optionalMenuItems.nextTile;

    InkHUD::Applet::fontLarge = FREESANS_12PT_WIN1251;
    InkHUD::Applet::fontMedium = FREESANS_9PT_WIN1251;
    InkHUD::Applet::fontSmall = FREESANS_6PT_WIN1251;

    inkhud->persistence->settings.rotation = driver->height > driver->width ? 1 : 0;
    inkhud->persistence->settings.userTiles.maxCount = INKHUD_BUILDCONF_MAX_TILES;
    inkhud->persistence->settings.userTiles.count = 1;
    inkhud->persistence->settings.joystick.enabled = true;
    inkhud->persistence->settings.joystick.aligned = true;
    inkhud->persistence->settings.optionalMenuItems.nextTile = false;
    inkhud->persistence->settings.optionalFeatures.batteryIcon = true;
    inkhud->persistence->settings.joystick.alignment = (4 - inkhud->persistence->settings.rotation) % 4;
    if (saveInputSettings)
        inkhud->persistence->saveSettings();

    inkhud->addApplet("All Messages", new InkHUD::AllMessageApplet, true, true);
    inkhud->addApplet("DMs", new InkHUD::DMApplet);
    inkhud->addApplet("Channel 0", new InkHUD::ThreadedMessageApplet(0));
    inkhud->addApplet("Channel 1", new InkHUD::ThreadedMessageApplet(1));
    inkhud->addApplet("Positions", new InkHUD::PositionsApplet, true);
    inkhud->addApplet("Favorites Map", new InkHUD::FavoritesMapApplet);
    inkhud->addApplet("Recents List", new InkHUD::RecentsListApplet);
    inkhud->addApplet("Heard", new InkHUD::HeardApplet, true, false, 0);

    inkhud->begin();

#if !MESHTASTIC_EXCLUDE_INPUTBROKER
    // Persisted settings can override hardware input defaults during begin().
    inkhud->persistence->settings.joystick.enabled = true;
    inkhud->persistence->settings.joystick.aligned = true;
    inkhud->persistence->settings.optionalMenuItems.nextTile = false;
    inkhud->persistence->settings.joystick.alignment = (4 - inkhud->persistence->settings.rotation) % 4;

    static E22W5500RotaryEncoder rotaryEncoder;
    static E22W5500InkHUDRotaryBridge rotaryBridge;
    rotaryEncoder.begin();
    rotaryBridge.observe(&rotaryEncoder);
#endif

    Inputs::TwoButton *buttons = Inputs::TwoButton::getInstance();
    buttons->setWiring(0, Inputs::TwoButton::getUserButtonPin(), true);
#if !MESHTASTIC_EXCLUDE_INPUTBROKER
    buttons->setHandlerShortPress(0, []() { rotaryBridge.shortPress(); });
    buttons->setHandlerLongPress(0, []() { rotaryBridge.longPress(); });
#else
    buttons->setHandlerShortPress(0, [inkhud]() { inkhud->shortpress(); });
    buttons->setHandlerLongPress(0, [inkhud]() { inkhud->longpress(); });
#endif
    buttons->start();
}

#endif
