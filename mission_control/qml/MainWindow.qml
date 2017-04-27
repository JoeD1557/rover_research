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

    property alias leftGStreamerSurface: leftGStreamerSurface
    property alias rightGStreamerSurface: rightGStreamerSurface
    property alias mainGStreamerSurface: leftGStreamerSurface

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
        id: leftGStreamerSurface
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.right: stereo ? parent.horizontalCenter : parent.right
    }

    GStreamerSurface {
        id: rightGStreamerSurface

        anchors.left: parent.horizontalCenter
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        visible: stereo
        enabled: stereo
    }

    HudPower {
        id: hudPower
        anchors.left: parent.left + hudParallax
        anchors.top: parent.top
        halfWidth: stereo
        visible: hudVisible
    }

    HudOrientationSide {
        id: hudOrientationSide
        anchors.left: parent.left + hudParallax
        anchors.bottom: parent.bottom
        halfWidth: stereo
        visible: hudVisible
        enabled: hudVisible
    }

    HudOrientationBack {
        id: hudOrientationBack
        anchors.right: stereo ? parent.horizontalCenter : parent.right
        anchors.bottom: parent.bottom
        halfWidth: stereo
        visible: hudVisible
        enabled: hudVisible
    }

    HudLatency {
        id: hudLatency
        anchors.right: stereo ? parent.horizontalCenter : parent.right
        anchors.top: parent.top
        halfWidth: stereo
        visible: hudVisible
        enabled: hudVisible
    }

    ShaderEffectSource {
        sourceItem: hudPower
        sourceRect: Qt.rect(0, 0, sourceItem.width, sourceItem.height)
        anchors.left: parent.horizontalCenter
        anchors.top: parent.top
        visible: stereo && hudVisible
        enabled: stereo && hudVisible
    }

    ShaderEffectSource {
        sourceItem: hudOrientationSide
        sourceRect: Qt.rect(0, 0, sourceItem.width, sourceItem.height)
        anchors.left: parent.horizontalCenter
        anchors.bottom: parent.bottom
        visible: stereo && hudVisible
        enabled: stereo && hudVisible
    }

    ShaderEffectSource {
        sourceItem: hudOrientationBack
        sourceRect: Qt.rect(0, 0, sourceItem.width, sourceItem.height)
        anchors.right: parent.right - hudParallax
        anchors.bottom: parent.bottom
        visible: stereo && hudVisible
        enabled: stereo && hudVisible
    }

    ShaderEffectSource {
        sourceItem: hudLatency
        sourceRect: Qt.rect(0, 0, sourceItem.width, sourceItem.height)
        anchors.right: parent.right - hudParallax
        anchors.top: parent.top
        visible: stereo && hudVisible
        enabled: stereo && hudVisible
    }
}
