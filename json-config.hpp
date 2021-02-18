#include "utils.hpp"

#include <unistd.h>

#include <phosphor-logging/log.hpp>
#include <sdbusplus/exception.hpp>
#include <sdeventplus/event.hpp>

#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

using namespace phosphor::logging;

namespace phosphor::led
{

static constexpr auto confOverridePath = "/etc/phosphor-led-manager";
static constexpr auto confBasePath = "/usr/share/phosphor-led-manager";
static constexpr auto confCompatIntf =
    "xyz.openbmc_project.Configuration.IBMCompatibleSystem";
static constexpr auto confCompatProp = "Names";

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
     * @param[in] bus       - The dbus bus object
     * @param[in] event     - sd event handler
     * @param[in] fileName  - Application's configuration file's name
     */
    JsonConfig(sdbusplus::bus::bus& bus, sdeventplus::Event& event,
               const std::string& fileName) :
        _event(event),
        _fileName(fileName)
    {
        _match = std::make_unique<sdbusplus::server::match::match>(
            bus,
            sdbusplus::bus::match::rules::interfacesAdded() +
                sdbusplus::bus::match::rules::sender(
                    "xyz.openbmc_project.EntityManager"),
            std::bind(&JsonConfig::ifacesAddedCallback, this,
                      std::placeholders::_1));
        getFilePath();

        if (!_confFile.empty())
        {
            _match.reset();
        }
    }

    /**
     * @brief Get the configuration file
     *
     * @return filesystem path
     */
    const fs::path& getConfFile() const
    {
        return _confFile;
    }

  private:
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

        if (interfaces.find(confCompatIntf) == interfaces.end())
        {
            return;
        }

        const auto& properties = interfaces.at(confCompatIntf);
        auto names =
            std::get<std::vector<std::string>>(properties.at(confCompatProp));

        auto it =
            std::find_if(names.begin(), names.end(), [this](auto const& name) {
                auto confFile = fs::path{confBasePath} / name / _fileName;
                if (fs::exists(confFile))
                {
                    _confFile = confFile;
                    return true;
                }
                return false;
            });

        if (it != names.end())
        {
            _match.reset();
            _event.exit(0);
        }
    }

    /**
     * Get the json configuration file. The first location found to contain
     * the json config file for the given fan application is used from the
     * following locations in order.
     * 1.) From the confOverridePath location
     * 2.) From the confBasePath location
     * 3.) From config file found using an entry from a list obtained from an
     * interface's property as a relative path extension on the base path where:
     *     interface = Interface set in confCompatIntf with the property
     *     property = Property set in confCompatProp containing a list of
     *                subdirectories in priority order to find a config
     *
     * @brief Get the configuration file to be used
     *
     * @return
     */
    const void getFilePath()
    {
        // Check override location
        _confFile = fs::path{confOverridePath} / _fileName;
        if (fs::exists(_confFile))
        {
            return;
        }

        // If the default file is there, use it
        _confFile = fs::path{confBasePath} / _fileName;
        if (fs::exists(_confFile))
        {
            return;
        }
        _confFile.clear();

        // Get all objects implementing the compatible interface
        auto objects = dBusHandler.getSubTreePaths("/", confCompatIntf);
        for (auto& path : objects)
        {
            try
            {
                // Retrieve json config compatible relative path locations
                utils::PropertyValue value = dBusHandler.getProperty(
                    path, confCompatIntf, confCompatProp);

                auto confCompatValue =
                    std::get<std::vector<std::string>>(value);
                // Look for a config file at each entry relative to the base
                // path and use the first one found
                auto it =
                    std::find_if(confCompatValue.begin(), confCompatValue.end(),
                                 [this](auto const& entry) {
                                     this->_confFile = fs::path{confBasePath} /
                                                       entry / this->_fileName;
                                     return fs::exists(this->_confFile);
                                 });
                if (it != confCompatValue.end())
                {
                    // Use the first config file found at a listed location
                    break;
                }
                _confFile.clear();
            }
            catch (const sdbusplus::exception::SdBusError& e)
            {
                // Property unavailable on object.
                log<level::ERR>("Failed to get Names property",
                                entry("ERROR=%s", e.what()),
                                entry("INTERFACE=%s", confCompatIntf),
                                entry("PATH=%s", path.c_str()));
            }
        }

        return;
    }

  private:
    /**
     * @brief sd event handler.
     */
    sdeventplus::Event& _event;

    /**
     * @brief The config file name.
     */
    const std::string _fileName;

    /**
     * @brief The JSON config file
     */
    fs::path _confFile;

    /**
     * @brief The interfacesAdded match that is used to wait
     *        for the IBMCompatibleSystem interface to show up.
     */
    std::unique_ptr<sdbusplus::server::match::match> _match;

    /** DBusHandler class handles the D-Bus operations */
    utils::DBusHandler dBusHandler;
};

} // namespace phosphor::led
