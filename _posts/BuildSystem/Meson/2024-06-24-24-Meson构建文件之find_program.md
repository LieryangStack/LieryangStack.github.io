---
layout: post
title: 二十四、Meson构建文件函数——find_program()
categories: Meson
tags: [Meson]
---

```python
# `program_name` here is a string that can be an executable or script
external_program find_program(
  str | file program_name,     # The name of the program to search, or a file object to be used
  str | file fallback...,      # These parameters are used as fallback names to search for

  # Keyword arguments:
  default_options  : list[str] | dict[str | bool | int | list[str]]  # An array of default option values
  dirs             : list[str]                                     # extra list of absolute paths where to look for program names
  disabler         : bool                                          # If `true` and the program couldn't be found, return a disabler object
  native           : bool                                          # Defines how this executable should be searched
  required         : bool | feature                                # When `true`, Meson will abort if no program can be found
  version          : str | list[str]                               # Specifies the required version, see
  version_argument : str                                           # Specifies the argument to pass when trying to find the version of the program
)
```

- **fallback**: 这跟`dependency`中的一样，这里就不详细记录了。