fault_monitor_sources = [
    generated_sources,
    'monitor-main.cpp',
]

if get_option('monitor-operational-status').enabled()
    fault_monitor_sources += [
        '../utils.cpp',
        'operational-status-monitor.cpp',
        ]
else
    fault_monitor_sources += [
        'fru-fault-monitor.cpp',
        ]
endif

executable(
    'phosphor-fru-fault-monitor',
    '../utils.cpp',
    'multi-purpose-status.cpp',
    fault_monitor_sources,
    include_directories: ['.', '../', '../gen'],
    dependencies: deps,
    install: true,
    install_dir: get_option('bindir')
)
