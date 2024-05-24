project('GSettings Study', 'C', 
        version: '1.0')

gnome = import('gnome')

schemas = gnome.compile_schemas()

libgtk_dep = dependency('gtk4', required: true)

executable('main', 'main.c', 'main.c', schemas,
  dependencies: libgtk_dep)
