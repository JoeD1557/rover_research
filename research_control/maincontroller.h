#ifndef MAINCONTROLLER_H
#define MAINCONTROLLER_H

#include <QObject>
#include <QApplication>
#include <QQmlEngine>

#include "soro_core/csvrecorder.h"
#include "soro_core/sensordataparser.h"
#include "soro_core/gpscsvseries.h"
#include "soro_core/channel.h"

#include "latencycsvseries.h"
#include "commentcsvseries.h"
#include "connectioneventcsvseries.h"
#include "gamepadmanager.h"
#include "settingsmodel.h"
#include "mainwindowcontroller.h"
#include "controlwindowcontroller.h"
#include "drivecontrolsystem.h"
#include "commentswindowcontroller.h"
#include "audioclient.h"
#include "audioplayer.h"
#include "videoclient.h"
#include "gstreamerrecorder.h"

namespace Soro {

class MainController : public QObject
{
    Q_OBJECT
public:
    static void init(QApplication *app);
    static void panic(QString tag, QString message);

private:
    explicit MainController(QObject *parent=0);

    static MainController *_self;

    QQmlEngine *_qml = 0;
    GamepadManager *_gamepad = 0;
    SettingsModel _settings;
    DriveControlSystem *_driveSystem = 0;
    MainWindowController *_mainWindow = 0;
    ControlWindowController *_controlWindow = 0;
    CommentsWindowController *_commentsWindow = 0;
    Channel *_mainChannel = 0;

    // Video clients
    VideoClient *_mainVideoClient = 0;
    VideoClient *_aux1VideoClient = 0;

    // Video recorders
    GStreamerRecorder *_stereoLGStreamerRecorder = 0;
    GStreamerRecorder *_stereoRGStreamerRecorder = 0;
    GStreamerRecorder *_monoGStreamerRecorder = 0;
    GStreamerRecorder *_aux1GStreamerRecorder = 0;

    // Audio stream subsystem
    AudioClient *_audioClient = 0;
    AudioPlayer *_audioPlayer = 0;
    GStreamerRecorder *_audioGStreamerRecorder = 0;

    // Data recording systems
    SensorDataParser *_sensorDataSeries = 0;
    GpsCsvSeries *_gpsDataSeries = 0;
    ConnectionEventCsvSeries *_connectionEventSeries = 0;
    LatencyCsvSeries *_latencyDataSeries = 0;
    CommentCsvSeries *_commentDataSeries = 0;
    CsvRecorder *_dataRecorder = 0;

    qint64 _recordStartTime = 0;

private Q_SLOTS:
    void onMainChannelMessageReceived(const char *message, Channel::MessageSize length);
    void onWindowClosed();
    void onRequestUiSync();
    void onSettingsApplied();

    void startDataRecording();
    void stopDataRecording();
    void toggleDataRecording();

    void stopAllRoverCameras();
    void stopAudio();

    void startAudioStream(GStreamerUtil::AudioProfile profile);
    void startAux1VideoStream(GStreamerUtil::VideoProfile profile);
    void startMainCameraStream(GStreamerUtil::VideoProfile profile, bool stereo);

    void sendStopRecordCommandToRover();
    void sendStartRecordCommandToRover();

    void onAudioClientStateChanged(MediaClient *client, MediaClient::State state);
    void onVideoClientStateChanged(MediaClient *client, MediaClient::State state);
};

} // namespace Soro

#endif // MAINCONTROLLER_H
