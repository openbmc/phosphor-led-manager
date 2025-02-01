project(
    'phosphor-ledmanager',
    'cpp',
    version: '1.0.0',
    meson_version: '>=1.1.1',
    default_options: [
        'warning_level=3',
        'werror=true',
        'cpp_std=c++23',
        'buildtype=debugoptimized',
    ],
)

cpp = meson.get_compiler('cpp')

conf_data = configuration_data()
conf_data.set_quoted(
    'SAVED_GROUPS_FILE',
    '/var/lib/phosphor-led-manager/savedGroups',
)
conf_data.set_quoted('CALLOUT_FWD_ASSOCIATION', 'callout')
conf_data.set_quoted('CALLOUT_REV_ASSOCIATION', 'fault')
conf_data.set_quoted('ELOG_ENTRY', 'entry')
conf_data.set_quoted('LED_FAULT', 'fault')

conf_data.set('CLASS_VERSION', 1)
conf_data.set('LED_USE_JSON', get_option('use-json').allowed())
conf_data.set('USE_LAMP_TEST', get_option('use-lamp-test').allowed())
conf_data.set(
    'MONITOR_OPERATIONAL_STATUS',
    get_option('monitor-operational-status').allowed(),
)
conf_data.set(
    'PERSISTENT_LED_ASSERTED',
    get_option('persistent-led-asserted').allowed(),
)

nlohmann_json_dep = dependency('nlohmann_json', include_type: 'system')
phosphor_dbus_interfaces_dep = dependency('phosphor-dbus-interfaces')
phosphor_logging_dep = dependency('phosphor-logging')
sdbusplus_dep = dependency('sdbusplus')
sdeventplus_dep = dependency('sdeventplus')

prog_python = find_program('python3', required: true)
realpath_prog = find_program('realpath')


if cpp.has_header('CLI/CLI.hpp')
    CLI11_dep = declare_dependency()
else
    CLI11_dep = dependency('CLI11')
endif

# Get Cereal dependency.
cereal_dep = dependency('cereal', required: false)
has_cereal = cpp.has_header_symbol(
    'cereal/cereal.hpp',
    'cereal::specialize',
    dependencies: cereal_dep,
    required: false,
)
if not has_cereal
    cereal_opts = import('cmake').subproject_options()
    cereal_opts.add_cmake_defines(
        {'BUILD_TESTS': 'OFF', 'SKIP_PERFORMANCE_COMPARISON': 'ON'},
    )
    cereal_proj = import('cmake').subproject(
        'cereal',
        options: cereal_opts,
        required: false,
    )
    assert(cereal_proj.found(), 'cereal is required')
    cereal_dep = cereal_proj.dependency('cereal')
endif

deps = [
    CLI11_dep,
    cereal_dep,
    nlohmann_json_dep,
    phosphor_dbus_interfaces_dep,
    phosphor_logging_dep,
    sdbusplus_dep,
    sdeventplus_dep,
]

subdir('manager')
subdir('fault-monitor')

configure_file(output: 'config.h', configuration: conf_data)

install_data(
    'scripts/led-set-all-groups-asserted.sh',
    install_dir: get_option('bindir'),
)

if get_option('tests').allowed()
    subdir('test')
endif

install_subdir(
    'configs',
    install_dir: get_option('datadir') / 'phosphor-led-manager',
    strip_directory: true,
)
