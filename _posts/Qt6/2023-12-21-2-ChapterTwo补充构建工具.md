---
layout: post
title: 第2章 补充-构建工具
categories: Qt6开发学习
tags: [Qt6开发学习]
---

如果使用qmake构建系统，就会生成后缀 `.pro` 的项目配置文件。

qmake工具有助于简化跨不同平台的开发项目的构建过程。它自动化生成Makefile的过程，因此只需要几行信息就能创建每一个Makefile。即使软件项目不是用Qt编写的，你也可以使用qmake。

qmake基于项目文件中的信息生成Makefile。项目文件由开发者创建，通常很简单，但对于复杂的项目，也可以创建更复杂的项目文件。

qmake包含了支持Qt开发的附加功能，能够自动包含元对象编译器（meta-object compiler，MOC）和用户界面编译器（user interface compiler，UIC）生成构建规则。

qmake还能为Microsoft Visual Studio生成项目，而无需开发者更改项目文件。

## 1 概述

qmake工具为您提供了一个以项目为中心的系统，用于管理应用程序、库和其他组件的构建过程。这种方法使您可以控制使用的源文件，并允许以简洁的方式描述过程中的每个步骤，通常在单个文件内完成。qmake将每个项目文件中的信息扩展为一个Makefile，该Makefile执行编译和链接所需的命令。

### 1.1 描述项目

项目通过项目文件（.pro 文件）的内容来描述。qmake使用这些文件中的信息生成Makefile，这些Makefile包含了构建每个项目所需的所有命令。项目文件通常包含源文件和头文件的列表、一般配置信息，以及任何特定于应用程序的细节，例如链接的额外库列表或使用的额外包含路径列表。

项目文件可以包含多种不同元素，包括注释、变量声明、内置函数和一些简单的控制结构。在大多数简单项目中，只需声明用于构建项目的源文件和头文件以及一些基本配置选项。有关如何创建简单项目文件的更多信息，请参阅 qmake 入门。

您可以为复杂项目创建更复杂的项目文件。有关项目文件的概述，请参阅创建项目文件。有关您可以在项目文件中使用的变量和函数的详细信息，请参阅参考资料。

您可以使用应用程序或库项目模板来指定专门的配置选项，以微调构建过程。有关更多信息，请参阅构建常见项目类型。

您可以使用 Qt Creator 新项目向导来创建项目文件。您选择项目模板，Qt Creator 会创建一个具有默认值的项目文件，使您能够构建并运行项目。您可以修改项目文件以适应您的目的。

您还可以使用 qmake 生成项目文件。有关 qmake 命令行选项的完整描述，请参阅运行 qmake。

qmake的基本配置功能可以处理大多数跨平台项目。然而，使用一些特定于平台的变量可能是有用的，甚至是必要的。有关更多信息，请参阅平台说明。

### 1.2 建造项目

对于简单的项目，您只需要在项目的顶层目录中运行qmake来生成Makefile。然后，您可以运行您平台的make工具根据Makefile构建项目。

有关qmake在配置构建过程时使用的环境变量的更多信息，请参见配置qmake。

注意：将您的项目构建目录添加到您系统上任何防病毒应用的排除目录列表中。

### 1.3 使用第三方库

第三方库指南向您展示了如何在Qt项目中使用简单的第三方库。

### 1.4 预编译头文件

在大型项目中，可以利用预编译头文件来加速构建过程。有关更多信息，请参见使用预编译头文件。

## 2 qmake入门

这个教程教您qmake的基础知识。这份手册中的其他主题包含了关于使用qmake的更详细信息。

### 2.1 从简单开始

假设您刚完成了应用程序的基本实现，并且创建了以下文件：

- hello.cpp
- hello.h
- main.cpp

您会在Qt发行版的examples/qmake/tutorial目录中找到这些文件。关于应用程序的设置，您只知道它是用Qt编写的。首先，使用您喜欢的纯文本编辑器，在examples/qmake/tutorial中创建一个名为hello.pro的文件。您需要做的第一件事是添加告诉qmake您的开发项目中包含哪些源文件和头文件的行。

我们首先向项目文件添加源文件。为此，您需要使用SOURCES变量。只需以SOURCES += 开始新行并在其后添加hello.cpp。您应该得到类似以下内容：

```qmake
SOURCES += hello.cpp
```

对于项目中的每个源文件重复此操作，直到得到以下内容：

```qmake
SOURCES += hello.cpp
SOURCES += main.cpp
```

如果您更喜欢使用类似Make的语法，可以使用换行转义一次列出所有文件，如下所示：

```qmake
SOURCES = hello.cpp \
          main.cpp
```

现在源文件已列在项目文件中，必须添加头文件。这些以与源文件完全相同的方式添加，只是我们使用的变量名是HEADERS。

完成这些后，您的项目文件应如下所示：

```qmake
HEADERS += hello.h
SOURCES += hello.cpp
SOURCES += main.cpp
```

目标名称是自动设置的。它与项目文件名相同，但后缀适用于平台。例如，如果项目文件名为hello.pro，则目标在Windows上为hello.exe，在Unix上为hello。如果您想使用不同的名称，可以在项目文件中设置：

```qmake
TARGET = helloworld
```

完成的项目文件应如下所示：

```c
HEADERS += hello.h
SOURCES += hello.cpp
SOURCES += main.cpp
```

现在您可以使用qmake为您的应用程序生成Makefile。在命令行中，您的项目目录中，键入以下内容：

```c
qmake -o Makefile hello.pro
```

注意：如果您通过包管理器安装Qt，二进制文件可能是qmake6。

然后根据您使用的编译器键入make或nmake。

对于Visual Studio用户，qmake还可以生成Visual Studio项目文件。例如：

```sh
qmake -tp vc hello.pro
```

注意：如果您通过包管理器安装Qt，二进制文件可能是qmake6。

然后根据您使用的编译器键入make或nmake。

对于Visual Studio用户，qmake还可以生成Visual Studio项目文件。例如：

```qmake
qmake -tp vc hello.pro
```

### 2.2 使应用程序可调试
应用程序的发布版本不包含任何调试符号或其他调试信息。在开发过程中，生成带有相关信息的应用程序的调试版本是有用的。这可以通过在项目文件中的CONFIG变量中添加debug轻松实现。

例如：
```qmake
CONFIG += debug
HEADERS += hello.h
SOURCES += hello.cpp
SOURCES += main.cpp
```

像之前一样使用qmake生成Makefile。现在，在调试环境中运行应用程序时，您将获得有关应用程序的有用信息。

### 2.3 添加平台特定的源文件
经过几个小时的编码后，您可能已开始编写应用程序的平台特定部分，并决定将平台依赖的代码分开。因此，您现在有两个新文件要包含在您的项目文件中：hellowin.cpp和hellounix.cpp。我们不能只是将这些添加到SOURCES变量中，因为这会将两个文件都放入Makefile中。因此，我们需要做的是使用一个范围，这将根据我们为哪个平台构建而处理。

为Windows添加平台依赖文件的简单范围如下所示：

```qmake
win32 {
    SOURCES += hellowin.cpp
}
```

在为Windows构建时，qmake将hellowin.cpp添加到源文件列表中。在为任何其他平台构建时，qmake简单地忽略它。现在剩下的就是为Unix特定文件创建一个范围。

完成后，您的项目文件应如下所示：

```qmake
CONFIG += debug
HEADERS += hello.h
SOURCES += hello.cpp
SOURCES += main.cpp
win32 {
    SOURCES += hellowin.cpp
}
unix {
    SOURCES += hellounix.cpp
}
```

像之前一样使用qmake生成Makefile。

### 2.4 如果文件不存在则停止qmake

如果某个文件不存在，您可能不想创建Makefile。我们可以使用exists()函数来检查文件是否存在。我们可以使用error()函数来停止qmake的处理。这与范围的工作方式相同。只需用函数替换范围条件即可。检查名为main.cpp的文件的范围如下所示：

```qmake
!exists( main.cpp ) {
    error( "No main.cpp file found" )
}
```

`!` 符号用于否定测试。也就是说，exists( main.cpp )在文件存在时为真，!exists( main.cpp )在文件不存在时为真。

```qmake
CONFIG += debug
HEADERS += hello.h
SOURCES += hello.cpp
SOURCES += main.cpp
win32 {
    SOURCES += hellowin.cpp
}
unix {
    SOURCES += hellounix.cpp
}
!exists( main.cpp ) {
    error( "No main.cpp file found" )
}
```

像之前一样使用qmake生成makefile。如果您暂时重命名main.cpp，您会看到消息，并且qmake将停止处理。

### 2.5 检查多个条件

假设您使用Windows，并且希望在命令行上运行应用程序时能够看到qDebug()的语句输出。要看到输出，您必须使用适当的控制台设置构建应用程序。我们可以轻松地将console放在CONFIG行上，以在Windows上将此设置包含在Makefile中。然而，假设我们只想在运行Windows且CONFIG行上已经有debug时添加CONFIG行。这需要使用两个嵌套的范围。首先创建一个范围，然后在其内部创建另一个。将要处理的设置放在第二个范围内，如下所示：

```qmake
win32 {
    debug {
        CONFIG += console
    }
}
```

嵌套范围可以使用冒号连接在一起，因此最终的项目文件看起来像这样：

就这样！您现在已经完成了qmake教程，并准备好为您的开发项目编写项目文件。

## 参考

[1. qmake Manual: Overview](https://doc.qt.io/qt-6/qmake-overview.html)