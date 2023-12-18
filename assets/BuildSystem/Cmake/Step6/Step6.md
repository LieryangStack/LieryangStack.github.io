# Step 6: Adding Support for a Testing Dashboard

添加支持将我们的测试结果提交到dashboard很简单。我们已经在测试支持中为项目定义了许多测试。现在，我们只需运行这些测试并将它们提交到CDash。

## 1 Exercise 1 - Send Results to a Testing Dashboard

### 1.1 Goal

显示我们的CTest结果用CDash

### 1.2 Getting Started

在这个练习中，通过在顶层CMakeLists.txt中完成TODO 1来包含CTest模块。这将启用使用CTest进行测试以及将测试结果提交到CDash的功能，因此我们可以安全地删除enable_testing()的调用。

此外，我们还需要获取一个名为CTestConfig.cmake的文件，将其放置在顶层目录中。当运行时，ctest可执行文件将读取此文件以获取有关testing dashboard的信息。该文件包含：

- 项目名字
  
- 项目“Nightly”开始时间（用于该项目的24小时“日”开始的时间）

- 生成文档将被发送到的CDash实例的URL

在本教程中，我们使用了一个dashboard服务器，并在此根目录为您提供了相应的CTestConfig.cmake文件。在实际应用中，该文件将从打算托管测试结果的CDash实例的“设置”页面下载。一旦从CDash下载，不应在本地进行修改。


```cmake
# CTestConfig.cmake
set(CTEST_PROJECT_NAME "CMakeTutorial")
set(CTEST_NIGHTLY_START_TIME "00:00:00 EST")

set(CTEST_DROP_METHOD "http")
set(CTEST_DROP_SITE "my.cdash.org")
set(CTEST_DROP_LOCATION "/submit.php?project=CMakeTutorial")
set(CTEST_DROP_SITE_CDASH TRUE)
```

### 1.3 Build and Run

请注意，作为CDash提交的一部分，关于您的开发系统的一些信息（例如站点名称或完整路径名）可能会公开显示。

要创建一个简单的测试仪表板，请运行cmake可执行文件或cmake-gui来配置项目，但不要构建它。相反，导航到构建目录并运行：

```bash
ctest [-VV] -D Experimental
```

请注意，对于多配置生成器（例如Visual Studio），必须指定配置类型：

```bash
ctest [-VV] -C Debug -D Experimental
```

或者，从IDE中构建Experimental目标。

ctest可执行文件将构建项目，运行任何测试，并将结果提交到Kitware的公共仪表板：[https://my.cdash.org/index.php?project=CMakeTutorial](https://my.cdash.org/index.php?project=CMakeTutorial)

### 1.4 Solution

在这一步中，唯一需要更改的CMake代码是通过在我们的顶级CMakeLists.txt中包含CTest模块来启用向CDash提交dashboard。

```cmake
include(CTest)
```