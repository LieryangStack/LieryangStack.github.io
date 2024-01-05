---
layout: post
title: 十二、Meson构建文件函数——executable()
categories: Meson
tags: [Meson]
---

Meson本质上是用 `Python` 编写的，所以这些函数也都是Python函数。

## 1 executable()

创建一个新的可执行文件。第一个参数指定其名称，其余的位置参数定义要使用的输入文件。

kwargs 的列表（例如 sources、objects 和 dependencies）总是被扁平化处理，这意味着在创建最终列表时，你可以自由嵌套和添加列表。

返回的对象还具有在 exe 中记录的方法。

自从 1.3.0 版本起，只要每个目标都有不同的 name_suffix，可执行文件的名称可以在多个目标中相同。

## 2 executable()定义

```python
# Creates a new executable
exe executable(
  str                                                   target_name,     # The *unique* name of the build target
  str | file | custom_tgt | custom_idx | generated_list source...,       # Input source to compile

  # Keyword arguments:
  <lang>_args                  : list[str]                                                               # compiler flags to use for the given language;
  <lang>_pch                   : str                                                                     # precompiled header file to use for the given language
  build_by_default             : bool                                                                    # Causes, when set to `true`, to have this target be built by default
  build_rpath                  : str                                                                     # A string to add to target's rpath definition in the build dir,
  d_debug                      : list[str]                                                               # The [D version identifiers](https://dlang
  d_import_dirs                : list[str]                                                               # List of directories to look in for string imports used in the D programming language
  d_module_versions            : list[str | int]                                                         # List of module version identifiers set when compiling D sources
  d_unittest                   : bool                                                                    # When set to true, the D modules are compiled in debug mode
  dependencies                 : list[dep]                                                               # one or more dependency objects
  export_dynamic               : bool                                                                    # when set to true causes the target's symbols to be
  extra_files                  : str | file | custom_tgt | custom_idx                                    # Not used for the build itself but are shown as source files in IDEs
  gnu_symbol_visibility        : str                                                                     # Specifies how symbols should be exported, see
  gui_app                      : bool                                                                    # When set to true flags this target as a GUI application
  implib                       : bool | str                                                              # When set to true, an import library is generated for the
  implicit_include_directories : bool                                                                    # Controls whether Meson adds the current source and build directories to the include path
  include_directories          : list[inc | str]                                                         # one or more objects created with the include_directories() function,
  install                      : bool                                                                    # When set to true, this executable should be installed
  install_dir                  : str                                                                     # override install directory for this file
  install_mode                 : list[str | int]                                                         # Specify the file mode in symbolic format
  install_rpath                : str                                                                     # A string to set the target's rpath to after install
  install_tag                  : str                                                                     # A string used by the `meson install --tags` command
  link_args                    : list[str]                                                               # Flags to use during linking
  link_depends                 : str | file | custom_tgt | custom_idx                                    # Strings, files, or custom targets the link step depends on
  link_language                : str                                                                     # Makes the linker for this target be for the specified language
  link_whole                   : list[lib | custom_tgt | custom_idx]                                     # Links all contents of the given static libraries whether they are used or
  link_with                    : list[lib | custom_tgt | custom_idx]                                     # One or more shared or static libraries
  name_prefix                  : str | list[void]                                                        # The string that will be used as the prefix for the
  name_suffix                  : str | list[void]                                                        # The string that will be used as the extension for the
  native                       : bool                                                                    # Controls whether the target is compiled for the build or host machines
  objects                      : list[extracted_obj | file | str]                                        # List of object files that should be linked in this target
  override_options             : list[str] | dict[str | bool | int | list[str]]                            # takes an array of strings in the same format as `project`'s `default_options`
  pie                          : bool                                                                    # Build a position-independent executable
  rust_crate_type              : str                                                                     # Set the specific type of rust crate to compile (when compiling rust)
  rust_dependency_map          : dict[str]                                                               # On rust targets this provides a map of library names to the crate name
  sources                      : str | file | custom_tgt | custom_idx | generated_list | structured_src  # Additional source files
  vala_args                    : list[str | file]                                                        # Compiler flags for Vala
  vs_module_defs               : str | file | custom_tgt | custom_idx                                    # Specify a Microsoft module definition file for controlling symbol exports,
  win_subsystem                : str                                                                     # Specifies the subsystem type to use
)
```

## 3 executable()举例

### 3.1 示例一

```python
app2_resources = gnome.compile_resources('exampleapp2_resources',
  'exampleapp.gresource.xml',
  source_dir: '.')

executable('exampleapp2',
  'exampleapp.c', 'exampleappwin.c', 'main.c', app2_resources,
  dependencies: libgtk_dep,
  c_args: common_cflags)
```

## 3.2 示例二

```python
project('simple', 'c')
src = ['source1.c', 'source2.c', 'source3.c']
executable('myexe', src)
```

