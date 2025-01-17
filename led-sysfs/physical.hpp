#pragma once

#include "sysfs.hpp"

#include <sdbusplus/bus.hpp>
#include <sdbusplus/server/object.hpp>
#include <xyz/openbmc_project/Led/Physical/server.hpp>

#include <fstream>
#include <string>

namespace fs = std::filesystem;

namespace phosphor
{
namespace led
{
/** @brief De-assert value */
constexpr unsigned long deasserted = 0;

using PhysicalIfaces = sdbusplus::server::object_t<
    sdbusplus::xyz::openbmc_project::Led::server::Physical>;

/** @class Physical
 *  @brief Responsible for applying actions on a particular physical LED
 */
class Physical : public PhysicalIfaces
{
  public:
    Physical() = delete;
    ~Physical() override = default;
    Physical(const Physical&) = delete;
    Physical& operator=(const Physical&) = delete;
    Physical(Physical&&) = delete;
    Physical& operator=(Physical&&) = delete;

    /** @brief Constructs LED object. Argument 'true' says that we hold off
     *   from sending the signals since we need to do some house keeping and
     *   only when we finish that, we are considered active and can then
     *   broadcast the signal.
     *
     * @param[in] bus       - system dbus handler
     * @param[in] objPath   - The Dbus path that hosts physical LED
     * @param[in] ledPath   - sysfs path where this LED is exported
     * @param[in] color     - led color name
     */

    Physical(sdbusplus::bus_t& bus, const std::string& objPath,
             std::unique_ptr<phosphor::led::SysfsLed> led,
             const std::string& color = "") :
        PhysicalIfaces(bus, objPath.c_str(),
                       PhysicalIfaces::action::defer_emit),
        led(std::move(led))
    {
        // Suppose this is getting launched as part of BMC reboot, then we
        // need to save what the micro-controller currently has.
        setInitialState();

        // Read led color from environment and set it in DBus.
        setLedColor(color);

        // We are now ready.
        emit_object_added();
    }

    /** @brief Overloaded State Property Setter function
     *
     *  @param[in] value   -  One of OFF / ON / BLINK
     *  @return            -  Success or exception thrown
     */
    Action state(Action value) override;

    /** @brief Overridden State Property Getter function
     *
     *  @return  -  One of OFF / ON / BLINK
     */
    Action state() const override;

  private:
    /** @brief Associated LED implementation
     */
    std::unique_ptr<phosphor::led::SysfsLed> led;

    /** @brief The value that will assert the LED */
    unsigned long assert{};

    /** @brief reads sysfs and then setup the parameters accordingly
     *
     *  @return None
     */
    void setInitialState();

    /** @brief Applies the user triggered action on the LED
     *   by writing to sysfs
     *
     *  @param [in] current - Current state of LED
     *  @param [in] request - Requested state
     *
     *  @return None
     */
    void driveLED(Action current, Action request);

    /** @brief Sets the LED to either ON or OFF state
     *
     *  @param [in] action - Requested action. Could be OFF or ON
     *  @return None
     */
    void stableStateOperation(Action action);

    /** @brief Sets the LED to BLINKING
     *
     *  @return None
     */
    void blinkOperation();

    /** @brief set led color property in DBus
     *
     *  @param[in] color - led color name
     */
    void setLedColor(const std::string& color);
};

} // namespace led
} // namespace phosphor
