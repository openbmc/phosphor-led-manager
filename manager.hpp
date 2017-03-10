#pragma once

#include <map>
#include <set>
#include <sdbusplus/bus.hpp>
#include "ledlayout.hpp"
namespace phosphor
{
namespace led
{

/** @brief Physical LED dbus constructs */
constexpr auto PHY_LED_PATH = "/xyz/openbmc_project/led/physical/";
constexpr auto PHY_LED_IFACE = "xyz.openbmc_project.Led.Physical";
constexpr auto DBUS_PROPERTY_IFACE = "org.freedesktop.DBus.Properties";

/** @class Manager
 *  @brief Manages group of LEDs and applies action on the elements of group
 */
class Manager
{
    public:
        /** @brief Only need the default Manager */
        Manager() = delete;
        ~Manager() = default;
        Manager(const Manager&) = delete;
        Manager& operator=(const Manager&) = delete;
        Manager(Manager&&) = delete;
        Manager& operator=(Manager&&) = delete;

        /** @brief For finding intersection */
        static bool ledComp(const phosphor::led::Layout::LedAction& left,
                            const phosphor::led::Layout::LedAction& right)
        {
            return left.name < right.name;
        }

        using group = std::set<phosphor::led::Layout::LedAction>;
        using LedLayout = std::map<std::string, group>;

        /** @brief static global map constructed at compile time */
        const LedLayout& ledMap;

        /** @brief Refer the user supplied LED layout and sdbusplus handler
         *
         *  @param [in] bus       - sdbusplus handler
         *  @param [in] LedLayout - LEDs group layout
         */
        Manager(sdbusplus::bus::bus& bus,
                const LedLayout& ledLayout)
                : ledMap(ledLayout),
                  bus(bus)
        {
            // Nothing here
        }

        /** @brief Given a group name, applies the action on the group
         *
         *  @param[in]  path          -  dbus path of group
         *  @param[in]  assert        -  Could be true or false
         *  @param[in]  ledsAssert    -  LEDs that are to be asserted newly
         *  @param[in]  ledsDeAssert  -  LEDs that are to be Deasserted
         *  @param[in]  ledsUpdate    -  LEDs that need a transition between
         *                               different types of asserted states.
         *
         *  @return                   -  Success or exception thrown
         */
        bool setGroupState(const std::string& path, bool assert,
                           group& ledsAssert, group& ledsDeAssert,
                           group& ledsUpdate);

        /** @brief Finds the set of LEDs to operate on and executes action
         *
         *  @param[in]  ledsAssert    -  LEDs that are to be asserted newly
         *  @param[in]  ledsDeAssert  -  LEDs that are to be Deasserted
         *  @param[in]  ledsUpdate    -  LEDs that need a transition between
         *                               different types of asserted states.
         *
         *  @return: None
         */
        void driveLEDs(group& ledsAssert, group& ledsDeAssert,
                       group& ledsUpdate);

    private:
        /** @brief sdbusplus handler */
        sdbusplus::bus::bus& bus;

        /** @brief Pointers to groups that are in asserted state */
        std::set<const group*> assertedGroups;

        /** @brief Contains the LEDs that are in asserted state */
        group currentState;

        /** @brief Returns action string based on enum
         *
         *  @param[in]  action - Action enum
         *
         *  @return string equivalent of the passed in enumeration
         */
        static std::string getPhysicalAction(Layout::Action action);

        /** @brief Chooses appropriate action to be triggered on physical LED
         *  and calls into function that applies the actual action.
         *
         *  @param[in]  objPath   -  dbus object path
         *  @param[in]  action    -  Intended action to be triggered
         *  @param[in]  dutyOn    -  Duty Cycle ON percentage
         */
        void drivePhysicalLED(const std::string& objPath,
                              Layout::Action action,
                              uint8_t dutyOn);

        /** @brief Makes a dbus call to a passed in service name.
         *  This is now the physical LED controller
         *
         *  @param[in]  service   -  dbus service name
         *  @param[in]  objPath   -  dbus object path
         *  @param[in]  property  -  property to be written to
         *  @param[in]  value     -  Value to write
         */
        template <typename T>
        void drivePhysicalLED(const std::string& service,
                              const std::string& objPath,
                              const std::string& property,
                              const T& value)
        {
            sdbusplus::message::variant<T> data = value;

            auto method = bus.new_method_call(service.c_str(), objPath.c_str(),
                                              DBUS_PROPERTY_IFACE, "Set");
            method.append(PHY_LED_IFACE);
            method.append(property);
            method.append(data);

            // There will be exceptions going forward and hence don't need a
            // response
            bus.call_noreply(method);
            return;
        }

        /** @brief Finds the service name given a dbus object path and interface
         *
         *  @param[in]  objPath    -  dbus object path
         *  @param[in]  interface  -  dbus interface
         *
         *  @return: Service name or none
         */
        std::string getServiceName(const std::string& objPath,
                                   const std::string& interface) const;
};

} // namespace led
} // namespace phosphor
