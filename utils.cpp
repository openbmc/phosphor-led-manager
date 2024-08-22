#include "utils.hpp"

#include <phosphor-logging/lg2.hpp>

namespace phosphor
{
namespace led
{
namespace utils
{

// Get service name
std::string DBusHandler::getService(const std::string& path,
                                    const std::string& interface) const
{
    using InterfaceList = std::vector<std::string>;
    std::unordered_map<std::string, std::vector<std::string>> mapperResponse;

    auto& bus = DBusHandler::getBus();

    auto mapper = bus.new_method_call(mapperBusName, mapperObjPath, mapperIntf,
                                      "GetObject");
    mapper.append(path, InterfaceList({interface}));

    auto mapperResponseMsg = bus.call(mapper);
    mapperResponseMsg.read(mapperResponse);
    if (mapperResponse.empty())
    {
        lg2::error(
            "Failed to read getService mapper response, OBJECT_PATH = {PATH}, INTERFACE = {INTERFACE}",
            "PATH", path, "INTERFACE", interface);
        return "";
    }

    // the value here will be the service name
    return mapperResponse.cbegin()->first;
}

// Get all properties
PropertyMap DBusHandler::getAllProperties(const std::string& objectPath,
                                          const std::string& interface) const
{
    PropertyMap properties;

    auto& bus = DBusHandler::getBus();
    auto service = getService(objectPath, interface);
    if (service.empty())
    {
        return properties;
    }

    auto method = bus.new_method_call(service.c_str(), objectPath.c_str(),
                                      proIntf, "GetAll");
    method.append(interface);

    auto reply = bus.call(method);
    reply.read(properties);

    return properties;
}

// Get the property name
PropertyValue DBusHandler::getProperty(const std::string& objectPath,
                                       const std::string& interface,
                                       const std::string& propertyName) const
{
    PropertyValue value{};

    auto& bus = DBusHandler::getBus();
    auto service = getService(objectPath, interface);
    if (service.empty())
    {
        return value;
    }

    auto method = bus.new_method_call(service.c_str(), objectPath.c_str(),
                                      proIntf, "Get");
    method.append(interface, propertyName);

    auto reply = bus.call(method);
    reply.read(value);

    return value;
}

// Set property
void DBusHandler::setProperty(
    const std::string& objectPath, const std::string& interface,
    const std::string& propertyName, const PropertyValue& value) const
{
    auto& bus = DBusHandler::getBus();
    auto service = getService(objectPath, interface);
    if (service.empty())
    {
        return;
    }

    auto method = bus.new_method_call(service.c_str(), objectPath.c_str(),
                                      proIntf, "Set");
    method.append(interface.c_str(), propertyName.c_str(), value);

    bus.call_noreply(method);
}

std::vector<std::string> DBusHandler::getSubTreePaths(
    const std::string& objectPath, const std::string& interface) const
{
    std::vector<std::string> paths;

    auto& bus = DBusHandler::getBus();

    auto method = bus.new_method_call(mapperBusName, mapperObjPath, mapperIntf,
                                      "GetSubTreePaths");
    method.append(objectPath.c_str());
    method.append(0); // Depth 0 to search all
    method.append(std::vector<std::string>({interface.c_str()}));
    auto reply = bus.call(method);

    reply.read(paths);

    return paths;
}

} // namespace utils
} // namespace led
} // namespace phosphor
