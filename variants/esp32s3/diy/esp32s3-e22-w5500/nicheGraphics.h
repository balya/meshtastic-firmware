#pragma once

#include "configuration.h"

#ifdef MESHTASTIC_INCLUDE_NICHE_GRAPHICS

#include "GDEY037T03.h"
#include "graphics/niche/Drivers/EInk/HINK_E0213A289.h"
#include "graphics/niche/Drivers/EInk/HINK_E042A87.h"
#include "graphics/niche/Drivers/EInk/ZJY128296_029EAAMFGN.h"
#include "graphics/niche/InkHUD/InkHUD.h"

#include "graphics/niche/InkHUD/Applets/User/AllMessage/AllMessageApplet.h"
#include "graphics/niche/InkHUD/Applets/User/DM/DMApplet.h"
#include "graphics/niche/InkHUD/Applets/User/FavoritesMap/FavoritesMapApplet.h"
#include "graphics/niche/InkHUD/Applets/User/Heard/HeardApplet.h"
#include "graphics/niche/InkHUD/Applets/User/Positions/PositionsApplet.h"
#include "graphics/niche/InkHUD/Applets/User/RecentsList/RecentsListApplet.h"
#include "graphics/niche/InkHUD/Applets/User/ThreadedMessage/ThreadedMessageApplet.h"
#include "graphics/niche/Inputs/TwoButton.h"

#if !defined(INKHUD_BUILDCONF_DRIVER) || !defined(INKHUD_BUILDCONF_DISPLAYRESILIENCE) || !defined(INKHUD_BUILDCONF_MAX_TILES)
#error InkHUD display model, resilience, and tile count must be selected by the PlatformIO environment
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

    InkHUD::Applet::fontLarge = FREESANS_12PT_WIN1251;
    InkHUD::Applet::fontMedium = FREESANS_9PT_WIN1251;
    InkHUD::Applet::fontSmall = FREESANS_6PT_WIN1251;

    inkhud->persistence->settings.rotation = driver->height > driver->width ? 1 : 0;
    inkhud->persistence->settings.userTiles.maxCount = INKHUD_BUILDCONF_MAX_TILES;
    inkhud->persistence->settings.userTiles.count = 1;
    inkhud->persistence->settings.optionalFeatures.batteryIcon = true;

    inkhud->addApplet("All Messages", new InkHUD::AllMessageApplet, true, true);
    inkhud->addApplet("DMs", new InkHUD::DMApplet);
    inkhud->addApplet("Channel 0", new InkHUD::ThreadedMessageApplet(0));
    inkhud->addApplet("Channel 1", new InkHUD::ThreadedMessageApplet(1));
    inkhud->addApplet("Positions", new InkHUD::PositionsApplet, true);
    inkhud->addApplet("Favorites Map", new InkHUD::FavoritesMapApplet);
    inkhud->addApplet("Recents List", new InkHUD::RecentsListApplet);
    inkhud->addApplet("Heard", new InkHUD::HeardApplet, true, false, 0);

    inkhud->begin();

    Inputs::TwoButton *buttons = Inputs::TwoButton::getInstance();
    buttons->setWiring(0, Inputs::TwoButton::getUserButtonPin(), true);
    buttons->setHandlerShortPress(0, [inkhud]() { inkhud->shortpress(); });
    buttons->setHandlerLongPress(0, [inkhud]() { inkhud->longpress(); });
    buttons->start();
}

#endif
