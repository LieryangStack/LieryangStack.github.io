---
layout: post
title: 十二、Meson构建文件函数——executable()
categories: Meson
tags: [Meson]
---

Meson本质上是用 `Python` 编写的，所以这些函数也都是Python函数。

## 1 executable()

- 创建一个新的可执行文件。第一个参数指定其名称，其余的位置参数定义要使用的输入文件。

- kwargs 的列表（例如 sources、objects 和 dependencies）总是被扁平化处理，这意味着在创建最终列表时，你可以自由嵌套和添加列表。

## 2 executable()定义

```python
# Creates a new executable
exe executable(
  str                                                   target_name,     # The *unique* name of the build target
  str | file | custom_tgt | custom_idx | generated_list source...,       # Input source to compile

  # Keyword arguments:
  <lang>_args                  : list[str] # compiler flags to use for the given language;
  <lang>_pch                   : str       # precompiled header file to use for the given language
  build_by_default             : bool      # Causes, when set to `true`, to have this target be built by default
  # -Wl,rpath,str
  build_rpath                  : str       # A string to add to target's rpath definition in the build dir,
  d_debug                      : list[str]     # The [D version identifiers](https://dlang
  d_import_dirs                : list[str]     # List of directories to look in for string imports used in the D programming language
  d_module_versions            : list[str | int]                                                    # List of module version identifiers set when compiling D sources
  d_unittest                   : bool          # When set to true, the D modules are compiled in debug mode
  dependencies                 : list[dep]     # one or more dependency objects
  export_dynamic               : bool          # when set to true causes the target's symbols to be
  extra_files                  : str | file | custom_tgt | custom_idx                               # Not used for the build itself but are shown as source files in IDEs
  gnu_symbol_visibility        : str           # Specifies how symbols should be exported, see
  gui_app                      : bool          # When set to true flags this target as a GUI application
  implib                       : bool | str                                                         # When set to true, an import library is generated for the
  implicit_include_directories : bool          # Controls whether Meson adds the current source and build directories to the include path
  include_directories          : list[inc | str]                                                    # one or more objects created with the include_directories() function,
  install                      : bool          # When set to true, this executable should be installed
  install_dir                  : str           # override install directory for this file
  install_mode                 : list[str | int]                                                    # Specify the file mode in symbolic format
  install_rpath                : str           # A string to set the target's rpath to after install
  install_tag                  : str           # A string used by the `meson install --tags` command
  link_args                    : list[str]     # Flags to use during linking
  link_depends                 : str | file | custom_tgt | custom_idx                               # Strings, files, or custom targets the link step depends on
  link_language                : str           # Makes the linker for this target be for the specified language
  link_whole                   : list[lib | custom_tgt | custom_idx]                                # Links all contents of the given static libraries whether they are used or
  link_with                    : list[lib | custom_tgt | custom_idx]                                # One or more shared or static libraries
  name_prefix                  : str | list[void]                                                   # The string that will be used as the prefix for the
  name_suffix                  : str | list[void]                                                   # The string that will be used as the extension for the
  native                       : bool          # Controls whether the target is compiled for the build or host machines
  objects                      : list[extracted_obj | file | str]                                   # List of object files that should be linked in this target
  override_options             : list[str] | dict[str | bool | int | list[str]]                       # takes an array of strings in the same format as `project`'s `default_options`
  pie                          : bool          # Build a position-independent executable
  rust_crate_type              : str           # Set the specific type of rust crate to compile (when compiling rust)
  rust_dependency_map          : dict[str]     # On rust targets this provides a map of library names to the crate name
  sources                      : str | file | custom_tgt | custom_idx | generated_list | structured_src  # Additional source files
  vala_args                    : list[str | file]                                                   # Compiler flags for Vala
  vs_module_defs               : str | file | custom_tgt | custom_idx                               # Specify a Microsoft module definition file for controlling symbol exports,
  win_subsystem                : str           # Specifies the subsystem type to use
)
```

## 3 编译器和连接器参数

- **c_args**: 比如我们使用的是`c`语言，那么`<lang>_args`就是`c_args`，`c_args`表示传入编译器（比如GCC）的相关参数。

- **link_args**: 表示传入链接器（ld）的相关参数。


  ```python
  project('my_project', 'c', version: '1.0')

  cc = meson.get_compiler('c')

  compile_args = ['-DCRT_SECURE_NO_DEPRECATE="sss"', '-DCRT_NONSTDC_NO_DEPRECATE']

  # 也可以直接使用字符串赋值给link_args，使用编译器具备检测功能
  link_args = cc.get_supported_link_arguments(
    '-Wl,--rpath,/usr/local:/usr',
    '-Wl,--rpath,/usr',
    '-Wl,--warn-common'
  )

  executable('demo', 'main.c',
            c_args: compile_args,
            link_args: link_args,
            build_rpath: '/test/path'
  )

  ```

  <font color="red">也可以通过 build_rpath 指定运行时动态库搜索路径</font>

  ![alt text](/assets/BuildSystem/Meson/12_Executable/image/image.png)

  `--rpath` 和 `-rpath` 区别：`--rpath`是链接器`ld`的一个选项，而`-rpath`是编译器`gcc`的选项，但两者的作用都是指定运行时库的搜索路径。


## 4 executable()举例

### 4.1 示例一

```python
app2_resources = gnome.compile_resources('exampleapp2_resources',
  'exampleapp.gresource.xml',
  source_dir: '.')

executable('exampleapp2',
  'exampleapp.c', 'exampleappwin.c', 'main.c', app2_resources,
  dependencies: libgtk_dep,
  c_args: common_cflags)
```

### 4.2 示例二

```python
project('simple', 'c')
src = ['source1.c', 'source2.c', 'source3.c']
executable('myexe', src)
```


### 4.3 示例三

```python
executable(`test_name`, [extra_sources, file_name],
      dependencies: common_deps + extra_deps,
    )
# 或者 ，因为 dependencies 要求的变量类型是列表lib，所有可以有两种方式
# common_deps + extra_deps == [common_deps, extra_deps]
executable(`test_name`, [extra_sources, file_name],
      dependencies: [common_deps, extra_deps],
    )
```


### 4.4 示例四

```python
executable(exe_name,
      src_file,
      install: true,
      install_tag: 'bin',
      include_directories : [configinc],
      dependencies : [glib_dep, gobject_dep, gmodule_dep, mathlib, gst_dep] + extra_deps,
      c_args: gst_c_args + extra_c_args + ['-DG_LOG_DOMAIN="@0@"'.format(exe_name)],
    )
```

