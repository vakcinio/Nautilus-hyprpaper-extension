project(
    'nautilus-hyprpaper',
    'c',
    version: '1.0',
    license: 'GPL-2.0-or-later'
)

dependencies = [
    dependency('libnautilus-extension-4'),
    dependency('glib-2.0')
]

extension_dir = run_command(
  'pkg-config',
  '--variable=extensiondir',
  'libnautilus-extension-4',
  check: true
).stdout().strip()

i18n = import('i18n')

locale_dir = join_paths(
    get_option('prefix'), 
    get_option('datadir'), 
    'locale'
)

add_project_arguments(
    '-DGETTEXT_PACKAGE="'+meson.project_name()+'"',
    language:'c'
)

add_project_arguments(
  '-DLOCALEDIR="' + locale_dir + '"',
  language: 'c'
)

subdir('po')

shared_module(
    'nautilus-hyprpaper',
    'src/nautilus-hyprpaper.c',
    dependencies: dependencies,
    install: true,
    install_dir: extension_dir
)