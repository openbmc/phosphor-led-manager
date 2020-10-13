# Generated file; do not modify.
subdir('Fru')
subdir('Mapper')
generated_others += custom_target(
    'xyz/openbmc_project/Led/Mapper__markdown'.underscorify(),
    input: [ meson.source_root() / 'xyz/openbmc_project/Led/Mapper.errors.yaml',  ],
    output: [ 'Mapper.md' ],
    command: [
        sdbuspp_gen_meson_prog, '--command', 'markdown',
        '--output', meson.current_build_dir(),
        '--tool', sdbusplusplus_prog,
        '--directory', meson.source_root(),
        'xyz/openbmc_project/Led/Mapper',
    ],
    build_by_default: true,
)

