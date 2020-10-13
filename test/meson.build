test_sources = [
  '../manager.cpp',
  '../serialize.cpp',
  '../utils.cpp'
]

tests = [
  'utest.cpp',
  'utest-serialize.cpp',
]

if get_option('use-json').enabled()
  tests += ['utest-led-json.cpp']
endif

foreach t : tests
  test(t, executable(t.underscorify(), t,
                     test_sources,
                     include_directories: ['..'],
                     dependencies: [
                         gtest,
                         gmock,
                         deps
                         ]),
       workdir: meson.current_source_dir())
endforeach