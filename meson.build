project(
    'phosphor-ledmanager', 'cpp',
    version : '1.0.0',
    meson_version: '>=0.57.0',
    default_options: [
        'warning_level=3',
        'werror=true',
        'cpp_std=c++20',
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

conf_data.set('CLASS_VERSION', 1)
conf_data.set('LED_USE_JSON', get_option('use-json').enabled())
conf_data.set('USE_LAMP_TEST', get_option('use-lamp-test').enabled())
conf_data.set('MONITOR_OPERATIONAL_STATUS', get_option('monitor-operational-status').enabled())

sdbusplus_dep = dependency('sdbusplus', required: false)
if sdbusplus_dep.found()
    sdbusplusplus_prog = find_program('sdbus++')
    sdbuspp_gen_meson_prog = find_program('sdbus++-gen-meson')
else
    sdbusplus_proj = subproject('sdbusplus', required: true)
    sdbusplus_dep = sdbusplus_proj.get_variable('sdbusplus_dep')
    sdbusplusplus_prog = sdbusplus_proj.get_variable('sdbusplusplus_prog')
    sdbuspp_gen_meson_prog = sdbusplus_proj.get_variable(
        'sdbuspp_gen_meson_prog'
    )
endif

sdeventplus_dep = dependency(
    'sdeventplus',
    fallback: [
        'sdeventplus',
        'sdeventplus_dep'
    ],
)
phosphor_logging_dep = dependency(
    'phosphor-logging',
    fallback: [
        'phosphor-logging',
        'phosphor_logging_dep'
    ],
)
phosphor_dbus_interfaces_dep = dependency(
    'phosphor-dbus-interfaces',
    fallback: [
        'phosphor-dbus-interfaces',
        'phosphor_dbus_interfaces_dep'
    ],
)
fmt_dep = dependency('fmt', required: false)
if not fmt_dep.found()
  fmt_proj = import('cmake').subproject(
    'fmt',
    cmake_options: [
      '-DCMAKE_POSITION_INDEPENDENT_CODE=ON',
      '-DMASTER_PROJECT=OFF'
    ],
    required: false)
  assert(fmt_proj.found(), 'fmtlib is required')
  fmt_dep = fmt_proj.dependency('fmt')
endif

prog_python = find_program('python3', required: true)
realpath_prog = find_program('realpath')

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

selected_subdirs = []
selected_subdirs += 'xyz/openbmc_project'

generated_root = meson.current_build_dir() / 'gen'
generated_others = []
generated_sources = []

# Source the generated meson files.
subdir('gen')
foreach d : selected_subdirs
  subdir('gen' / d)
endforeach

# Parse through the list from sdbus++-gendir and put into sets.
generated_headers = []
generated_cpp = []
generated_others_files = []

foreach g : generated_sources generated_others
    foreach f : g.to_list()
        rel_path = run_command(
            realpath_prog,
            '--relative-to', generated_root,
            f.full_path(),
        ).stdout().strip().split('\n')[-1]

        if rel_path.endswith('.hpp')
            generated_headers += rel_path
        elif rel_path.endswith('.cpp')
            generated_cpp += rel_path
        else
            generated_others_files += rel_path
        endif
    endforeach
endforeach

deps = [
    sdbusplus_dep,
    sdeventplus_dep,
    phosphor_logging_dep,
    phosphor_dbus_interfaces_dep,
    nlohmann_json,
    fmt_dep,
]

sources = [
    'group.cpp',
    'led-main.cpp',
    'manager.cpp',
    'serialize.cpp',
    'utils.cpp',
]

if get_option('use-json').disabled()
    led_gen_hpp = custom_target(
        'led-gen.hpp',
        command : [
            prog_python,
            meson.project_source_root() + '/parse_led.py',
            '-i', meson.project_source_root(),
            '-o', meson.current_build_dir(),
        ],
        output : 'led-gen.hpp')
    sources += [led_gen_hpp]
endif

if get_option('use-lamp-test').enabled()
    conf_data.set_quoted('LAMP_TEST_OBJECT', '/xyz/openbmc_project/led/groups/lamp_test')
    conf_data.set_quoted('HOST_LAMP_TEST_OBJECT', '/xyz/openbmc_project/led/groups/host_lamp_test')
    conf_data.set_quoted('LAMP_TEST_LED_OVERRIDES_JSON', '/usr/share/phosphor-led-manager/lamp-test-led-overrides.json')
    conf_data.set('LAMP_TEST_TIMEOUT_IN_SECS', 240)

    sources += ['lamptest.cpp']
endif

configure_file(output: 'config.h',
    configuration: conf_data
)

install_data(
    'scripts/led-set-all-groups-asserted.sh',
    install_dir: get_option('bindir')
)

executable(
    'phosphor-ledmanager',
    sources,
    generated_sources,
    include_directories: include_directories('gen'),
    implicit_include_directories: true,
    dependencies: deps,
    install: true,
    install_dir: get_option('bindir')
)
subdir('fault-monitor')

build_tests = get_option('tests')
if not build_tests.disabled()
  subdir('test')
endif

install_subdir('configs',
    install_dir: get_option('datadir') / 'phosphor-led-manager',
    strip_directory: true)
