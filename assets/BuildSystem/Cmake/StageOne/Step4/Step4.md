# Step 4: Adding Generator Expressions

生成器表达式（Generator Expressions）在生成构建系统时进行评估，以生成特定于每个构建配置的信息。

生成器表达式（Generator Expressions）可以用在许多目标属性的上下文中，如LINK_LIBRARIES、INCLUDE_DIRECTORIES、COMPILE_DEFINITIONS等。它们也可以在使用命令来填充这些属性时使用，比如target_link_libraries()、target_include_directories()、target_compile_definitions()等。

生成器表达式（Generator Expressions）可以用于启用条件化的链接、在编译时使用的条件化定义、条件化的包含目录等。条件可以基于构建配置、目标属性、平台信息或任何其他可查询的信息。

有不同类型的生成器表达式（Generator Expressions），包括逻辑表达式、信息表达式和输出表达式。

逻辑表达式用于创建条件化的输出。基本的表达式是0和1表达式。$<0:...> 表达式结果为空字符串，<1:...> 结果为...的内容。它们也可以嵌套使用。

## 1 Exercise 1 - Adding Compiler Warning Flags with Generator Expressions

生成器表达式的常见用途之一是有条件地添加编译器标志，例如语言级别或警告标志。一种不错的模式是将这些信息关联到一个INTERFACE目标，从而允许这些信息传播到依赖它的其他目标。

### 1.1 Goal

在构建时添加编译器警告标志，但不针对已安装的版本。

### 1.2 Getting Started

打开Step4/CMakeLists.txt文件，并完成TODO 1到TODO 4。

首先，在顶层的CMakeLists.txt文件中，我们需要将cmake_minimum_required()设置为3.15。在这个练习中，我们将使用在CMake 3.15中引入的生成器表达式。

接下来，我们需要添加我们项目中所需的编译器警告标志。由于警告标志根据编译器而异，我们使用COMPILE_LANG_AND_ID生成器表达式来控制在给定语言和一组编译器标识的情况下应用哪些标志。

### 1.3 Solution

将cmake_minimum_required()更新为至少需要CMake版本3.15：

```cmake
# TODO 1: CMakeLists.txt
cmake_minimum_required(VERSION 3.15)
```

接下来，我们需要确定我们的系统当前使用的编译器来构建，因为警告标志根据所使用的编译器而异。这可以通过使用COMPILE_LANG_AND_ID生成器表达式来完成。我们将结果设置在变量gcc_like_cxx和msvc_cxx中，如下所示：

```cmake
# TODO 2: CMakeLists.txt
set(gcc_like_cxx "$<COMPILE_LANG_AND_ID:CXX,ARMClang,AppleClang,Clang,GNU,LCC>")
set(msvc_cxx "$<COMPILE_LANG_AND_ID:CXX,MSVC>")
```

接下来，我们需要添加我们项目中所需的编译器警告标志。使用我们的变量gcc_like_cxx和msvc_cxx，我们可以使用另一个生成器表达式，仅在这些变量为true时应用相应的标志。我们使用target_compile_options()来将这些标志应用于我们的接口库。

```cmake
# TODO 3: CMakeLists.txt
target_compile_options(tutorial_compiler_flags INTERFACE
  "$<${gcc_like_cxx}:-Wall;-Wextra;-Wshadow;-Wformat=2;-Wunused>"
  "$<${msvc_cxx}:-W3>"
)
```

最后，我们只希望在构建时使用这些警告标志，而安装项目的使用者不应继承我们的警告标志。为了指定这一点，我们使用BUILD_INTERFACE条件将TODO 3中的标志包装在生成器表达式中。完整的代码如下所示：

```cmake
# TODO 4: CMakeLists.txt
target_compile_options(tutorial_compiler_flags INTERFACE
  "$<${gcc_like_cxx}:$<BUILD_INTERFACE:-Wall;-Wextra;-Wshadow;-Wformat=2;-Wunused>>"
  "$<${msvc_cxx}:$<BUILD_INTERFACE:-W3>>"
)
```