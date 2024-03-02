---
layout: post
title: 十七、Meson构建文件函数——static_library()
categories: Meson
tags: [Meson]
---

Meson本质上是用 `Python` 编写的，所以这些函数也都是Python函数。

## 1 static_library()

使用给定的源代码构建静态库。

## 2 static_library()定义

```python
# Builds a static library with the given sources
lib static_library(
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
  extra_files                  : str | file | custom_tgt | custom_idx                                    # Not used for the build itself but are shown as source files in IDEs
  gnu_symbol_visibility        : str                                                                     # Specifies how symbols should be exported, see
  gui_app                      : bool                                                                    # When set to true flags this target as a GUI application
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
  pic                          : bool                                                                    # Builds the library as positional independent code
  prelink                      : bool                                                                    # If `true` the object files in the target will be prelinked,
  rust_abi                     : str                                                                     # Set the specific ABI to compile (when compiling rust)
  rust_crate_type              : str                                                                     # Set the specific type of rust crate to compile (when compiling rust)
  rust_dependency_map          : dict[str]                                                               # On rust targets this provides a map of library names to the crate name
  sources                      : str | file | custom_tgt | custom_idx | generated_list | structured_src  # Additional source files
  vala_args                    : list[str | file]                                                        # Compiler flags for Vala
  win_subsystem                : str                                                                     # Specifies the subsystem type to use
)
```

## 3 static_library()示例

```meson
catch2 = static_library(
  'Catch2',
  sources,
  include_directories: '..',
  install: true,
)

catch2_dep = declare_dependency(
  link_with: catch2,
  include_directories: '..',
)
```
