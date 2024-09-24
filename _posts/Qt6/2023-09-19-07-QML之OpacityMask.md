---
layout: post
title: 七、QML——OpacityMask
categories: QML
tags: [QML]
---

## 1 OpacityMask

**OpacityMask**: 通过 `MaskSource` 提供的非透明区域跟 `Source` 区域进行与运算，保留 `MaskSource` 跟  `Source` 重叠的的非透明区域。

![alt text](image-2.png)

### 1.1 属性

#### 1.1.1 cached

是否允许缓存效果输出像素以提高渲染性能。默认为 false。

每次更改源或效果属性时，都必须更新缓存中的像素。 内存消耗增加，因为需要额外的内存缓冲区来存储效果输出。

建议在源或效果属性动画时禁用缓存。

#### 1.1.2 invert

此属性控制 sourceMask 的 alpha 值的行为方式。默认为 false。

如果此属性为 false，则生成的不透明度是源 alpha 乘以掩码 alpha，As * Am。

如果此属性为真，则生成的不透明度是源 alpha 乘以掩码 alpha 的倒数 As * (1 - Am)。


#### 1.1.3 maskSource

将用作掩码的项目。 掩码项被渲染到一个中间像素缓冲区中，结果中的 alpha 值用于确定源项在显示中的像素可见性。

#### 1.1.4 source

将被掩藏的源项。

注意：不支持让效果包含自身，例如通过将源设置为效果的父级。

## 2 Image遮罩

<font color="red">Image遮罩的特殊情况，就是Image保持长宽比，如果只用一个矩形，可能不能产生圆角效果，因为可能圆角效果正好对的是Image的透明区域。</font>

如果图片保持长宽比，图片除了显示区域，还会有透明区域。

![alt text](image.png)

<font color="red">虽然官方没有要求说是要求 source 与 maskSource 长宽要一样，但是我发现，无论怎么修改maskSource的长宽（只要赋值长宽的情况下），还是跟source的长宽一样。所以最好还是设置相同的size</font>

我们可以通过设置两个矩形，或者一个Item嵌套一个矩形，从而实现对保持长宽比图像的圆角。通过下面两种方式的任意一种：

1. 通过设置两个嵌套矩形。

2. Item内嵌套一个矩形。


图片中白色区域，其实要改成透明区域，只不过是为了看到两个矩形的明显效果，而设置成白色。

![alt text](image-1.png)

## 参考

[参考1：qml Image绘制圆角(图片绘制圆角)](https://blog.csdn.net/qq_43081702/article/details/125104269)

[参考2：QML类型：OpacityMask](https://blog.csdn.net/kenfan1647/article/details/122150554)