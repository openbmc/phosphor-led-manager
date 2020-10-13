sources = [
    fault_generated_files,
    'fru-fault-monitor.cpp',
    'monitor-main.cpp',
]

executable(
    'phosphor-fru-fault-monitor',
    sources,
    include_directories: ['../'],
    dependencies: deps,
    install: true,
    install_dir: get_option('bindir')
)
