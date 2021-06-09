#include "config.h"

#include "multi-purpose-status.hpp"

#ifdef MONITOR_OPERATIONAL_STATUS
#include "operational-status-monitor.hpp"
#else
#include "fru-fault-monitor.hpp"
#endif

int main(void)
{
    /** @brief Dbus constructs used by Fault Monitor */
    sdbusplus::bus::bus bus = sdbusplus::bus::new_default();

    phosphor::led::multi::purpose::status::Status Status;

#ifdef MONITOR_OPERATIONAL_STATUS
    phosphor::led::Operational::status::monitor::Monitor monitor(bus);
#else
    phosphor::led::fru::fault::monitor::Add monitor(bus);
#endif

    /** @brief Wait for client requests */
    while (true)
    {
        /** @brief process dbus calls / signals discarding unhandled */
        bus.process_discard();
        bus.wait();
    }
    return 0;
}
