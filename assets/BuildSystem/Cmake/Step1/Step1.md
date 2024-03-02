# Step 1: A Basic Starting Point

我应该从哪里开始学习CMake？这一步将介绍CMake的基本语法、命令和变量。随着这些概念的介绍，我们将完成三个练习并创建一个简单的CMake项目。

这一步中的每个练习都将以一些背景信息开始。然后，提供一个目标和有用资源的列表。在“要编辑的文件”部分中的每个文件都位于Step1目录中，包含一个或多个TODO注释。每个TODO代表要更改或添加的一两行代码。TODO应按照数字顺序完成，首先完成TODO 1，然后再完成TODO 2，以此类推。入门指南部分将提供一些有用的提示，并引导您完成练习。然后，“构建和运行”部分将逐步介绍如何构建和测试练习。最后，在每个练习结束时，将讨论预期的解决方案。

还请注意，教程中的每一步都是基于下一步构建的。例如，Step2的起始代码是Step1的完整解决方案。

## Exercise 1 - Building a Basic Project

最基本的CMake项目是通过单个源代码文件构建的可执行文件。对于这样简单的项目，只需一个包含三个命令的CMakeLists.txt文件即可。

注意：虽然CMake支持大写、小写和大小写混合的命令，但推荐使用小写命令，并将在整个教程中使用小写命令。

任何项目的顶层CMakeLists.txt必须通过使用cmake_minimum_required()命令指定最低CMake版本。这会建立策略设置，并确保以下CMake函数在兼容的CMake版本上运行。

为了启动一个项目，我们使用project()命令来设置项目名称。这个调用在每个项目中都是必需的，并应该在cmake_minimum_required()之后尽快调用。正如我们将在后面看到的，这个命令也可以用于指定其他项目级别的信息，如语言或版本号。

最后，add_executable()命令告诉CMake使用指定的源代码文件创建一个可执行文件。

### 1.1 Goal

了解如何创建一个简单的CMake项目

### 1.2 Getting Started

tutorial.cxx的源代码位于Help/guide/tutorial/Step1目录中，可以用于计算一个数字的平方根。在这一步中，无需编辑这个文件。

在相同的目录中有一个CMakeLists.txt文件，您将完成它。从TODO 1开始，依次完成TODO 3。

### 1.3 Build and Run

完成了 TODO 1 到 TODO 3 后，我们就准备好构建和运行项目了！首先，运行cmake可执行文件或者使用cmake-gui来配置项目，然后使用你选择的构建工具构建项目。

例如，从命令行，我们可以导航到CMake源代码树的Step1目录，并创建一个构建目录：

```bash
mkdir Step1_build
```

接下来，导航到那个构建目录，并运行cmake来配置项目并生成本地的构建系统：

```bash
cd Step1_build
cmake ../Step1
```

然后调用该构建系统来实际编译/链接项目：

```bash
cmake --build .
```

最后，尝试使用以下命令来使用新构建的Tutorial：

```bash
Tutorial 4294967296
Tutorial 10
Tutorial
```

### 1.4 Solution

如上所述，我们只需要一个三行的CMakeLists.txt文件就可以开始运行了。第一行是使用cmake_minimum_required()来设置CMake版本，如下所示：

```Cmake
# TODO 1: CMakeLists.txt
cmake_minimum_required(VERSION 3.10)
```

制作一个基本项目的下一步是使用project()命令来设置项目名称，如下所示：

```Cmake
# TODO 2: CMakeLists.txt
project(Tutorial)
```

为一个基本项目调用的最后一个命令是add_executable()。我们可以按照以下方式调用它：

```Cmake
# TODO 3: CMakeLists.txt
add_executable(Tutorial tutorial.cxx)
```

## Exercise 2 - Specifying the C++ Standard

CMake具有一些特殊的变量，这些变量要么在幕后自动创建，要么在项目代码中设置时对CMake具有特殊意义。其中许多变量以CMAKE_开头。在创建项目变量时，应避免使用这种命名约定。其中两个特殊的可由用户设置的变量是CMAKE_CXX_STANDARD和CMAKE_CXX_STANDARD_REQUIRED。它们可以一起使用，以指定构建项目所需的C++标准。

### 2.1 Goal

添加一个需要C++11的特性

### 2.2 Getting Started

继续编辑Step1目录中的文件。从TODO 4开始，一直完成到TODO 6。

首先，通过添加一个需要C++11的特性来编辑tutorial.cxx。然后，更新CMakeLists.txt以要求C++11。

### 2.3 Solution

我们首先通过在tutorial.cxx中将atof替换为std::stod来向我们的项目添加一些C++11特性。代码如下所示：

```Cmake
# TODO 4: tutorial.cxx
const double inputValue = std::stod(argv[1]);
```

要完成TODO 5，只需删除#include <cstdlib>。

我们需要在CMake代码中明确指定它应该使用正确的标志。在CMake中启用对特定C++标准的支持的一种方法是使用CMAKE_CXX_STANDARD变量。对于本教程，在CMakeLists.txt文件中将CMAKE_CXX_STANDARD变量设置为11，并将CMAKE_CXX_STANDARD_REQUIRED设置为True。确保在调用add_executable()之前添加CMAKE_CXX_STANDARD声明。

```Cmake
# TODO 6: CMakeLists.txt
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
```

## Exercise 3 - Adding a Version Number and Configured Header File

有时，在CMakeLists.txt文件中定义的变量也需要在源代码中可用。在这种情况下，我们希望打印项目的版本号。

一种实现这一目标的方法是使用一个配置好的头文件。我们创建一个输入文件，其中包含一个或多个要替换的变量。这些变量具有特殊的语法，看起来像 `@VAR@` 。然后，我们使用configure_file()命令将输入文件复制到给定的输出文件，并将这些变量替换为CMakeLists.txt文件中VAR的当前值。

虽然我们可以直接在源代码中编辑版本号，但使用这个功能是首选的，因为它创建了一个单一的真相来源，避免了重复。

### 3.1 Goal

定义并报告项目的版本号。

### 3.2 Getting Started

继续编辑Step1目录中的文件。从TODO 7开始，一直完成到TODO 12。在这个练习中，首先在CMakeLists.txt中添加一个项目版本号。在同一个文件中，使用configure_file()将一个给定的输入文件复制到一个输出文件，并替换输入文件内容中的一些变量值。

接下来，创建一个名为TutorialConfig.h.in的输入头文件，定义接受由configure_file()传递的变量值的版本号。

最后，更新tutorial.cxx以打印出它的版本号。

### 3.3 Solution

在这个练习中，我们通过打印版本号来改进我们的可执行文件。虽然我们可以在源代码中完全做到这一点，但使用CMakeLists.txt可以让我们维护版本号的单一数据来源。

首先，我们修改CMakeLists.txt文件，使用project()命令设置项目名称和版本号。当调用project()命令时，CMake会在幕后定义Tutorial_VERSION_MAJOR和Tutorial_VERSION_MINOR。

```Cmake
# TODO 7: CMakeLists.txt
project(Tutorial VERSION 1.0)
```

然后我们使用configure_file()复制输入文件，替换指定的CMake变量:

```Cmake
# TODO 8: CMakeLists.txt
configure_file(TutorialConfig.h.in TutorialConfig.h)
```

由于配置的文件将被写入项目的二进制目录，因此我们必须将该目录添加到用于搜索包含文件的路径列表中。

注意：在整个教程中，我们将项目构建目录和项目二进制目录互换使用。它们是相同的，不是指bin/目录。

我们使用了target_include_directories()来指定可执行目标应该查找包含文件的位置。

```Cmake
# TODO 9: CMakeLists.txt
target_include_directories(Tutorial PUBLIC
                           "${PROJECT_BINARY_DIR}"
                           )
```

TutorialConfig.h.in是要进行配置的输入头文件。当我们在CMakeLists.txt中调用configure_file()时，@Tutorial_VERSION_MAJOR@和@Tutorial_VERSION_MINOR@的值将被替换为TutorialConfig.h中项目的相应版本号。

```c
// TODO 10: TutorialConfig.h.in
// the configured options and settings for Tutorial
#define Tutorial_VERSION_MAJOR @Tutorial_VERSION_MAJOR@
#define Tutorial_VERSION_MINOR @Tutorial_VERSION_MINOR@

```

接下来，我们需要修改tutorial.cxx以包括配置的头文件TutorialConfig.h。

```Cmake
# TODO 11: tutorial.cxx
#include "TutorialConfig.h"
```

最后，我们通过更新tutorial.cxx来打印出可执行文件的名称和版本号，如下所示：

```Cmake
# TODO 12 : tutorial.cxx
  if (argc < 2) {
    // report version
    std::cout << argv[0] << " Version " << Tutorial_VERSION_MAJOR << "."
              << Tutorial_VERSION_MINOR << std::endl;
    std::cout << "Usage: " << argv[0] << " number" << std::endl;
    return 1;
  }
```

## 参考

[Step 1: A Basic Starting Point](https://cmake.org/cmake/help/latest/guide/tutorial/A%20Basic%20Starting%20Point.html)