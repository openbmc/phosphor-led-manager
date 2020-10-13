mapper_error_hpp = custom_target(
    'error.hpp',
    capture : true,
    command : [
        sdbuspp,
        '-r',
        meson.source_root() + '/fault-monitor',
        'error',
        'exception-header',
        'xyz.openbmc_project.Led.Mapper',
    ],
    input : '../Mapper.errors.yaml',
    install : true,
    install_dir : get_option('includedir') / 'fault-monitor/xyz/openbmc_project/Led/Mapper',
    output : 'error.hpp',
)

mapper_error_cpp = custom_target(
    'error.cpp',
    capture : true,
    command : [
        sdbuspp,
        '-r',
        meson.source_root() + '/fault-monitor',
        'error',
        'exception-cpp',
        'xyz.openbmc_project.Led.Mapper',
    ],
    input : '../Mapper.errors.yaml',
    output : 'error.cpp',
)