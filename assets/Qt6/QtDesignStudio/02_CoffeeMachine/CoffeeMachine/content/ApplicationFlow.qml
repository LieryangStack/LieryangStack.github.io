/****************************************************************************
**
** Copyright (C) 2019 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Design Studio.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick 2.4
import CoffeeMachine 1.0

ApplicationFlowForm {
    id: applicationFlow

    /* 初始状态 */
    state: "initial"

    property int animationDuration: 400

    /* 根据不同的按钮，切换到不同的状态 */

    /* 点击给我冲杯咖啡页面后，就会进入到选择设置咖啡牛奶、糖数量页面 */
    choosingCoffee.brewButtonSelection.onClicked: {
        applicationFlow.state = "to settings"
        applicationFlow.choosingCoffee.milkSlider.value = applicationFlow.choosingCoffee.sideBar.currentMilk
        applicationFlow.choosingCoffee.sugarSlider.value = 2
    }

    /* 侧边栏选择某一种类型的咖啡 */
    choosingCoffee.sideBar.onCoffeeSelected: {
        applicationFlow.state = "selection"
    }

    /* 冲咖啡页面可以选择返回 */
    choosingCoffee.backButton.onClicked: {
        applicationFlow.state = "back to selection"
    }

    /* 放入一个空杯子 */
    choosingCoffee.brewButton.onClicked: {
        applicationFlow.state = "to empty cup"
    }

    /* 该信号处理函数是连接到 Navigation -> row 组件的自定义信号 clicked */
    emptyCup.continueButton.onClicked: {
        applicationFlow.state = "to brewing"
        brewing.coffeeName = choosingCoffee.sideBar.currentCoffee
        brewing.foamAmount = choosingCoffee.sideBar.currentFoam
        brewing.milkAmount = applicationFlow.choosingCoffee.milkSlider.value
        brewing.coffeeAmount = choosingCoffee.sideBar.currentCoffeeAmount
        brewing.start()
    }

    brewing.onFinished: {
        applicationFlow.state = "reset"
    }

}
