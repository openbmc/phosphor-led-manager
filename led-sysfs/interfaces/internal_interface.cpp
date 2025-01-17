/**
 * Copyright © 2020 IBM Corporation
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

#include "internal_interface.hpp"

#include <sdbusplus/message.hpp>

#include <algorithm>
#include <iterator>
#include <numeric>

namespace phosphor
{
namespace led
{
namespace sysfs
{
namespace interface
{

InternalInterface::InternalInterface(sdbusplus::bus_t& bus, const char* path) :
    bus(bus), serverInterface(bus, path, internalInterface, vtable.data(), this)
{}

std::string InternalInterface::getDbusName(const LedDescr& ledDescr)
{
    std::vector<std::string> words;
    if (ledDescr.devicename.has_value())
    {
        words.emplace_back(ledDescr.devicename.value());
    }
    if (ledDescr.function.has_value())
    {
        words.emplace_back(ledDescr.function.value());
    }

    if (ledDescr.color.has_value())
    {
        words.emplace_back(ledDescr.color.value());
    }

    std::string s =
        std::accumulate(std::next(words.begin()), words.end(), words[0],
                        [](std::string a, const std::string& b) {
                            return std::move(a) + "_" + b;
                        });

    // we assume the string to be a correct dbus name besides
    // this detail
    std::replace(s.begin(), s.end(), '-', '_');

    return s;
}

void InternalInterface::createLEDPath(const std::string& ledName)
{
    std::string name;
    std::string path = devParent + ledName;

    if (!std::filesystem::exists(fs::path(path)))
    {
        lg2::error("No such directory {PATH}", "PATH", path);
        return;
    }

    auto sled = std::make_unique<phosphor::led::SysfsLed>(fs::path(path));

    // Convert LED name in sysfs into DBus name
    const LedDescr ledDescr = sled->getLedDescr();

    name = getDbusName(ledDescr);

    lg2::debug("LED {NAME} receives dbus name {DBUSNAME}", "NAME", ledName,
               "DBUSNAME", name);

    // Unique path name representing a single LED.
    std::string objPath = std::string(physParent) + "/" + name;

    if (leds.contains(objPath))
    {
        return;
    }

    leds.emplace(objPath, std::make_unique<phosphor::led::Physical>(
                              bus, objPath, std::move(sled),
                              ledDescr.color.value_or("")));
}

void InternalInterface::addLED(const std::string& name)
{
    createLEDPath(name);
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
void InternalInterface::removeLED(const std::string& name)
{
    lg2::debug("RemoveLED is not configured {NAME}", "NAME", name);
}

int InternalInterface::addLedConfigure(sd_bus_message* msg, void* context,
                                       sd_bus_error* error)
{
    if (msg == nullptr && context == nullptr)
    {
        lg2::error("Unable to configure addLed");
        return -EINVAL;
    }

    try
    {
        auto message = sdbusplus::message_t(msg);
        auto ledName = message.unpack<std::string>();

        auto* self = static_cast<InternalInterface*>(context);
        self->addLED(ledName);

        auto reply = message.new_method_return();
        reply.method_return();
    }
    catch (const sdbusplus::exception_t& e)
    {
        return sd_bus_error_set(error, e.name(), e.description());
    }

    return 1;
}

int InternalInterface::removeLedConfigure(sd_bus_message* msg, void* context,
                                          sd_bus_error* error)
{
    if (msg == nullptr && context == nullptr)
    {
        lg2::error("Unable to configure removeLed");
        return -EINVAL;
    }

    try
    {
        auto message = sdbusplus::message_t(msg);
        auto ledName = message.unpack<std::string>();

        auto* self = static_cast<InternalInterface*>(context);
        self->removeLED(ledName);

        auto reply = message.new_method_return();
        reply.method_return();
    }
    catch (const sdbusplus::exception_t& e)
    {
        return sd_bus_error_set(error, e.name(), e.description());
    }

    return 1;
}

const std::array<sdbusplus::vtable::vtable_t, 4> InternalInterface::vtable = {
    sdbusplus::vtable::start(),
    // AddLed method takes a string parameter and returns void
    sdbusplus::vtable::method("AddLED", "s", "", addLedConfigure),
    // RemoveLed method takes a string parameter and returns void
    sdbusplus::vtable::method("RemoveLED", "s", "", removeLedConfigure),
    sdbusplus::vtable::end()};

} // namespace interface
} // namespace sysfs
} // namespace led
} // namespace phosphor
