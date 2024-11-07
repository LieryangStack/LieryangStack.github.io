// Copyright (C) 2021 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import QtQuick
import LiQML

Window {
    id: window
    width: Constants.width
    height: Constants.height

    flags: Qt.FramelessWindowHint | Qt.Window

    visible: true
    color: "#00000000"
    title: "LiQML"

    Screen01 {
        id: screen01
        anchors.fill: parent
    }


}

