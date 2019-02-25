#define S2_IMPL
#define DEBUG 3
#define PLATFORM_ARDUINO

#include "utils.h"
#include "umn.h"
#include "naivepoller.h"
#include "arduinoserialstream.h"

using namespace umn;

Poller* p; // = new NaivePoller();
UMN * u;

void setup() {
  // put your setup code here, to run once:
  p = new NaivePoller();
  u = new UMN( p );
  umn::Stream * udps = new ArduinoSerialStream();
  u->addStream( udps );
}

void loop() {
  // put your main code here, to run repeatedly:
  UMN_DEBUG_PRINT("coucou\n");
  u->tick();
}
