# Step 7: Adding System Introspection

让我们考虑向我们的项目添加一些依赖于目标平台可能没有的功能的代码。在这个示例中，我们将添加一些依赖于目标平台是否具有对数（log）和指数（exp）函数的代码。当然，几乎每个平台都具有这些功能，但在本教程中假设它们不常见。

## 1 Exercise 1 - Assessing Dependency Availability

### 1 Goal

根据可用的系统依赖项来更改实现。

### 1.1 Getting Started

起始源代码已经提供在Step7目录中。在这个练习中，完成TODO 1到TODO 5。

首先编辑MathFunctions/CMakeLists.txt。包含CheckCXXSourceCompiles模块。然后，使用check_cxx_source_compiles来确定cmath中是否可用log和exp。如果它们可用，使用target_compile_definitions()来指定HAVE_LOG和HAVE_EXP作为编译定义。

在MathFunctions/mysqrt.cxx中，包含cmath。然后，如果系统有log和exp，使用它们来计算平方根。

### 1.2 Solution
