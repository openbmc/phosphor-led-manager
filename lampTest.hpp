#pragma once

#include "group.hpp"
#include "manager.hpp"

#include <com/ibm/Led/LampTest/server.hpp>
#include <sdeventplus/event.hpp>
#include <sdeventplus/utility/timer.hpp>
#include <xyz/openbmc_project/Common/Progress/server.hpp>

#include <chrono>
#include <vector>

namespace phosphor
{
namespace led
{

using LampTestInherit = sdbusplus::server::object::object<
    sdbusplus::xyz::openbmc_project::Common::server::Progress,
    sdbusplus::com::ibm::Led::server::LampTest>;

using namespace std::chrono;

/** @class LampTest
 *  @brief Initiate a Lamp Test request
 */
class LampTest : public LampTestInherit
{
  public:
    LampTest() = delete;
    ~LampTest() = default;
    LampTest(const LampTest&) = delete;
    LampTest& operator=(const LampTest&) = delete;
    LampTest(LampTest&&) = default;
    LampTest& operator=(LampTest&&) = default;

    /** @brief Constructs LED LampTest
     *
     * @param[in] bus     - Handle to system dbus
     * @param[in] path    - lamp test path
     * @param[in] event   - sd event handler
     * @param[in] manager - Reference to Manager
     */
    LampTest(sdbusplus::bus::bus& bus, const std::string& path,
             const sdeventplus::Event& event, Manager& manager) :
        LampTestInherit(bus, path.c_str()),
        bus(bus), path(path), event(event), manager(manager),
        timer(event, std::bind(&LampTest::lampTestTimeout, this))
    {
        status(OperationStatus::InProgress);
        completedTime(0);

        // Emit deferred signal.
        emit_object_added();
    }

  private:
    /** @brief sdbusplus handler */
    sdbusplus::bus::bus& bus;

    /** @brief Path of the lamp test */
    std::string path;

    /** @brief sdbusplus event */
    const sdeventplus::Event& event;

    /** @brief Reference to Manager object */
    Manager& manager;

    /** DBusHandler class handles the D-Bus operations */
    DBusHandler dBusHandler;

    /** @brief Timer used for host transition with seconds */
    sdeventplus::utility::Timer<sdeventplus::ClockId::RealTime> timer;

    /** @brief Indicate the LampTest request
     *
     *  @return
     */
    void initiate() override;

    /** @brief Get current time
     *
     * @return Microseconds since UTC
     */
    microseconds getTime() const;

    /** @brief  Call back the restore function when timeout. */
    void lampTestTimeout();

    /** @brief Set all the physical action to On for lamp test
     *
     *  @param[in]  action    -  Intended action to be triggered
     *  @return               -  Success or Failed
     */
    bool updatePhysicalAction(Layout::Action action);
};

} // namespace led
} // namespace phosphor
