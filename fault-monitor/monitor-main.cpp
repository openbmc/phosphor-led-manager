#include "config.h"

#ifdef MONITOR_OPER_STATUS
#include "oper-status-monitor.hpp"
#else
#include "fru-fault-monitor.hpp"
#endif

int main(void)
{
    /** @brief Dbus constructs used by Fault Monitor */
    sdbusplus::bus::bus bus = sdbusplus::bus::new_default();

#ifdef MONITOR_OPER_STATUS
    phosphor::led::oper::status::monitor::Monitor operStatus(bus);
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
