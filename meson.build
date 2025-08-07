project(
  'tiny-renderer-cpp',
  'cpp',
  version: '1.0.0',
  default_options: ['warning_level=3', 'werror=false', 'cpp_std=c++23'],
)
add_project_arguments('-Wno-missing-field-initializers', language: 'cpp')
add_project_arguments('-Wfatal-errors', language: 'cpp')

common_sources = files(
  'gl.cpp',
  'model.cpp',
  'tgaimage.cpp',
)

executable(
  'main',
  files('main.cpp') + common_sources,
  install: true,
)

executable(
  'test_sample_triangle_nomodel',
  files('test/sample_triangle.cpp') + common_sources,
)

#executable(
#  'test_clown',
#  files('test/clown.cpp') + common_sources,
#)

executable(
  'test_illumination',
  files('test/illumination.cpp') + common_sources,
)

executable(
  'test_perspective_clown',
  files('test/perspective_clown.cpp') + common_sources,
)
