project(
    'cpp-lox',
    'cpp',
    version : '0.0.1',
    license : 'MIT',
    default_options : ['cpp_std=c++11'])

# it will be referred from subdir projects
inc = include_directories('include')

# meson will try to find a meson.build file inside following directories
subdir('include')
subdir('src')
subdir('test')
