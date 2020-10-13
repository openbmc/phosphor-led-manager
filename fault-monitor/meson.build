sources = [
    'fru-fault-monitor.cpp',
    'monitor-main.cpp',
]

executable(
    'phosphor-fru-fault-monitor',
    sources,
    include_directories: ['../'],
    dependencies: deps,
    link_with : fault_files,
    install: true,
    install_dir: get_option('bindir')
)
