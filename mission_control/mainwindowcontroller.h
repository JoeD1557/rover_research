#ifndef MAINWINDOWCONTROLLER_H
#define MAINWINDOWCONTROLLER_H

#include <QObject>
#include <QQuickWindow>
#include <QQmlApplicationEngine>
#include <QTimerEvent>

#include <Qt5GStreamer/QGst/Element>
#include <Qt5GStreamer/QGst/Pipeline>
#include <Qt5GStreamer/QGst/Bin>
#include <Qt5GStreamer/QGst/Message>

#include "gamepadmanager.h"

#include "core/enums.h"
#include "core/videoformat.h"
#include "core/socketaddress.h"

namespace Soro {

class MainWindowController : public QObject
{
    Q_OBJECT
public:
    enum VideoTestSrcPattern {
        Pattern_SMPTE = 0,
        Pattern_Snow,
        Pattern_Black,
        Pattern_White,
        Pattern_Red,
        Pattern_Green,
        Pattern_Blue,
        Pattern_Checkers1,
        Pattern_Checkers2,
        Pattern_Checkers4,
        Pattern_Checkers8,
        Pattern_Circular,
        Pattern_Blink,
        Pattern_SMPTE75,
        Pattern_ZonePlate,
        Pattern_Gamut,
        Pattern_ChromaZonePlate,
        Pattern_Solid,
        Pattern_Ball,
        Pattern_SMPTE100,
        Pattern_Bar,
        Pattern_Pinwheel,
        Pattern_Spokes,
        Pattern_Gradient,
        Pattern_Colors
    };

    explicit MainWindowController(QQmlEngine *engine, QObject *parent = 0);

    bool isHudVisible() const;
    int getHudParallax() const;
    int getHudLatency() const;
    bool getSideBySideStereo() const;
    QGst::ElementPtr getLeftVideoSink();
    QGst::ElementPtr getRightVideoSink();
    QGst::ElementPtr getMainVideoSink();
    DriveGamepadMode getDriveGamepadMode() const;
    void playVideo(SocketAddress address, VideoFormat format);
    void stopVideo(VideoTestSrcPattern pattern = Pattern_SMPTE, bool grayscale=false);

Q_SIGNALS:
    void closed();

public Q_SLOTS:
    void onLatencyChanged(int latency);
    void onZeroHudOrientationClicked();
    void onSensorUpdate(char tag, int value);
    void setRecordingState(RecordingState state);
    void setSideBySideStereo(bool stereo);
    void onGamepadUpdate(const GamepadManager::GamepadState *state);
    void setHudVisible(bool visible);
    void setHudParallax(int parallax);
    void setHudLatency(int latency);
    void setDriveGamepadMode(DriveGamepadMode mode);

protected:
    void timerEvent(QTimerEvent *e);

private:
    void resetPipeline();
    void onBusMessage(const QGst::MessagePtr & message);

    int _latency;
    QGst::PipelinePtr _pipeline;
    bool _playing;
    VideoFormat _videoFormat;
    QQuickWindow *_window = 0;
    DriveGamepadMode _driveMode;
    int _updateLatencyTimerId;
};

} // namespace Soro

#endif // MAINWINDOWCONTROLLER_H
