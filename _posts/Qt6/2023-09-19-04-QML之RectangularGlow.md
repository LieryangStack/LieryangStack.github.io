---
layout: post
title: 四、QML——ApplicationWindow
categories: QML
tags: [QML]
---

RectangularGlow 具有良好的性能。光晕的形状限制为具有自定义角半径的矩形。对于需要自定义形状的情况，考虑光晕效应。


## 1 属性

### 1.1 color

用于光晕的 RGBA 颜色值。默认情况下，该属性设置为 “white”

### 1.2 cornerRadius

- **cornerRadius**：用于绘制圆角光晕的角半径。该值的范围为 0.0 到光晕有效宽度或高度的一半，以较小者为准。这可以通过以下公式计算：min(width, height) / 2.0 + glowRadius。默认情况下，该特性绑定到 glowRadius 特性。调整 glowRadius 特性时，光晕的行为就像矩形模糊一样。

### 1.3 glowRadius

- **glowRadius**：光晕到达项目区域外的像素数。该值的范围从 0.0（无光晕）到 inf（无限光晕）。默认情况下，该属性设置为 0.0

### 1.4 spread

- **spread**：在源边附近辉光颜色的大部分增强程度。该值的范围为 0.0（无强度增加）到 1.0（最大强度增加）。默认情况下，该属性设置为 0.0
