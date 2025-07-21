project('cheaptalk', 'cpp', version: '1.0.0', default_options : ['warning_level=3', 'werror=false', 'cpp_std=c++23'])
add_project_arguments('-Wno-missing-field-initializers', language: 'cpp')
add_project_arguments('-Wfatal-errors', language : 'cpp')

executable('main',
  files(
    'main.cpp',
    'tgaimage.cpp',
  ),
  install : true,
)
