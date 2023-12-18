# 一、Using Meson

Meson的设计目标是尽可能简单易用。本页面概述了安装、故障排除和标准用法的初始步骤。

如需进行更高级的配置，请参考命令行帮助 meson --help 或位于Mesonbuild网站上的Meson文档。

## 1 Requirements

- [Python 3](https://www.python.org/)
- [Ninja](https://github.com/ninja-build/ninja/)

只有在使用Ninja后端时才需要Ninja。Meson还可以生成本机的Visual Studio（VS）和Xcode项目文件。

## 2 Installation using package manager

**Ubuntu**:
```bash
sudo apt-get install python3 python3-pip python3-setuptools \
                       python3-wheel ninja-build
```

由于我们频繁的发布周期和开发速度，Linux发行版打包的软件可能会很快变得过时。

## 3 Installation using Python

Requirements: pip3

The best way to receive the most up-to-date version of Mesonbuild.

Install as a local user (recommended):

```bash
pip3 install --user meson
```
Install as root:

```bash
sudo pip3 install meson
```
## 4 Installation from source

Requirements: git

Meson can be run directly from the cloned git repository.

```bash
git clone https://github.com/mesonbuild/meson.git /path/to/sourcedir
```

## 5 Troubleshooting

Common Issues:

```bash
meson setup builddir
bash: /usr/bin/meson: No such file or directory
```
描述：Python pip模块安装的默认安装前缀未包含在您的shell环境PATH中。Python pip安装模块的默认前缀位于/usr/local下。

解决方法：可以通过更改默认的shell环境PATH来解决此问题，以包括/usr/local/bin。

注意：还有其他解决此问题的方法，如使用符号链接或将二进制文件复制到默认路径，但这些方法不建议也不受支持，因为它们可能破坏软件包管理的互操作性。

## 6 Compiling a Meson project

Meson的最常见用例是在您正在开发的代码库上编译代码。采取的步骤非常简单。

```bash
cd /path/to/source/root
meson setup builddir && cd builddir
meson compile
meson test
```

需要注意的是，您需要创建一个单独的构建目录。Meson不允许在源树内构建源代码。所有构建产物都存储在构建目录中。这使您可以同时拥有多个具有不同配置的构建树。这样生成的文件不会意外添加到版本控制中。

要在更改代码后重新编译，只需输入 meson compile。构建命令始终相同。您可以对源代码和构建系统文件进行任意更改，Meson会检测到这些更改并执行正确的操作。如果要构建优化的二进制文件，只需在运行 Meson 时使用参数 --buildtype=debugoptimized。建议您保留一个用于非优化构建的构建目录，另一个用于优化构建。要编译任何给定的配置，只需进入相应的构建目录并运行 meson compile。

Meson会自动添加编译器标志以启用调试信息和编译器警告（例如 -g 和 -Wall）。这意味着用户无需处理它们，而可以专注于编码。

## 7 Using Meson as a distro packager

Linux发行版的打包者通常希望完全控制使用的构建标志。Meson原生支持这种用例。以下是构建和安装Meson项目所需的命令：

```bash
cd /path/to/source/root
meson --prefix /usr --buildtype=plain builddir -Dc_args=... -Dcpp_args=... -Dc_link_args=... -Dcpp_link_args=...
meson compile -C builddir
meson test -C builddir
DESTDIR=/path/to/staging/root meson install -C builddir
```
--buildtype=plain命令行开关告诉Meson不向命令行添加自己的标志，使打包者完全控制使用的标志。

这种方法与其他构建系统非常相似。唯一的区别是DESTDIR变量作为环境变量传递，而不作为meson install的参数。

由于Linux发行版的构建始终从头开始，您可能考虑在您的软件包中启用统一构建，因为它们更快并生成更高质量的代码。但是，一些项目可能不支持启用统一构建，因此是否使用统一构建应由打包者根据具体情况决定。

## 参考
[翻译自：Using Meson](https://mesonbuild.com/Quick-guide.html#using-meson)