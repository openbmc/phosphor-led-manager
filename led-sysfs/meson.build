
udevdir = dependency('udev').get_variable('udevdir')
install_data(['udev' / 'rules.d' / '70-leds.rules'], install_dir : udevdir / 'rules.d')

systemd = dependency('systemd')
install_data(['systemd' / 'system' / 'phosphor-ledcontroller.service'],
             install_dir: systemd.get_variable(pkgconfig: 'systemdsystemunitdir')
)
install_data(['systemd' / 'system' / 'sysfs-led@.service'],
             install_dir: systemd.get_variable(pkgconfig: 'systemdsystemunitdir')
)

dbus = dependency('dbus-1')
install_data(['dbus-1' / 'system-services' / 'xyz.openbmc_project.LED.Controller.service'],
             install_dir: dbus.get_variable(pkgconfig: 'system_bus_services_dir')
)

led_sysfs_deps = [
    CLI11_dep,
    sdbusplus_dep,
    phosphor_dbus_interfaces_dep,
    phosphor_logging_dep,
]

executable(
    'phosphor-ledcontroller',
    'interfaces/internal_interface.cpp',
    'controller.cpp',
    'physical.cpp',
    'sysfs.cpp',
    implicit_include_directories: true,
    dependencies: led_sysfs_deps,
    install: true,
    install_dir: '/usr/libexec/phosphor-led-sysfs'
)

executable(
    'add-led-action',
    'argument.cpp',
    'add_led_action.cpp',
    implicit_include_directories: true,
    dependencies: led_sysfs_deps,
    install: true,
    install_dir: '/usr/libexec/phosphor-led-sysfs'
)

if get_option('tests').enabled()
  subdir('test')
endif
