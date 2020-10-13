fault_monitor_sources = [
    'fru-fault-monitor.cpp',
    'monitor-main.cpp',
]

executable(
    'phosphor-fru-fault-monitor',
    fault_monitor_sources,
    include_directories: ['.', '../'],
    dependencies: deps,
    install: true,
    link_with: libphosphor_dbus,
    install_dir: get_option('bindir')
)
