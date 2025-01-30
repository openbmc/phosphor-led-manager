gtest_dep = dependency('gtest', main: true, disabler: true, required: false)
gmock_dep = dependency('gmock', disabler: true, required: false)
if not gtest_dep.found() or not gmock_dep.found()
    gtest_proj = import('cmake').subproject('googletest', required: false)
    if gtest_proj.found()
        gtest_dep = declare_dependency(
            dependencies: [
                dependency('threads'),
                gtest_proj.dependency('gtest'),
                gtest_proj.dependency('gtest_main'),
            ],
        )
        gmock_dep = gtest_proj.dependency('gmock')
    else
        assert(
            not get_option('tests').allowed(),
            'Googletest is required if tests are enabled',
        )
    endif
endif

test_sources = [
    '../manager/manager.cpp',
    '../manager/config-validator.cpp',
    '../utils.cpp',
]

tests = [
    'utest.cpp',
    'utest-led-json.cpp',
    'utest-group-priority.cpp',
    'utest-config-validator.cpp',
]
if get_option('persistent-led-asserted').allowed()
    test_sources += ['../manager/serialize.cpp']
    tests += ['utest-serialize.cpp']
endif

foreach t : tests
    test(
        t,
        executable(
            t.underscorify(),
            t,
            test_sources,
            include_directories: ['..', '../manager'],
            dependencies: [gtest_dep, gmock_dep, deps],
        ),
        workdir: meson.current_source_dir(),
    )
endforeach
