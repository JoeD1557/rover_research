import QtQuick 2.7
import QtQuick.Controls 2.0
import QtGraphicalEffects 1.0
import QtQuick.Window 2.2

import Soro 1.0

ApplicationWindow {
    id: mainWindow

    width: 640
    height: 480

    title: "Mission Control"

    property alias gstreamerSurface: gstreamerSurface

    property alias latency: hudLatency.latency
    property alias gamepadX: hudLatency.xValue
    property alias gamepadY: hudLatency.yValue
    property alias latencyTolerance: hudLatency.latencyTolerance
    property alias rearRoll: hudOrientationBack.rearRoll
    property alias frontRoll: hudOrientationBack.frontRoll
    property alias rearRollZero: hudOrientationBack.rearRollZero
    property alias frontRollZero: hudOrientationBack.frontRollZero
    property alias rearPitch: hudOrientationSide.rearPitch
    property alias frontPitch: hudOrientationSide.frontPitch
    property alias rearPitchZero: hudOrientationSide.rearPitchZero
    property alias frontPitchZero: hudOrientationSide.frontPitchZero
    property alias wheelFLPower: hudPower.wheelFLPower
    property alias wheelFRPower: hudPower.wheelFRPower
    property alias wheelMLPower: hudPower.wheelMLPower
    property alias wheelMRPower: hudPower.wheelMRPower
    property alias wheelBLPower: hudPower.wheelBLPower
    property alias wheelBRPower: hudPower.wheelBRPower

    property bool stereo: false
    property bool hudVisible: true
    property int hudParallax: 0
    property string recordingState: "idle"

    signal closed()

    onClosing: {
        if (recordingState !== "idle") {
            close.accepted = false
        }
        else {
            closed()
        }
    }

    function zero() {
        frontPitchZero = frontPitch
        rearPitchZero = rearPitch
        frontRollZero = frontRoll
        rearRollZero = rearRoll
    }

    GStreamerSurface {
        id: gstreamerSurface
        anchors.fill: parent
    }

    Item {
        id: overlayItem
        anchors.fill: parent

        HudPower {
            id: hudPower
            blurSource: gstreamerSurface
            anchors.left: parent.left
            anchors.leftMargin: hudParallax
            anchors.top: parent.top
            width: height * 0.8
            height: parent.height * 0.3
            halfWidth: stereo
            visible: hudVisible
        }

        HudOrientationSide {
            id: hudOrientationSide
            blurSource: gstreamerSurface
            anchors.left: parent.left
            anchors.leftMargin: hudParallax
            anchors.bottom: parent.bottom
            width: height
            height: parent.height * 0.3
            halfWidth: stereo
            visible: hudVisible
            enabled: hudVisible
        }

        HudOrientationBack {
            id: hudOrientationBack
            blurSource: gstreamerSurface
            anchors.right: stereo ? parent.horizontalCenter : parent.right
            anchors.bottom: parent.bottom
            width: height
            height: parent.height * 0.3
            halfWidth: stereo
            visible: hudVisible
            enabled: hudVisible
        }

        HudLatency {
            id: hudLatency
            blurSource: gstreamerSurface
            anchors.right: stereo ? parent.horizontalCenter : parent.right
            anchors.top: parent.top
            halfWidth: stereo
            width: height
            height: parent.height * 0.6
            visible: hudVisible
            enabled: hudVisible
        }
    }
}
