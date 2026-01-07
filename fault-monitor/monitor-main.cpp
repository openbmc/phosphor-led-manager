#include "config.h"

#include "fru-fault-monitor.hpp"
#include "operational-status-monitor.hpp"

static void loop(sdbusplus::bus_t& bus)
{
    /** @brief Wait for client requests */
    while (true)
    {
        /** @brief process dbus calls / signals discarding unhandled */
        bus.process_discard();
        bus.wait();
    }
}

int main(void)
{
    /** @brief Dbus constructs used by Fault Monitor */
    sdbusplus::bus_t bus = sdbusplus::bus::new_default();

    if constexpr (MONITOR_OPERATIONAL_STATUS)
    {
        phosphor::led::Operational::status::monitor::Monitor monitor(bus);
        loop(bus);
    }
    else
    {
        phosphor::led::fru::fault::monitor::Add monitor(bus);
        loop(bus);
    }
    return 0;
}
