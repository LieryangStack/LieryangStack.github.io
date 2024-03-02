# Step 2: Adding a Library

在这一点上，我们已经看到了如何使用CMake创建一个基本项目。在这一步中，我们将学习如何在项目中创建和使用库。我们还将看到如何使库的使用成为可选的。

## Exercise 1 - Creating a Library

要在CMake中添加一个库，使用add_library()命令并指定哪些源文件应该组成该库。

与将所有源文件放在一个目录中不同，我们可以使用一个或多个子目录来组织项目。在这种情况下，我们将为我们的库创建一个子目录。在这里，我们可以添加一个新的CMakeLists.txt文件和一个或多个源文件。在顶层CMakeLists.txt文件中，我们将使用add_subdirectory()命令将子目录添加到构建中。

一旦库被创建，就可以使用target_include_directories()和target_link_libraries()将其连接到我们的可执行目标中。

### 1.1 Goal

增加，然后使用一个库

### 1.2 Getting Started

在这个练习中，我们将向我们的项目中添加一个库，其中包含我们自己的计算平方根的实现。可执行文件可以使用这个库而不是编译器提供的标准平方根函数。

在本教程中，我们将把这个库放到一个名为MathFunctions的子目录中。该目录已经包含了头文件MathFunctions.h和mysqrt.h。它们各自的源文件MathFunctions.cxx和mysqrt.cxx也已提供。我们不需要修改这些文件中的任何一个。mysqrt.cxx有一个名为mysqrt的函数，提供了类似于编译器的sqrt函数的功能。MathFunctions.cxx包含一个名为sqrt的函数，用于隐藏sqrt的实现细节。

从Help/guide/tutorial/Step2目录开始，从TODO 1开始，一直完成到TODO 6。

首先，在MathFunctions子目录中填写一行CMakeLists.txt。

接下来，编辑顶级CMakeLists.txt。

最后，在tutorial.cxx中使用新创建的MathFunctions库。

### 1.3 Solution
