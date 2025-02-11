project('gtuber', 'c',
  version: '0.0.1',
  meson_version: '>=0.53.0',
  license: 'LGPLv2.1+',
  default_options: [
    'warning_level=1',
    'buildtype=debugoptimized',
  ],
)

glib_req = '>=2.66'

gnome = import('gnome')
pkgconfig = import('pkgconfig')

prefix = get_option('prefix')
bindir = get_option('bindir')
datadir = get_option('datadir')
libdir = get_option('libdir')
includedir = get_option('includedir')

cdata = configuration_data()
cc = meson.get_compiler('c')

add_global_arguments(
  '-DG_LOG_USE_STRUCTURED=1',
  language: 'c',
)

gtuber_version = meson.project_version().split('-')[0]
version_array = gtuber_version.split('.')
gtuber_version_suffix = '-' + version_array[0] + '.0'

gtuber_api_name = meson.project_name() + gtuber_version_suffix

gtuber_utils_prefix = meson.project_name() + '-utils-'
gtuber_plugin_prefix = meson.project_name() + '-'

gtuber_plugins_libdir = join_paths(prefix, libdir, gtuber_api_name)

cdata.set('GTUBER_PLUGIN_PATH', '"@0@"'.format(gtuber_plugins_libdir))

warning_flags = [
  '-Wmissing-declarations',
  '-Wredundant-decls',
  '-Wwrite-strings',
  '-Wformat',
  '-Wformat-security',
  '-Winit-self',
  '-Wmissing-include-dirs',
  '-Waddress',
  '-Wno-multichar',
  '-Wvla',
  '-Wpointer-arith',
  '-Wmissing-prototypes',
  '-Wdeclaration-after-statement',
  '-Wold-style-definition',
]

foreach extra_arg : warning_flags
  if cc.has_argument (extra_arg)
    add_project_arguments([extra_arg], language: 'c')
  endif
endforeach

configure_file(
  output: 'config.h',
  configuration: cdata,
)

summary({
  'prefix': prefix,
  'bindir': bindir,
  'datadir': datadir,
  'libdir': libdir,
  'includedir': includedir,
}, section: 'Directories')

subdir('gtuber')
summary('introspection', build_gir ? 'Yes' : 'No', section: 'Build')
summary('vapi', build_vapi ? 'Yes' : 'No', section: 'Build')

subdir('doc')
summary('doc', build_doc, section: 'Build')

subdir('utils')
subdir('plugins')
