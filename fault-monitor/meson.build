fault_monitor_sources = [
    generated_sources,
    'monitor-main.cpp',
]

if get_option('multi-purpose-monitor').enabled()
    fault_monitor_sources += [
        '../utils.cpp',
        'multi-purpose-status.cpp',
        ]
endif

if get_option('monitor-operational-status').enabled()
    fault_monitor_sources += [
        '../utils.cpp',
        'operational-status-monitor.cpp',
        ]
endif

if get_option('fru-fault-monitor').enabled()
    fault_monitor_sources += [
        'fru-fault-monitor.cpp',
        ]
endif

executable(
    'phosphor-fru-fault-monitor',
    fault_monitor_sources,
    include_directories: ['.', '../', '../gen'],
    dependencies: deps,
    install: true,
    install_dir: get_option('bindir')
)
