#include "led-manager.hpp"
#include "config.h"

int main(void)
{
    phosphor::led::Manager ledMgr(BUSNAME, OBJPATH, INTERFACE);
    ledMgr.run();
    return 0;
}
