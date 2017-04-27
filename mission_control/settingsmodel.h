#ifndef SORO_MISSIONCONTROL_SETTINGSMODEL_H
#define SORO_MISSIONCONTROL_SETTINGSMODEL_H

#include <QtCore>
#include <QQuickWindow>
#include <QHostAddress>

#include "core/videoformat.h"
#include "core/audioformat.h"
#include "core/constants.h"

namespace Soro {

struct SettingsModel {

    bool enableStereoUi;
    bool enableVideo;
    bool enableStereoVideo;
    bool enableAudio;
    bool enableHud;
    bool enableGps;

    int selectedVideoEncoding;
    int selectedVideoResolution;
    int selectedVideoFramerate;
    int selectedVideoBitrate;
    int selectedMjpegQuality;
    int selectedCamera;
    int selectedLatency;
    int selectedHudParallax;
    int selectedHudLatency;

    int mainCameraIndex;
    int aux1CameraIndex;

    QHostAddress roverAddress;

    QStringList cameraNames;
    QStringList videoEncodingNames;
    QStringList videoResolutionNames;
    AudioFormat defaultAudioFormat;

    static SettingsModel Default(QHostAddress roverAddress);

    void syncUi(QQuickWindow *window);
    void syncModel(const QQuickWindow *window);
    void setSelectedCamera(int mediaId);
    VideoFormat getSelectedVideoFormat();
};

} // namespace Soro

#endif // SORO_MISSIONCONTROL_SETTINGSMODEL_H
