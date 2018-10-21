#include "RCTX.h"

nRFRemote remote;

//////////////////////////////////////
void setup()
{
  remote.init();
  }
///////////////////////////
void loop()
{
  remote.run();
}