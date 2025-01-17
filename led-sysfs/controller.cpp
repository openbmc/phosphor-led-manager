/**
 * Copyright © 2016 IBM Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "interfaces/internal_interface.hpp"

int main()
{
    // Get a handle to system dbus
    auto bus = sdbusplus::bus::new_default();

    // Add the ObjectManager interface
    sdbusplus::server::manager_t objManager(bus, ledPath);

    // Create an led controller object
    phosphor::led::sysfs::interface::InternalInterface internal(bus, ledPath);

    // Request service bus name
    bus.request_name(busName);

    while (true)
    {
        // Handle dbus message / signals discarding unhandled
        bus.process_discard();
        bus.wait();
    }
    return 0;
}
