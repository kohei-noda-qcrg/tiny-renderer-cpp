project(
  'tiny-renderer-cpp',
  'cpp',
  version: '1.0.0',
  default_options: ['warning_level=3', 'werror=false', 'cpp_std=c++23'],
)
add_project_arguments('-Wno-missing-field-initializers', language: 'cpp')
add_project_arguments('-Wfatal-errors', language: 'cpp')

executable(
  'main',
  files(
    'gl.cpp',
    'main.cpp',
    'model.cpp',
    'tgaimage.cpp',
  ),
  install: true,
)

executable(
  'test_perspective_clown',
  files(
    'test/perspective_clown.cpp',
    'gl.cpp',
    'model.cpp',
    'tgaimage.cpp',
  ),
)
