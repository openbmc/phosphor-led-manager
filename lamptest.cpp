#include "config.h"

#include "lamptest.hpp"

namespace phosphor
{
namespace led
{

void LampTest::stop()
{
    timer.setEnabled(false);
}

void LampTest::start()
{
    // restart lamp test, it contains initiate or reset the timer.
    timer.restart(std::chrono::seconds(LAMP_TEST_TIMEOUT_IN_SECS));
}

void LampTest::timeOutHandler()
{
    // set the Asserted property of lamp test to false
}

void LampTest::requestHandler(bool value)
{
    if (value)
    {
        start();
    }
    else
    {
        stop();
    }
}

} // namespace led
} // namespace phosphor
