---
layout: post
title: 二十四、QML——ShaderEffect
categories: QML
tags: [QML]
---

在 `Qt5` 中，效果是以 `GLSL` （OpenGL着色语言）源码的形式提供的，通常以字符串的形式嵌入到QML中。从 Qt 5.8 开始，也可以引用文件，无论是本地文件还是 Qt 资源系统中的文件。

在 Qt 6 中，`Qt Quick` 还支持诸如 `Vulkan`、`Metal` 和 `Direct3D 11` 等图形 API。因此，使用 GLSL 源字符串不再可行。会把 `GLSL` 源字符串转换成相应的着色语言。生成的资源会打包成一个单一的包，通常存储在扩展名为 .qsb 的文件中。这个过程是在离线或最迟在应用程序构建时完成的。在运行时，场景图和底层图形抽象会使用这些 .qsb 文件。因此，在 Qt 6 中，`ShaderEffect` 需要引用文件（本地或 qrc文件），而不是内联的着色器代码。


## 1 着色输入

以下 `in` （输入）是预先定义的：

- **vec4 qt_Vertex(location 0)**：顶点位置，左上角顶点的位置为（0,0），右下角的位置为（width，height）。

- **vec2 qt_MultiTexCoord0（location 1）**：纹理坐标，左上角坐标为 (0, 0)，右下角坐标为 (1, 1)。如果 supportsAtlasTextures 为 true，则坐标基于纹理图集中的位置。

以下 `uniform`（统一变量）是预定义的：

- **mat4 qt_Matrix** - 组合变换矩阵，从根item到该 ShaderEffect 的矩阵乘积，以及一个正交投影矩阵。

- **float qt_Opacity** - 组合不透明度，从根item到该 ShaderEffect 的不透明度乘积。


