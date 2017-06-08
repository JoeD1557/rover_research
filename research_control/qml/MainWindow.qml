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
    property alias middlePitch: hudOrientationSide.middlePitch
    property alias middlePitchZero: hudOrientationSide.middlePitchZero
    property alias middleRoll: hudOrientationBack.middleRoll
    property alias middleRollZero: hudOrientationBack.middleRollZero
    property alias wheelFLPower: hudPower.wheelFLPower
    property alias wheelFRPower: hudPower.wheelFRPower
    property alias wheelMLPower: hudPower.wheelMLPower
    property alias wheelMRPower: hudPower.wheelMRPower
    property alias wheelBLPower: hudPower.wheelBLPower
    property alias wheelBRPower: hudPower.wheelBRPower
    property alias compassHeading: hudCompass.compassHeading

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
        antialiasing: false
        anchors.fill: parent
    }

    Item {
        id: overlayItem
        anchors.fill: parent

        HudCompass {
            id: hudCompass
            x: stereo ? overlayItem.width / 4 - width / 2 : overlayItem.width / 2 - width / 2
            y: 0
            width: parent.height * 0.15
            height: width
            halfWidth: stereo
            visible: hudVisible
            enabled: visible
        }

        HudPower {
            id: hudPower
            blurSource: gstreamerSurface
            x: stereo ? hudParallax : 0
            y: 0
            width: height * 0.8
            height: parent.height * 0.3
            halfWidth: stereo
            visible: hudVisible
            enabled: visible
        }

        HudOrientationSide {
            id: hudOrientationSide
            blurSource: gstreamerSurface
            x: stereo ? hudParallax : 0
            y: overlayItem.height - height
            width: height
            height: parent.height * 0.3
            halfWidth: stereo
            visible: hudVisible
            enabled: visible
        }

        HudOrientationBack {
            id: hudOrientationBack
            blurSource: gstreamerSurface
            x: stereo ? overlayItem.width / 2 - width / 2 : overlayItem.width - width
            y: overlayItem.height - height
            width: height
            height: parent.height * 0.3
            halfWidth: stereo
            visible: hudVisible
            enabled: visible
        }

        HudLatency {
            id: hudLatency
            blurSource: gstreamerSurface
            x: stereo ? overlayItem.width / 2 - width / 2 : overlayItem.width - width
            y: 0
            halfWidth: stereo
            width: height
            height: parent.height * 0.6
            visible: hudVisible
            enabled: visible
        }


        HudCompass {
            id: hudCompass2
            x: overlayItem.width / 4 * 3 - width / 2
            y: 0
            compassHeading: hudCompass.compassHeading
            width: parent.height * 0.15
            height: width
            halfWidth: true
            visible: hudVisible && stereo
            enabled: visible
        }

        HudPower {
            id: hudPower2
            wheelBLPower: hudPower.wheelBLPower
            wheelBRPower: hudPower.wheelBRPower
            wheelMLPower: hudPower.wheelMLPower
            wheelMRPower: hudPower.wheelMRPower
            wheelFLPower: hudPower.wheelFLPower
            wheelFRPower: hudPower.wheelFRPower
            blurSource: gstreamerSurface
            x: overlayItem.width / 2
            y: 0
            width: height * 0.8
            height: parent.height * 0.3
            halfWidth: true
            visible: hudVisible && stereo
            enabled: visible
        }

        HudOrientationSide {
            id: hudOrientationSide2
            rearPitch: hudOrientationSide.rearPitch
            rearPitchZero: hudOrientationSide.rearPitchZero
            frontPitch: hudOrientationSide.frontPitch
            frontPitchZero: hudOrientationSide.frontPitchZero
            middlePitch: hudOrientationSide.middlePitch
            middlePitchZero: hudOrientationSide.middlePitchZero
            blurSource: gstreamerSurface
            x: overlayItem.width / 2
            y: overlayItem.height - height
            width: height
            height: parent.height * 0.3
            halfWidth: true
            visible: hudVisible && stereo
            enabled: visible
        }

        HudOrientationBack {
            id: hudOrientationBack2
            rearRoll: hudOrientationBack.rearRoll
            rearRollZero: hudOrientationBack.rearRollZero
            frontRoll: hudOrientationBack.frontRoll
            frontRollZero: hudOrientationBack.frontRollZero
            middleRoll: hudOrientationBack.middleRoll
            middleRollZero: hudOrientationBack.middleRollZero
            blurSource: gstreamerSurface
            x: overlayItem.width - width / 2 - hudParallax
            y: overlayItem.height - height;
            width: height
            height: parent.height * 0.3
            halfWidth: true
            visible: hudVisible && stereo
            enabled: visible
        }

        HudLatency {
            id: hudLatency2
            latency: hudLatency.latency
            latencyTolerance: hudLatency.latencyTolerance
            xValue: hudLatency.xValue
            yValue: hudLatency.yValue
            blurSource: gstreamerSurface
            x: overlayItem.width - width / 2 - hudParallax
            y: 0;
            halfWidth: true
            width: height
            height: parent.height * 0.6
            visible: hudVisible && stereo
            enabled: visible
        }
    }
}
