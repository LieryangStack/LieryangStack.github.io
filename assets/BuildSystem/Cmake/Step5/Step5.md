# Step 5：Installing and Testing
通常，仅构建可执行文件是不够的，它还应该具备可安装性。使用CMake，我们可以使用install()命令来指定安装规则。在CMake中，为你的构建支持本地安装通常只需要指定一个安装位置以及要安装的目标和文件，这非常简单。
## 1 Exercise 1 - Install Rules

### 1.1 Goal

安装Tutorial可执行文件和MathFunctions库

### 1.2 Getting Started

在Step5目录中提供了初始代码。在这个练习中，请完成TODO 1到TODO 4。

首先，更新MathFunctions/CMakeLists.txt文件，将MathFunctions和tutorial_compiler_flags库安装到lib目录中。在同一文件中，指定安装规则以将MathFunctions.h安装到include目录中。

然后，更新顶层的CMakeLists.txt文件，将Tutorial可执行文件安装到bin目录中。最后，任何头文件都应安装到include目录中。请记住，TutorialConfig.h位于PROJECT_BINARY_DIR中。

### 1.3 Build and Run


创建一个名为Step5_build的新目录。运行cmake可执行文件或cmake-gui来配置项目，然后使用你选择的构建工具构建它。

然后，通过在命令行中使用cmake命令的--install选项（引入自CMake 3.15，较旧版本的CMake必须使用make install）来运行安装步骤。这一步将安装适当的头文件、库和可执行文件。例如：

```bash
cmake --install .
```

对于多配置工具，请不要忘记使用--config参数来指定配置。

```bash
cmake --install . --config Release
```

如果使用集成开发环境（IDE），只需构建INSTALL目标。你也可以从命令行中构建相同的install目标，如下所示：


```bash
cmake --build . --target install --config Debug
```

CMake变量CMAKE_INSTALL_PREFIX 用于确定文件将被安装到的根目录。如果使用cmake --install命令，安装前缀可以通过--prefix参数进行覆盖。例如：

```bash
cmake --install . --prefix "/home/myuser/installdir"
```

### 1.4 Solution

我们项目的安装规则相当简单：

- 对于MathFunctions，我们希望将库和头文件分别安装到lib和include目录。

- 对于Tutorial可执行文件，我们希望将可执行文件和配置的头文件分别安装到bin和include目录。

因此，在MathFunctions/CMakeLists.txt文件的末尾，我们添加如下内容：


```cmake
# TODO 1: MathFunctions/CMakeLists.txt
set(installable_libs MathFunctions tutorial_compiler_flags)
if(TARGET SqrtLibrary)
  list(APPEND installable_libs SqrtLibrary)
endif()
install(TARGETS ${installable_libs} DESTINATION lib)
```


```cmake
# TODO 2: MathFunctions/CMakeLists.tx
install(FILES MathFunctions.h DESTINATION include)
```

可执行文件Tutorial和配置的头文件的安装规则类似。在顶层CMakeLists.txt文件的末尾，我们添加如下内容：

```cmake
# CMakeLists.txt
install(TARGETS Tutorial DESTINATION bin)
install(FILES "${PROJECT_BINARY_DIR}/TutorialConfig.h"
  DESTINATION include
  )
```

## 2 Exercise 2 - Testing Support

CTest提供了一种轻松管理项目测试的方式。通过add_test()命令可以添加测试。尽管本教程没有明确介绍，但CTest与其他测试框架（如GoogleTest）之间存在很大的兼容性。

### 2.1 Goal

Create unit tests for our executable using CTest.

### 2.2 Getting Started

起始源代码已提供在 Step5 目录中。在这个练习中，通过完成 TODO 5 到 TODO 9，首先，我们需要启用测试。接下来，开始使用 add_test() 向我们的项目添加测试。我们将逐步添加三个简单的测试，然后你可以根据需要添加额外的测试。

### 2.3 Build and Run

前往构建目录并重新构建应用程序。然后运行 ctest 可执行文件：ctest -N 和 ctest -VV。对于多配置生成器（例如 Visual Studio），必须使用 `-C <mode>` 标志指定配置类型。例如，要在 Debug 模式下运行测试，请在构建目录中使用 ctest -C Debug -VV（而不是 Debug 子目录！）。Release 模式将从相同位置执行，但需要加上 -C Release。或者，可以从IDE中构建 RUN_TESTS 目标。

### 2.4 Solution

让我们来测试我们的应用程序。在顶层的 CMakeLists.txt 文件末尾，首先需要使用 enable_testing() 命令启用测试。

```cmake
# TODO 5: CMakeLists.txt
enable_testing()
```

启用测试后，我们将添加一些基本测试，以验证应用程序是否正常工作。首先，我们使用 add_test() 创建一个测试，该测试运行 Tutorial 可执行文件，参数为 25。对于这个测试，我们不会检查可执行文件的计算结果。这个测试将验证应用程序是否运行，是否没有段错误或其他崩溃，并且是否返回零值。这是 CTest 测试的基本形式。


```cmake
# TODO 6: CMakeLists.txt
add_test(NAME Runs COMMAND Tutorial 25)
```

接下来，让我们使用 PASS_REGULAR_EXPRESSION 测试属性来验证测试的输出是否包含特定的字符串。在这种情况下，验证当提供了不正确数量的参数时是否打印了使用消息。

```cmake
# TODO 7: CMakeLists.txt
add_test(NAME Usage COMMAND Tutorial)
set_tests_properties(Usage
  PROPERTIES PASS_REGULAR_EXPRESSION "Usage:.*number"
  )
```

接下来添加的测试将验证计算出的值是否确实是平方根。


```cmake
# TODO 8: CMakeLists.txt
add_test(NAME StandardUse COMMAND Tutorial 4)
set_tests_properties(StandardUse
  PROPERTIES PASS_REGULAR_EXPRESSION "4 is 2"
  )
```

 单一测试还不足以让我们对它对所有传入值的工作有信心。我们应该添加更多的测试来验证这一点。为了轻松添加更多的测试，我们创建一个名为 do_test 的函数，该函数运行应用程序并验证给定输入的计算平方根是否正确。对于 do_test 的每次调用，将根据传递的参数添加另一个项目的测试，包括名称、输入和预期结果。


```cmake
# TODO 9: CMakeLists.txt

function(do_test target arg result)
  add_test(NAME Comp${arg} COMMAND ${target} ${arg})
  set_tests_properties(Comp${arg}
    PROPERTIES PASS_REGULAR_EXPRESSION ${result}
    )
endfunction()

# do a bunch of result based tests
do_test(Tutorial 4 "4 is 2")
do_test(Tutorial 9 "9 is 3")
do_test(Tutorial 5 "5 is 2.236")
do_test(Tutorial 7 "7 is 2.645")
do_test(Tutorial 25 "25 is 5")
do_test(Tutorial -25 "-25 is (-nan|nan|0)")
do_test(Tutorial 0.0001 "0.0001 is 0.01")
```

