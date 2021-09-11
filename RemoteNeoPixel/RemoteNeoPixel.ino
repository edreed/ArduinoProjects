#include <Adafruit_NeoPixel_ZeroDMA.h>
#include <Adafruit_DotStar.h>
#define DECODE_NEC 1
#include <IRremote.h>

#include "Animation.h"

#define IR_RECEIVE_PIN    2
#define NEOPIXEL_PIN      4
#define DOTSTAR_DATA_PIN  7
#define DOTSTAR_CLOCK_PIN 8

#define COMMAND_POWER_KEY 0x45
#define COMMAND_UP_KEY    0x09
#define COMMAND_DOWN_KEY  0x07
#define COMMAND_0_KEY     0x16
#define COMMAND_1_KEY     0x0C
#define COMMAND_2_KEY     0x18
#define COMMAND_3_KEY     0x5E
#define COMMAND_4_KEY     0x08
#define COMMAND_5_KEY     0x1C
#define COMMAND_6_KEY     0x5A
#define COMMAND_7_KEY     0x42
#define COMMAND_8_KEY     0x52
#define COMMAND_9_KEY     0x4A

#define NUM_PIXELS 12

#define PIXEL_ANIMATION_STEP_TIME_MILLIS 50

#define BLACK_COLOR 0x000000
#define RED_COLOR   0xFF0000
#define GREEN_COLOR 0x00FF00
#define BLUE_COLOR  0x0000FF

Adafruit_NeoPixel_ZeroDMA g_pixels(NUM_PIXELS, NEOPIXEL_PIN, NEO_GRB);
Adafruit_DotStar g_status(1, DOTSTAR_DATA_PIN, DOTSTAR_CLOCK_PIN, DOTSTAR_BGR);

anime::Configuration g_configuration;

auto g_solidBlack     = anime::make_sequence<anime::SolidColor>(&g_pixels, &g_configuration, BLACK_COLOR);
auto g_rainbowCycle   = anime::make_sequence<anime::RainbowCycle>(&g_pixels, &g_configuration);
auto g_colorWipeRed   = anime::make_sequence<anime::ColorWipe>(&g_pixels, &g_configuration, RED_COLOR);
auto g_colorWipeGreen = anime::make_sequence<anime::ColorWipe>(&g_pixels, &g_configuration, GREEN_COLOR);
auto g_colorWipeBlue  = anime::make_sequence<anime::ColorWipe>(&g_pixels, &g_configuration, BLUE_COLOR);
auto g_pulseRed       = anime::make_sequence<anime::Pulse>(&g_pixels, &g_configuration, RED_COLOR);
auto g_pulseGreen     = anime::make_sequence<anime::Pulse>(&g_pixels, &g_configuration, GREEN_COLOR);
auto g_pulseBlue      = anime::make_sequence<anime::Pulse>(&g_pixels, &g_configuration, BLUE_COLOR);
auto g_cometRed       = anime::make_sequence<anime::Comet>(&g_pixels, &g_configuration, RED_COLOR);
auto g_cometGreen     = anime::make_sequence<anime::Comet>(&g_pixels, &g_configuration, GREEN_COLOR);
auto g_cometBlue      = anime::make_sequence<anime::Comet>(&g_pixels, &g_configuration, BLUE_COLOR);

anime::Sequence<Adafruit_NeoPixel_ZeroDMA>* g_currentSequence = &g_solidBlack;

bool  g_powerOn = true;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);

  g_status.setBrightness(50);
  g_pixels.begin();

  setPowerOn(false);
}

void loop() {
  if (IrReceiver.decode()) {

    switch (IrReceiver.decodedIRData.command) {
      case COMMAND_POWER_KEY:
        togglePower();
        break;
        
      case COMMAND_UP_KEY:
        g_configuration.adjustBrightness(1);
        break;
        
      case COMMAND_DOWN_KEY:
        g_configuration.adjustBrightness(-1);
        break;
        
      case COMMAND_0_KEY:
        startSequence(&g_rainbowCycle);
        break;

      case COMMAND_1_KEY:
        startSequence(&g_colorWipeRed);
        break;

      case COMMAND_2_KEY:
        startSequence(&g_colorWipeGreen);
        break;

      case COMMAND_3_KEY:
        startSequence(&g_colorWipeBlue);
        break;

      case COMMAND_4_KEY:
        startSequence(&g_pulseRed);
        break;

      case COMMAND_5_KEY:
        startSequence(&g_pulseGreen);
        break;

      case COMMAND_6_KEY:
        startSequence(&g_pulseBlue);
        break;
      
      case COMMAND_7_KEY:
        startSequence(&g_cometRed);
        break;
      
      case COMMAND_8_KEY:
        startSequence(&g_cometGreen);
        break;
      
      case COMMAND_9_KEY:
        startSequence(&g_cometBlue);
        break;
    }
    
    IrReceiver.resume();
  }
  
  g_currentSequence->animate();
}

void togglePower() {
  setPowerOn(!g_powerOn);
}

void setPowerOn(bool powerOn) {
  if (g_powerOn != powerOn) {

    if (powerOn) {
      g_status.fill(GREEN_COLOR);
      g_status.show();
      g_currentSequence->start();
    }
    else {
      g_status.fill(RED_COLOR);
      g_status.show();
      g_currentSequence->stop();
      g_solidBlack.start();
    }

    g_powerOn = powerOn;
  }
}

void startSequence(anime::Sequence<Adafruit_NeoPixel_ZeroDMA>* sequence) {
  g_currentSequence->stop();
  g_currentSequence = sequence;
  sequence->start();
}
