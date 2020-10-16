#include "utils.hpp"

namespace phosphor
{
namespace led
{
namespace utils
{

/** Get service name */
std::string DBusHandler::getService(const char* path, const char* interface)
{
    using DbusInterfaceList = std::vector<std::string>;
    std::map<std::string, std::vector<std::string>> mapperResponse;

    auto& bus = DBusHandler::getBus();

    auto mapper = bus.new_method_call(MAPPER_BUSNAME, MAPPER_OBJ_PATH,
                                      MAPPER_IFACE, "GetObject");
    mapper.append(path, DbusInterfaceList({interface}));

    auto mapperResponseMsg = bus.call(mapper);
    mapperResponseMsg.read(mapperResponse);

    return mapperResponse.cbegin()->first;
}

/** Get peorperty and return the value of the property*/
PropertyValue DBusHandler::getProperty(const char* objPath,
                                       const char* dbusProp,
                                       const char* dbusInterface)
{
    auto& bus = DBusHandler::getBus();
    auto service = getService(objPath, dbusInterface);
    auto method = bus.new_method_call(service.c_str(), objPath,
                                      DBUS_PROPERTY_IFACE, "Get");
    method.append(dbusInterface, dbusProp);

    PropertyValue value{};
    auto reply = bus.call(method);
    reply.read(value);

    return value;
}

/** Get subtree paths and return the vector of the path*/
Paths DBusHandler::getSubTreePaths(const std::string& objPath,
                                   const std::vector<std::string>& intfs,
                                   int depth)
{
    auto& bus = DBusHandler::getBus();
    auto mapperCall = bus.new_method_call(MAPPER_BUSNAME, MAPPER_OBJ_PATH,
                                          MAPPER_IFACE, "GetSubTreePaths");

    mapperCall.append(objPath);
    mapperCall.append(depth);
    mapperCall.append(intfs);
    auto reply = bus.call(mapperCall);

    Paths paths;
    reply.read(paths);

    return paths;
}

/** Get subtree and return the map of the object*/
ObjectTree DBusHandler::getSubTree(const std::string& objPath,
                                   const std::vector<std::string>& intfs,
                                   int depth)
{
    auto& bus = DBusHandler::getBus();
    auto mapperCall = bus.new_method_call(MAPPER_BUSNAME, MAPPER_OBJ_PATH,
                                          MAPPER_IFACE, "GetSubTree");

    mapperCall.append(objPath);
    mapperCall.append(depth);
    mapperCall.append(intfs);
    auto reply = bus.call(mapperCall);

    ObjectTree objectTree;
    reply.read(objectTree);

    return objectTree;
}

/** Set property */
void DBusHandler::setProperty(const std::string& objectPath,
                              const std::string& interface,
                              const std::string& propertyName,
                              const PropertyValue& value)
{
    auto& bus = DBusHandler::getBus();
    auto service = getService(objectPath.c_str(), interface.c_str());
    auto method = bus.new_method_call(service.c_str(), objectPath.c_str(),
                                      DBUS_PROPERTY_IFACE, "Set");
    method.append(interface.c_str(), propertyName.c_str(), value);

    bus.call_noreply(method);
}

} // namespace utils
} // namespace led
} // namespace phosphor