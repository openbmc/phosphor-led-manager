#include "led-manager.hpp"

int main(void)
{
    phosphor::Led::Manager ledMgr(BUS, OBJPATH, INTERFACE);
    ledMgr.run();
    return 0;
}
