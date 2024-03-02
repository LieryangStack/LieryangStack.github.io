# Step 3: Adding Usage Requirements for a Library

## Exercise 1 - Adding Usage Requirements for a Library

目标参数的Usage Requirements允许更好地控制库或可执行文件的链接和包含行，同时还能更好地控制CMake中目标的传递属性。主要利用使用要求的命令包括：

- target_compile_definitions()
- target_compile_options()
- target_include_directories()
- target_link_directories()
- target_link_options()
- target_precompile_headers()
- target_sources()

### 1.1 Goal

为库添加使用要求

### 1.2 Getting Started

在这个练习中，我们将重构我们从"Adding a Library"部分的代码，使用现代CMake的方法。我们将让我们的库定义自己的使用要求，以便根据需要将它们传递给其他目标。在这种情况下，MathFunctions将自己指定所需的包含目录。然后，使用 MathFunctions 的目标对象 Tutorial 只需要链接到 MathFunctions，而不必担心任何额外的包含目录。

起始源代码已经提供在 Step3 目录中。在这个练习中，完成 TODO 1 到 TODO 3。

首先，在 MathFunctions/CMakeLists 中添加一个调用 target_include_directories()。记住，CMAKE_CURRENT_SOURCE_DIR 是当前正在处理的源目录的路径。

然后，在顶级 CMakeLists.txt 中更新（并简化！）对 target_include_directories() 的调用。

### 1.3 Build and Run

创建一个名为Step3_build的新目录，运行cmake可执行文件或者使用cmake-gui来配置项目，然后使用你选择的构建工具构建项目，或者在构建目录中使用"cmake --build ."命令构建项目。以下是在命令行中执行这些步骤的刷新说明：

```bash
mkdir Step3_build
cd Step3_build
cmake ../Step3
cmake --build .
```

接下来，使用新构建的Tutorial并验证它是否按预期工作。

### 1.4 Solution

让我们更新前一步的代码，使用现代CMake的使用要求方法。

我们希望声明任何链接到MathFunctions的人都需要包含当前源目录，而MathFunctions本身则不需要。这可以用INTERFACE使用要求来表示。记住，<span style="background-color: pink;">INTERFACE 表示**使用**该库需要的内容，但编译生成该库则不需要</span>。

在MathFunctions/CMakeLists.txt的末尾，使用target_include_directories()和INTERFACE关键字，如下所示：

```Cmake
# TODO 1: MathFunctions/CMakeLists.txt
target_include_directories(MathFunctions
                           INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
                           )
```

现在，既然我们已经为MathFunctions指定了使用要求，我们可以安全地从顶级CMakeLists.txt文件中删除对EXTRA_INCLUDES变量的使用。

删除这行：


```Cmake
# TODO 2: CMakeLists.txt
list(APPEND EXTRA_INCLUDES "${PROJECT_SOURCE_DIR}/MathFunctions")
```

同时，也从target_include_directories中移除EXTRA_INCLUDES。

```Cmake
# TODO 3: CMakeLists.txt
target_include_directories(Tutorial PUBLIC
                           "${PROJECT_BINARY_DIR}"
                           )
```

请注意，使用这种技术，我们的可执行目标所做的唯一事情就是通过调用target_link_libraries()并提供库目标的名称来使用我们的库。在更大的项目中，采用手动指定库依赖的传统方法会迅速变得非常复杂。

## Exercise 2 - Setting the C++ Standard with Interface Libraries

现在我们已经将代码转换为更现代的方法，让我们演示一种现代的方法来为多个目标设置属性。

让我们重构现有的代码以使用一个INTERFACE库。我们将在下一步中使用该库来演示生成器表达式的常见用途。


### 2.1 Goal

添加一个INTERFACE库目标来指定所需的C++标准。

### 2.2 Getting Started

在这个练习中，我们将重构我们的代码，使用一个INTERFACE库来指定C++标准。

从我们在“Step3 exercise 1”的末尾离开的地方开始这个练习。你需要完成TODO 4到TODO 7。

首先，编辑顶层的CMakeLists.txt文件。创建一个名为"tutorial_compiler_flags"的INTERFACE库目标，并将"cxx_std_11"指定为目标编译特性。

然后，修改CMakeLists.txt和MathFunctions/CMakeLists.txt，以确保所有目标都通过target_link_libraries()调用链接到"tutorial_compiler_flags"。

### 2.3 Solution

让我们更新之前步骤中的代码，使用接口库来设置我们的C++要求。

首先，我们需要删除两个关于变量CMAKE_CXX_STANDARD和CMAKE_CXX_STANDARD_REQUIRED的set()调用。需要删除的具体行如下：


```Cmake
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
```

接下来，我们需要创建一个接口库，名为"tutorial_compiler_flags"。然后，使用target_compile_features()来添加编译特性"cxx_std_11"。



```Cmake
# TODO 4: CMakeLists.txt
add_library(tutorial_compiler_flags INTERFACE)
target_compile_features(tutorial_compiler_flags INTERFACE cxx_std_11)

```

最后，使用我们设置好的接口库，我们需要将可执行文件"tutorial"、我们的"SqrtLibrary"库以及我们的"MathFunctions"库分别链接到我们的新"tutorial_compiler_flags"库。相应的代码如下：


```Cmake
# TODO 5: CMakeLists.txt
target_link_libraries(Tutorial PUBLIC MathFunctions tutorial_compiler_flags)
```



```Cmake
# TODO 6: MathFunctions/CMakeLists.txt
target_link_libraries(SqrtLibrary PUBLIC tutorial_compiler_flags)
```


```Cmake
# TODO 7: MathFunctions/CMakeLists.txt
target_link_libraries(MathFunctions PUBLIC tutorial_compiler_flags)
```

通过这样做，我们的所有代码仍然需要C++ 11才能构建。请注意，使用这种方法，我们有能力具体指定哪些目标需要特定的要求。此外，我们在接口库中创建了一个单一的源头，以确保一致性。

