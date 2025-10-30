#include "sdeventplus/clock.hpp"
#include "sdeventplus/source/time.hpp"
#include "utils.hpp"

#include <phosphor-logging/lg2.hpp>
#include <sdbusplus/exception.hpp>
#include <sdeventplus/event.hpp>

#include <chrono>
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

namespace phosphor
{
namespace led
{

static constexpr auto confFileName = "led-group-config.json";
static constexpr auto confOverridePath = "/etc/phosphor-led-manager";
static constexpr auto confBasePath = "/usr/share/phosphor-led-manager";
static constexpr auto confCompatibleInterface =
    "xyz.openbmc_project.Inventory.Decorator.Compatible";
static constexpr auto confCompatibleProperty = "Names";

class JsonConfig
{
  public:
    /**
     * @brief Constructor
     *
     * Looks for the JSON config file.  If it can't find one, then it
     * will watch entity-manager for the
     * xyz.openbmc_project.Inventory.Decorator.Compatible interface to show up.
     *
     * @param[in] bus       - The D-Bus object
     * @param[in] event     - sd event handler
     */
    JsonConfig(sdbusplus::bus_t& bus, sdeventplus::Event& event) : event(event)
    {
        match = std::make_unique<sdbusplus::bus::match_t>(
            bus,
            sdbusplus::bus::match::rules::interfacesAdded() +
                sdbusplus::bus::match::rules::sender(
                    "xyz.openbmc_project.EntityManager"),
            [this](sdbusplus::message_t& m) { ifacesAddedCallback(m); });
        getFilePath();

        if (!confFile.empty())
        {
            match.reset();
        }
    }

    /**
     * @brief Get the configuration file
     *
     * @return filesystem path
     */
    inline const fs::path& getConfFile() const
    {
        return confFile;
    }

  private:
    /** @brief Check the file path exists
     *
     *  @param[in]  names   -  Vector of the confCompatible Property
     *
     *  @return             -  True or False
     */
    bool filePathExists(const std::vector<std::string>& names)
    {
        auto it =
            std::find_if(names.begin(), names.end(), [this](const auto& name) {
                auto configFileName = name + ".json";
                auto configFilePath = fs::path{confBasePath} / configFileName;
                if (fs::exists(configFilePath))
                {
                    confFile = configFilePath;
                    return true;
                }
                return false;
            });
        return it != names.end();
    }

    /**
     * @brief The interfacesAdded callback function that looks for
     *        the xyz.openbmc_project.Inventory.Decorator.Compatible interface.
     * If it finds it, it uses the Names property in the interface to find the
     * JSON config file to use.
     *
     * @param[in] msg - The D-Bus message contents
     */
    void ifacesAddedCallback(sdbusplus::message_t& msg)
    {
        sdbusplus::message::object_path path;
        std::unordered_map<
            std::string,
            std::unordered_map<std::string,
                               std::variant<std::vector<std::string>>>>
            interfaces;

        msg.read(path, interfaces);

        if (!interfaces.contains(confCompatibleInterface))
        {
            return;
        }

        // Get the "Name" property value of the
        // "xyz.openbmc_project.Inventory.Decorator.Compatible" interface
        const auto& properties = interfaces.at(confCompatibleInterface);

        if (!properties.contains(confCompatibleProperty))
        {
            return;
        }
        auto names = std::get<std::vector<std::string>>(
            properties.at(confCompatibleProperty));

        if (filePathExists(names))
        {
            match.reset();

            // This results in event.loop() exiting in getSystemLedMap
            event.exit(0);
        }
    }

    /**
     * Get the json configuration file. The first location found to contain the
     * json config file from the following locations in order.
     * confOverridePath: /etc/phosphor-led-manager/led-group-config.json
     * confBasePath: /usr/shard/phosphor-led-manager/led-group-config.json
     * the name property of the confCompatibleInterface:
     * /usr/shard/phosphor-led-manager/${Name}/led-group-config.json
     *
     * @brief Get the configuration file to be used
     *
     * @return
     */
    void getFilePath()
    {
        // Check override location
        confFile = fs::path{confOverridePath} / confFileName;
        if (fs::exists(confFile))
        {
            return;
        }

        // If the default file is there, use it
        confFile = fs::path{confBasePath} / confFileName;
        if (fs::exists(confFile))
        {
            return;
        }
        confFile.clear();

        try
        {
            // Get all objects implementing the compatible interface
            auto objects = phosphor::led::utils::DBusHandler::getSubTreePaths(
                "/", confCompatibleInterface);
            for (const auto& path : objects)
            {
                try
                {
                    // Retrieve json config compatible relative path locations
                    auto value = phosphor::led::utils::DBusHandler::getProperty(
                        path, confCompatibleInterface, confCompatibleProperty);

                    auto confCompatValues =
                        std::get<std::vector<std::string>>(value);

                    // Look for a config file at each name relative to the base
                    // path and use the first one found
                    if (filePathExists(confCompatValues))
                    {
                        // Use the first config file found at a listed location
                        break;
                    }
                    confFile.clear();
                }
                catch (const sdbusplus::exception_t& e)
                {
                    // Property unavailable on object.
                    lg2::error(
                        "Failed to get Names property, ERROR = {ERROR}, INTERFACES = {INTERFACES}, PATH = {PATH}",
                        "ERROR", e, "INTERFACE", confCompatibleInterface,
                        "PATH", path);

                    confFile.clear();
                }
            }
        }
        catch (const sdbusplus::exception_t& e)
        {
            lg2::error(
                "Failed to call the SubTreePaths method, ERROR = {ERROR}, INTERFACE = {INTERFACE}",
                "ERROR", e, "INTERFACE", confCompatibleInterface);
        }
        return;
    }

    /**
     * @brief sd event handler.
     */
    sdeventplus::Event& event;

    /**
     * @brief The JSON config file
     */
    fs::path confFile;

    /**
     * @brief The interfacesAdded match that is used to wait
     *        for the xyz.openbmc_project.Inventory.Decorator.Compatible
     * interface to show up.
     */
    std::unique_ptr<sdbusplus::bus::match_t> match;
};

/** Blocking call to find the JSON Config from DBus. */
auto getJsonConfig()
{
    // Get a new Dbus
    auto bus = sdbusplus::bus::new_bus();

    // Get a new event loop
    auto event = sdeventplus::Event::get_new();

    // Attach the bus to sd_event to service user requests
    bus.attach_event(event.get(), SD_EVENT_PRIORITY_IMPORTANT);
    phosphor::led::JsonConfig jsonConfig(bus, event);

    // The event loop will be terminated from inside of a function in JsonConfig
    // after finding the configuration file
    if (jsonConfig.getConfFile().empty())
    {
        // Cannot wait forever here, otherwise systemd will timeout waiting for
        // the well-known name to be claimed.
        static constexpr auto accuracy = std::chrono::seconds(0);
        static constexpr auto waitDurationSeconds = 30;
        static constexpr auto waitDuration =
            std::chrono::seconds(waitDurationSeconds);
        auto expireTime =
            sdeventplus::Clock<sdeventplus::ClockId::Monotonic>(event).now() +
            waitDuration;
        sdeventplus::source::Time<sdeventplus::ClockId::Monotonic> source(
            event, expireTime, accuracy, [&event](auto&, auto) {
                lg2::info("No configuration after {SECONDS} seconds", "SECONDS",
                          waitDurationSeconds);
                event.exit(0);
            });
        event.loop();
    }

    // Detach the bus from its sd_event event loop object
    bus.detach_event();

    return jsonConfig.getConfFile();
}

} // namespace led
} // namespace phosphor
