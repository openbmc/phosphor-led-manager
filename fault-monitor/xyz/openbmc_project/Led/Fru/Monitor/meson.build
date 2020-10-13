monitor_error_hpp = custom_target(
    'error.hpp',
    capture : true,
    command : [
        sdbuspp,
        '-r',
        meson.source_root() + '/fault-monitor',
        'error',
        'exception-header',
        'xyz.openbmc_project.Led.Fru.Monitor',
    ],
    input : '../Monitor.errors.yaml',
    install : true,
    install_dir : get_option('includedir') / 'fault-monitor/xyz/openbmc_project/Led/Fru/Monitor',
    output : 'error.hpp',
)

monitor_error_cpp = custom_target(
    'error.cpp',
    capture : true,
    command : [
        sdbuspp,
        '-r',
        meson.source_root() + '/fault-monitor',
        'error',
        'exception-cpp',
        'xyz.openbmc_project.Led.Fru.Monitor',
    ],
    input : '../Monitor.errors.yaml',
    output : 'error.cpp',
)