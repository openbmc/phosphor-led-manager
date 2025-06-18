fault_monitor_sources = ['monitor-main.cpp']

if get_option('monitor-operational-status').allowed()
    fault_monitor_sources += ['../utils.cpp', 'operational-status-monitor.cpp']
else
    fault_monitor_sources += ['fru-fault-monitor.cpp']
endif

executable(
    'phosphor-fru-fault-monitor',
    fault_monitor_sources,
    include_directories: ['.', '../'],
    dependencies: deps,
    install: true,
    install_dir: get_option('libexecdir') / 'phosphor-led-manager',
)
