#include "config.h"

#include "fru-fault-monitor.hpp"
#include "operational-status-monitor.hpp"

int main(void)
{
    /** @brief Dbus constructs used by Fault Monitor */
    sdbusplus::bus_t bus = sdbusplus::bus::new_default();

    if constexpr (MONITOR_OPERATIONAL_STATUS)
    {
        phosphor::led::Operational::status::monitor::Monitor monitor(bus);
    }
    else
    {
        phosphor::led::fru::fault::monitor::Add monitor(bus);
    }
    bus.process_loop();
    return 0;
}
