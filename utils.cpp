#include "utils.hpp"

#include <phosphor-logging/log.hpp>

namespace phosphor
{
namespace led
{
namespace utils
{

using namespace phosphor::logging;

// Get service name
const std::string DBusHandler::getService(const std::string& path,
                                          const std::string& interface) const
{

    using InterfaceList = std::vector<std::string>;
    std::map<std::string, std::vector<std::string>> mapperResponse;

    auto& bus = DBusHandler::getBus();

    auto mapper = bus.new_method_call(MAPPER_BUSNAME, MAPPER_OBJ_PATH,
                                      MAPPER_IFACE, "GetObject");
    mapper.append(path, InterfaceList({interface}));

    auto mapperResponseMsg = bus.call(mapper);
    if (mapperResponseMsg.is_method_error())
    {
        log<level::ERR>("Failed to invoke ObjectMapper method",
                        entry("OBJECT_PATH=%s", path.c_str()),
                        entry("INTERFACE=%s", interface.c_str()));
        return "";
    }

    mapperResponseMsg.read(mapperResponse);
    if (mapperResponse.empty())
    {
        log<level::ERR>("Failed to read getService mapper response",
                        entry("OBJECT_PATH=%s", path.c_str()),
                        entry("INTERFACE=%s", interface.c_str()));
        return "";
    }

    // the value here will be the service name
    return mapperResponse.cbegin()->first;
}

// Set property
void DBusHandler::setProperty(const std::string& objectPath,
                              const std::string& interface,
                              const std::string& propertyName,
                              const PropertyValue& value) const
{
    auto& bus = DBusHandler::getBus();
    auto service = getService(objectPath, interface);
    if (service.empty())
    {
        return;
    }

    auto method = bus.new_method_call(service.c_str(), objectPath.c_str(),
                                      DBUS_PROPERTY_IFACE, "Set");
    method.append(interface.c_str(), propertyName.c_str(), value);

    bus.call_noreply(method);
}

} // namespace utils
} // namespace led
} // namespace phosphor