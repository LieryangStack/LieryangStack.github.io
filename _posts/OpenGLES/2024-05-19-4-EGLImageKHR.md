---
layout: post
title: 四、OpenGL ES——EGLImageKHR
categories: OpenGL ES
tags: [OpenGL ES]
---

这一节的内容其实讲的就是纹理，只不过 `EGLImageKHR` 是通过不同方式直接更新纹理信息。

## 1 回顾统一变量Uniform Variables

在OpenGL ES着色器编程中，uniform变量是一种特殊的全局变量，<font color="red">该变量可以在每次绘制调用之前更新</font>，它们的作用是在图形渲染管线的不同阶段（如顶点着色器、片段着色器等）共享常量数据。uniform变量的值由CPU端的应用程序设置，并且在整个渲染调用过程中保持不变，这意味着所有在这个渲染批次中的顶点或片段都将使用同一组uniform变量的值。

特性与用途：

1. 在GLSL（OpenGL Shading Language）中，uniform变量通过关键字uniform进行声明，并指定其数据类型，例如矩阵、向量、标量或者结构体。例如：

    ```c
    uniform mat4 modelMatrix; // 用于存储模型变换矩阵
    uniform vec3 lightPosition; // 存储光源位置
    uniform float time; // 表示当前时间
    ```

2. 生命周期：uniform变量的生命周期跨越整个渲染帧，在这一帧内它的值是恒定的。

3. 作用域：一个uniform变量在被链接在一起的所有着色器阶段中都是可见的，如果顶点着色器和片段着色器都引用了同一个uniform变量，则它们将访问到相同的值。

4. 赋值与更新：

    ```c
    // 获取uniform位置
    int modelLoc = glGetUniformLocation(shaderProgram, "model");
    int viewLoc = glGetUniformLocation(shaderProgram, "view");
    int projLoc = glGetUniformLocation(shaderProgram, "projection");
    int objectColorLoc = glGetUniformLocation(shaderProgram, "objectColor");
    int lightColorLoc = glGetUniformLocation(shaderProgram, "lightColor");

    // 设置uniform值
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
    glUniform3f(objectColorLoc, 1.0f, 0.5f, 0.31f);
    glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);

    ```

## 2 纹理类型

### 2.1 sampler2D

`sampler2D` 是一种用于访问二维纹理的统一变量类型。它允许在片段着色器中读取纹理数据，从而可以对像素进行纹理映射操作。

`uniform sampler2D texData` 这个统一变量和上面讲到的还不太一样，这里我们进行总结。

1. 实际上 texData 存储的是一个地址，texture对象的地址，我们一般管这个地址叫做 texture unit。而一些会给sampler类型的变量附一个默认值：0号地址。而每个texture对象的默认 texture unit都是0。<font color="red">(对应GL_TEXTURE0)</font>

2. 我们实际上可以同时绑定很多个texture对象，前提是，只要他们的texture unit不同。也由此，我们可以将多个texture对象传入shader。所以在绑定之前我们可以人为的激活不同的texture unit：

    ```c
    /* 使用uniform之前一定要先使用着色器程序对象 */
    glUseProgram(shaderProgram);
    /* 设置texData对应的纹理对象地址是 1 号地址 */
    glUniform1i(glGetUniformLocation(myShaderProg.ID, "texData"), 1);

    /* 激活和绑定1号地址的纹理 */
    glActiveTexture(GL_TEXTURE1); 
    glBindTexture(GL_TEXTURE_2D, texture);


    /* 现在我们就可以修改1号地址对应的纹理变量 texData 的内存信息了 */
    ```

    OpenGL保证了，至少有16个texture unit，并且他们的值是连续的，例如：
    ```c
    #define GL_TEXTURE0 0x84C0
    #define GL_TEXTURE1 0x84C1
    #define GL_TEXTURE2 0x84C2
    #define GL_TEXTURE3 0x84C3
    #define GL_TEXTURE4 0x84C4
    /* 至少一直到 GL_TEXTURE16 */
    ```

### 2.2 samplerExternalOES

在使用OpenGL ES进行图形图像开发时，我们常使用GL_TEXTURE_2D纹理类型，它提供了对标准2D图像的处理能力。这种纹理类型适用于大多数场景，可以用于展示静态贴图、渲染2D图形和进行图像处理等操作。
另外，有时我们需要从<font color="red">Camera或外部视频源读取数据帧</font>并进行处理。这时，我们会使用GL_TEXTURE_EXTERNAL_OES纹理类型。其专门用于对外部图像或实时视频流进行处理，可以直接从 BufferQueue 中接收的数据渲染纹理多边形，从而提供更高效的视频处理和渲染性能。

使用该部分扩展内容的时候，我们首先需要再着色器前面声明，我们需要使用该扩展：

```c
#extension GL_OES_EGL_image_external_essl3: require
```

其中定义了一个纹理的扩展类型 `GL_TEXTURE_EXTERNAL_OES` 。后面绑定纹理时需要绑定到 `GL_TEXTURE_EXTERNAL_OES` 上，而不是类型`GL_TEXTURE_2D` 上。

**个人理解：**

- `GL_TEXTURE_2D` 来源于 `glTexImage2D` 加载的二维图像数据。

- `GL_TEXTURE_EXTERNAL_OES` 来源于外部内存区域（摄像头或者视频帧）。


## 3 使用EGLImageKHR更新EXTERNAL纹理

### 3.1 eglCreateImageKHR

`eglCreateImageKHR` 是 EGL (Embedded-System Graphics Library) 中的一个函数，用于创建 EGLImage 对象。EGLImage 是一个抽象的图像对象，可以与多个图形 API (如 OpenGL ES、Vulkan 等) 共享。这使得在不同图形上下文之间传递图像数据变得更加高效。

主要作用：
1. 跨 API 图像共享：允许在 OpenGL ES、Vulkan 和其他图形 API 之间共享图像数据，而无需进行昂贵的数据复制或格式转换。

2. 视频解码：常用于视频解码场景，将解码后的帧作为 EGLImage，然后在 OpenGL ES 中进行渲染。

3. 相机预览：在摄像头应用中，捕获的图像可以直接作为 EGLImage 使用，避免额外的数据传输。

```c
/**
 * @param target: 决定了创建EGLImage的方式
 *                 
*/
EGLImageKHR 
eglCreateImageKHR (EGLDisplay dpy, 
                   EGLContext ctx, 
                   EGLenum target, 
                   EGLClientBuffer buffer, 
                   const EGLint *attrib_list);

EGLBoolean 
eglDestroyImageKHR(EGLDisplay dpy,
                  EGLImageKHR image);
```

创建EGLImage的方式如下：

```c
#define EGL_NATIVE_PIXMAP_KHR			0x30B0	/* eglCreateImageKHR target */
#define EGL_GL_TEXTURE_2D_KHR			0x30B1	/* eglCreateImageKHR target */
#define EGL_VG_PARENT_IMAGE_KHR			0x30BA	/* eglCreateImageKHR target */
#define EGL_GL_TEXTURE_3D_KHR			0x30B2	/* eglCreateImageKHR target */
#define EGL_GL_RENDERBUFFER_KHR			0x30B9	/* eglCreateImageKHR target */
#define EGL_LINUX_DMA_BUF_EXT             0x3270
#define EGL_GL_TEXTURE_CUBE_MAP_POSITIVE_X_KHR	0x30B3	/* eglCreateImageKHR target */
#define EGL_GL_TEXTURE_CUBE_MAP_NEGATIVE_X_KHR	0x30B4	/* eglCreateImageKHR target */
#define EGL_GL_TEXTURE_CUBE_MAP_POSITIVE_Y_KHR	0x30B5	/* eglCreateImageKHR target */
#define EGL_GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_KHR	0x30B6	/* eglCreateImageKHR target */
#define EGL_GL_TEXTURE_CUBE_MAP_POSITIVE_Z_KHR	0x30B7	/* eglCreateImageKHR target */
#define EGL_GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_KHR	0x30B8	/* eglCreateImageKHR target */
/* 属性 */

#define EGL_IMAGE_PRESERVED_KHR			0x30D2	/* eglCreateImageKHR attribute */
#define EGL_GL_TEXTURE_ZOFFSET_KHR		0x30BD	/* eglCreateImageKHR attribute */
```

| 属性                     | 描述                      | 有效 | 默认值         |
|-------------------------|--------------------------|------------------|---------------|
| EGL_NONE                | 标记属性-值列表的结束      | All              | N/A           |
|                         |                            |                  |               |
| EGL_IMAGE_PRESERVED_KHR | 是否保留像素数据 （暂时不理解具体作用？？）          | All              | EGL_FALSE     |
|                         |                            |                  |               |


#### 3.1.1 EGL_NATIVE_PIXMAP_KHR

这是从像素映射创建，比如通过X11的Pixmap创建EGLImageKHR

#### 3.1.2 EGL_GL_TEXTURE_2D_KHR

这是从 `GL_TEXTURE_2D` 纹理对象创建EGLImageKHR (<font color="red">只能是GL_TEXTURE_2D创建的纹理</font>)

#### 3.1.3 EGL_LINUX_DMA_BUF_EXT

这是从 DMA 内存缓冲区创建EGLImageKHR

### 3.2 glEGLImageTargetTexture2DOES

`glEGLImageTargetTexture2DOES` 是 OpenGL ES 中的一个扩展函数，用于将 EGLImage 对象绑定到 OpenGL ES 纹理目标。这使得应用程序可以直接使用外部图像资源（如视频帧、相机捕获的图像等）作为 OpenGL ES 纹理进行渲染。

主要作用：
1. 绑定 EGLImage 到 OpenGL ES 纹理：将 EGLImage 对象作为 OpenGL ES 纹理，使其能够在渲染管线中使用。

2. 高效数据共享：允许从外部来源（如视频解码器、相机）获取的图像数据直接用于 OpenGL ES 渲染，而无需额外的数据拷贝或格式转换。

3. 实现实时渲染：适用于需要实时处理和显示外部图像数据的应用，如视频播放器和增强现实（AR）应用。

## 参考

[参考1：OpenGL统一变量 Uniform Variables](https://blog.csdn.net/fwy00/article/details/136031448)

[参考2：我就说Uniform sampler2D肯定需要赋值！](https://blog.csdn.net/YYMHQE123/article/details/80548518)

[参考3：一文详解如何将 ExternalOES转换为 TEXTURE_2D纹理](https://download.csdn.net/blog/column/9284425/135137815)