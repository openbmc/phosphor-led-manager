project(
    'phosphor-ledmanager', 'cpp',
    version : '1.0',
    meson_version: '>=0.53.2',
    default_options: [
        'warning_level=3',
        'werror=true',
        'cpp_std=c++17',
        'buildtype=debugoptimized'
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

configure_file(output: 'config.h',
    configuration: conf_data
)

sdbusplus = dependency('sdbusplus')
phosphorlogging = dependency('phosphor-logging')
phosphordbusinterfaces = dependency('phosphor-dbus-interfaces')
gtest = dependency('gtest', main: true, disabler: true, required: true)
gmock = dependency('gmock', disabler: true, required: true)
sdbuspp = find_program('sdbus++', required: true)
sdbusgen_prog = find_program('sdbus++-gendir', required: true)
prog_python = find_program('python3', required: true)

cpp = meson.get_compiler('cpp')
if cpp.has_header('nlohmann/json.hpp')
    nlohmann_json = declare_dependency()
else
    subproject('nlohmann', required: false)
    nlohmann_json = declare_dependency(
        include_directories: [
            'subprojects/nlohmann/single_include',
            'subprojects/nlohmann/single_include/nlohmann',
        ]
    )
endif

deps = [
    sdbusplus,
    phosphorlogging,
    phosphordbusinterfaces,
    nlohmann_json
]

fault_buildroot = meson.current_build_dir() + '/fault-monitor'
fault_files = files(
    run_command(
        sdbusgen_prog,
        '--tool', sdbuspp,
        '--output', fault_buildroot,
        'xyz',
        check: true
    ).stdout().strip().split('\n')[-1]
)

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

sources = [
    'group.cpp',
    'led-main.cpp',
    'manager.cpp',
    'serialize.cpp',
    led_gen_hpp,
]

executable(
    'phosphor-ledmanager',
    sources,
    implicit_include_directories: false,
    dependencies: deps,
    install: true,
    install_dir: get_option('bindir')
)

subdir('fault-monitor')

build_tests = get_option('tests')
if not build_tests.disabled()
  subdir('test')
endif
