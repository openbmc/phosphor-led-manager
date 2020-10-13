fault_monitor_sources = [
    generated_sources,
    'fru-fault-monitor.cpp',
    'monitor-main.cpp',
]

executable(
    'phosphor-fru-fault-monitor',
    fault_monitor_sources,
    include_directories: ['.', '../', '../gen'],
    dependencies: deps,
    install: true,
    install_dir: get_option('bindir')
)
