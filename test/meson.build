test_sources = [
  '../manager.cpp',
  '../serialize.cpp',
  '../utils.cpp'
]

tests = [
  'utest.cpp',
  'utest-led-json.cpp',
  'utest-serialize.cpp',
]

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