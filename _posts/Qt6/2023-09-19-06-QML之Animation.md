---
layout: post
title: 六、QML——Animation
categories: QML
tags: [QML]
---

动画将应用于属性的更改。当属性值更改时，动画将定义从一个值到另一个值的插值曲线。这些动画曲线创建从一个值到另一个值的平滑过渡。

1. `Animation` 是一个基础类型，这个类型不能实例化。

2. `PropertyAnimation` 是基础于 `Animation` 的一个属性动画类型。可以在属性值变化区间插值，调整属性值变化的速度和如何变化。

3. `ColorAnimation` 和 `NumberAnimation` 以及 `RotationAnimation` 是继承于 `PropertyAnimation`。其中 `PropertyAnimation` 适用于任意类型的属性动画，而这三种只适用于颜色、数值、旋转属性动画。

4. `SequentialAnimation` 和 `ParallelAnimation` 是指一组动画，顺序运行或者并行运行。

## 1 Animation

`Animation` 类型不能直接在 QML 文件中使用。它的存在提供了一组通用的属性和方法，这些属性和方法在所有继承自它的其他动画类型中可用。

### 1.1 属性

#### 1.1.1 alwaysRunToEnd（bool）

- 如果 alwaysRunToEnd 为 true：当动画被停止（通过将 running 属性设置为 false 或调用 stop() 方法）时，动画会继续执行完当前的迭代再停止，不会立刻中断。

- 如果 alwaysRunToEnd 为 false：动画会立即停止，不会继续执行当前的迭代。

#### 1.1.2 loops（int）

该属性表示动画应播放的次数。

- 默认情况下，loops 为 `1`：<font color="red">动画将播放一次然后停止。</font>

- 如果设置为 `Animation.Infinite`，动画将无限次重复，直到显式停止（通过将 running 属性设置为 false 或调用 stop() 方法）。

- 如果设置为 `0`，则动画不运行。

在以下示例中，矩形将无限次旋转。

```qml
Rectangle {
    width: 100; height: 100; color: "green"
    RotationAnimation on rotation {
        loops: Animation.Infinite
        from: 0 /* 值从那个值开始变化 */
        to: 360 /* 值变化到那个值 */
        duration: 1000 /* 变化持续时间 */
    }
}
```

#### 1.1.3 paused (bool)

该属性表示动画当前是否处于暂停状态。

可以通过设置 paused 属性以声明式地控制动画是否暂停。

我们还可以通过 JavaScript 使用 `pause()` 和 `resume()` 方法控制动画暂停和恢复。

默认情况下，动画未暂停。

#### 1.1.4 running（bool）

该属性表示动画当前是否正在运行。

可以通过设置 running 属性以声明式地控制动画是否运行。以下示例将在按下 MouseArea 时对矩形进行动画处理。

```qml
Rectangle {
    width: 100; height: 100
    NumberAnimation on x {
        running: myMouse.pressed
        from: 0; to: 100
        duration: 1000
    }
    MouseArea { id: myMouse }
}
```

同样，running 属性可以读取，以确定动画是否正在运行。在以下示例中，Text 项将指示动画是否正在运行。

```qml
NumberAnimation { id: myAnimation }
Text { text: myAnimation.running ? "Animation is running" : "Animation is not running" }
```

动画还可以通过 JavaScript 使用 start() 和 stop() 方法以命令式方式启动和停止。

默认情况下，动画不运行。但当动画被分配给属性时，作为属性值源使用 `on` 语法时，它们默认设置为运行。

### 1.2 信号

#### 1.2.1 finished()

此信号不会在 running 被设置为 false 时发出，也不会为 loops 属性设置为 Animation.Infinite 的动画发出。此外，它仅为顶级独立动画发出。对于 Behavior 或 Transition 中的动画，或动画组中的动画，不会发出此信号。

如果 alwaysRunToEnd 为 true，则此信号将在动画完成当前迭代后发出。

注意：相应的处理程序是 onFinished。

#### 1.2.2 started()

当动画开始时发出此信号。

此信号仅为顶级独立动画触发。对于 Behavior 或 Transition 中的动画，或动画组中的动画，不会触发此信号。

注意：相应的处理程序是 onStarted。

#### 1.2.3 stopped()

当动画结束时发出此信号。

动画可能是手动停止的，也可能是运行完成的。此信号仅为顶级独立动画触发。对于 Behavior 或 Transition 中的动画，或动画组中的动画，不会触发此信号
。
如果 alwaysRunToEnd 为 true，则此信号将在动画完成当前迭代后发出。

注意：相应的处理程序是 onStopped。

### 1.3 方法

#### 1.3.1 complete()

停止动画并跳到最终属性值。

如果动画未运行，调用此方法无效。调用 complete() 后，running 属性将为 false。

与 stop() 不同，complete() 会立即快进动画至结束。在以下示例中：

```qml
Rectangle {
    NumberAnimation on x { from: 0; to: 100; duration: 500 }
}
```

在 250 毫秒时调用 stop() 会使 x 属性值为 50，而调用 complete() 会将 x 属性设置为 100，就像动画已完整播放一样。

#### 1.3.2 pause()

暂停动画。

如果动画已经暂停或未运行，调用此方法无效。调用 pause() 后，paused 属性将为 true。

#### 1.3.3 restart()

重启动画。

此方法是一个便捷方法，相当于先调用 stop() 然后调用 start()。

#### 1.3.4 resume()

恢复暂停的动画。

如果动画未暂停或未运行，调用此方法无效。调用 resume() 后，paused 属性将为 false。

#### 1.3.5 start()

启动动画。

如果动画已经在运行，调用此方法无效。调用 start() 后，running 属性将为 true。

#### 1.3.6 stop()

停止动画。

如果动画未运行，调用此方法无效。调用 stop() 后，running 和 paused 属性将为 false。通常，stop() 会立即停止动画，并且动画不再影响属性值。

在以下示例中：

```qml
Rectangle {
    NumberAnimation on x { from: 0; to: 100; duration: 500 }
}
```

在 250 毫秒时停止动画，x 属性值将为 50。
但是，如果 alwaysRunToEnd 属性被设置，动画将继续运行直到完成，然后停止。



## 2 PropertyAnimation

`PropertyAnimation` 是继承于 `Animation`。

在 `Transition` 中，例如，使用 `InOutQuad` 缓动曲线来为由于状态变化而修改 `x` 或 `y` 属性的对象添加动画：

```qml
Rectangle {
    id: rect
    width: 100; height: 100
    color: "red"

    states: State {
        name: "moved"
        PropertyChanges { target: rect; x: 50 }
    }

    transitions: Transition {
        PropertyAnimation { properties: "x,y"; easing.type: Easing.InOutQuad }
    }
}
```

在 `Behavior` 中，例如，为矩形的 `x` 属性的所有变化添加动画：

```qml
Rectangle {
    width: 100; height: 100
    color: "red"

    /* Behavior就是所有为 x 属性，所有的变化都添加动画 */
    Behavior on x { PropertyAnimation {} }

    MouseArea { anchors.fill: parent; onClicked: parent.x = 50 }
}
```

## 3 



