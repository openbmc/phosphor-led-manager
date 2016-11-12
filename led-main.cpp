#include "led-manager.hpp"
#include "config.h"

int main(void)
{
    auto ledMgr = std::make_unique<phosphor::led::Manager>
                  (BUSNAME, OBJPATH, INTERFACE);
    ledMgr->run();
    return 0;
}
