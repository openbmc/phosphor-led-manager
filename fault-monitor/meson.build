subdir('xyz/openbmc_project/Led/Fru/Monitor')
subdir('xyz/openbmc_project/Led/Mapper')

sources = [
    monitor_error_hpp,
    monitor_error_cpp,
    mapper_error_hpp,
    mapper_error_cpp,
    'fru-fault-monitor.cpp',
    'monitor-main.cpp',
]

deps = [
    dependency('sdbusplus'),
    dependency('phosphor-logging'),
    dependency('phosphor-dbus-interfaces'),
]

executable(
    'phosphor-fru-fault-monitor',
    sources,
    include_directories: ['../'],
    dependencies: deps,
    install: true,
    install_dir: get_option('bindir')
)


