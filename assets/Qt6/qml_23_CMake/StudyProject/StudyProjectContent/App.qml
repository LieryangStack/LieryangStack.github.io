// Copyright (C) 2021 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import QtQuick
import StudyProject
import QtQuick.Studio.Effects
import QtCharts
import Li1

Window {
    width: Constants.width
    height: Constants.height

    visible: true
    title: "StudyProject"

    Screen01 {
        id: mainScreen
        anchors.fill: parent
    }

    MyItem {
        x: 100; y: 100
        width: 100
        height: 100
    }

    MyItemControls {
        x: 300; y: 100
        width: 100
        height: 100
    }
}

