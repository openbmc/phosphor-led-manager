# Generated file; do not modify.
subdir('Monitor')
generated_others += custom_target(
    'xyz/openbmc_project/Led/Fru/Monitor__markdown'.underscorify(),
    input: [ meson.source_root() / 'xyz/openbmc_project/Led/Fru/Monitor.errors.yaml',  ],
    output: [ 'Monitor.md' ],
    command: [
        sdbuspp_gen_meson_prog, '--command', 'markdown',
        '--output', meson.current_build_dir(),
        '--tool', sdbusplusplus_prog,
        '--directory', meson.source_root(),
        'xyz/openbmc_project/Led/Fru/Monitor',
    ],
    build_by_default: true,
)

