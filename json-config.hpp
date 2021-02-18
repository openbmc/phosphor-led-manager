#include "utils.hpp"

#include <phosphor-logging/log.hpp>
#include <sdbusplus/exception.hpp>
#include <sdeventplus/event.hpp>

#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

using namespace phosphor::logging;

namespace phosphor
{
namespace led
{

static constexpr auto confFileName = "led-group-config.json";
static constexpr auto confOverridePath = "/etc/phosphor-led-manager";
static constexpr auto confBasePath = "/usr/share/phosphor-led-manager";
static constexpr auto confCompatibleInterface =
    "xyz.openbmc_project.Configuration.IBMCompatibleSystem";
static constexpr auto confCompatibleProperty = "Names";

class JsonConfig
{
  public:
    /**
     * @brief Constructor
     *
     * Looks for the JSON config file.  If it can't find one, then it
     * will watch entity-manager for the IBMCompatibleSystem interface
     * to show up.
     *
     * @param[in] bus       - The D-Bus object
     * @param[in] event     - sd event handler
     */
    JsonConfig(sdbusplus::bus::bus& bus, sdeventplus::Event& event) :
        event(event)
    {
        match = std::make_unique<sdbusplus::server::match::match>(
            bus,
            sdbusplus::bus::match::rules::interfacesAdded() +
                sdbusplus::bus::match::rules::sender(
                    "xyz.openbmc_project.EntityManager"),
            std::bind(&JsonConfig::ifacesAddedCallback, this,
                      std::placeholders::_1));
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
                auto tempConfFile =
                    fs::path{confBasePath} / name / confFileName;
                if (fs::exists(tempConfFile))
                {
                    confFile = tempConfFile;
                    return true;
                }
                return false;
            });
        return it == names.end() ? false : true;
    }

    /**
     * @brief The interfacesAdded callback function that looks for
     *        the IBMCompatibleSystem interface.  If it finds it,
     *        it uses the Names property in the interface to find
     *        the JSON config file to use.
     *
     * @param[in] msg - The D-Bus message contents
     */
    void ifacesAddedCallback(sdbusplus::message::message& msg)
    {
        sdbusplus::message::object_path path;
        std::map<std::string,
                 std::map<std::string, std::variant<std::vector<std::string>>>>
            interfaces;

        msg.read(path, interfaces);

        if (interfaces.find(confCompatibleInterface) == interfaces.end())
        {
            return;
        }

        // Get the "Name" property value of the
        // "xyz.openbmc_project.Configuration.IBMCompatibleSystem" interface
        const auto& properties = interfaces.at(confCompatibleInterface);

        if (properties.find(confCompatibleProperty) == properties.end())
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
    const void getFilePath()
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
            auto objects =
                dBusHandler.getSubTreePaths("/", confCompatibleInterface);
            for (const auto& path : objects)
            {
                try
                {
                    // Retrieve json config compatible relative path locations
                    auto value = dBusHandler.getProperty(
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
                catch (const sdbusplus::exception::SdBusError& e)
                {
                    // Property unavailable on object.
                    log<level::ERR>(
                        "Failed to get Names property",
                        entry("ERROR=%s", e.what()),
                        entry("INTERFACE=%s", confCompatibleInterface),
                        entry("PATH=%s", path.c_str()));

                    confFile.clear();
                }
            }
        }
        catch (const sdbusplus::exception::SdBusError& e)
        {
            log<level::ERR>("Failed to call the SubTreePaths method",
                            entry("ERROR=%s", e.what()),
                            entry("INTERFACE=%s", confCompatibleInterface));
        }
        return;
    }

  private:
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
     *        for the IBMCompatibleSystem interface to show up.
     */
    std::unique_ptr<sdbusplus::server::match::match> match;

    /** DBusHandler class handles the D-Bus operations */
    utils::DBusHandler dBusHandler;
};
} // namespace led
} // namespace phosphor
