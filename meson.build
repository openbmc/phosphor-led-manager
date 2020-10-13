project(
    'phosphor-ledmanager',
    'cpp',
    default_options: [
        'warning_level=3',
        'werror=true',
        'cpp_std=c++17'
    ]
)

conf_data = configuration_data()
conf_data.set_quoted('BUSNAME', 'xyz.openbmc_project.LED.GroupManager')
conf_data.set_quoted('OBJPATH', '/xyz/openbmc_project/led/groups')
conf_data.set_quoted('LED_JSON_FILE', '/usr/share/phosphor-led-manager/led-group-config.json')
conf_data.set_quoted('SAVED_GROUPS_FILE', '/var/lib/phosphor-led-manager/savedGroups')
conf_data.set_quoted('CALLOUT_FWD_ASSOCIATION', 'callout')
conf_data.set_quoted('CALLOUT_REV_ASSOCIATION', 'fault')
conf_data.set_quoted('ELOG_ENTRY', 'entry')
conf_data.set_quoted('LED_FAULT', 'fault')

conf_data.set('LED_USE_JSON', get_option('use-json').disabled())
conf_data.set('CLASS_VERSION', get_option('CLASS_VERSION'))

configure_file(output: 'config.h',
    configuration: conf_data
)

sdbuspp = find_program('sdbus++')

prog_python = import('python').find_installation('python3')

if get_option('use-json').disabled()
    led_gen_hpp = custom_target(
        'led-gen.hpp',
        command : [
            prog_python,
            meson.source_root() + '/parse_led.py',
            '-o', meson.current_build_dir(),
        ],
        output : 'led-gen.hpp')
endif

deps = [
    dependency('sdbusplus'),
    dependency('phosphor-logging'),
    dependency('phosphor-dbus-interfaces'),
]

sources = [
    'led-main.cpp',
    led_gen_hpp,
    'manager.cpp',
    'group.cpp',
    'serialize.cpp',
]

executable(
    'phosphor-ledmanager',
    sources,
    implicit_include_directories: false,
    dependencies: deps,
    install: true,
    install_dir: get_option('bindir')
)
subdir('test')
subdir('fault-monitor')
