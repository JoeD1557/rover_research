#include "mainwindowcontroller.h"
#include "maincontroller.h"
#include "qmlgstreamerglitem.h"
#include "core/sensordataparser.h"
#include "core/logger.h"
#include "core/constants.h"

#include <QQmlComponent>

#define LOG_TAG "MainWindowController"

namespace Soro {

MainWindowController::MainWindowController(QQmlEngine *engine, QObject *parent) : QObject(parent)
{
    _updateLatencyTimerId = TIMER_INACTIVE;
    _driveMode = DriveGamepadMode::DualStickDrive;
    _latency = 0;
    _playing = false;

    // Create UI for settings and control
    QQmlComponent qmlComponent(engine, QUrl("qrc:/qml/MainWindow.qml"));
    _window = qobject_cast<QQuickWindow*>(qmlComponent.create());
    if (!qmlComponent.errorString().isEmpty() || !_window)
    {
        MainController::panic(LOG_TAG, "Cannot create main window QML: " + qmlComponent.errorString());
    }

    _window->show();

    connect(_window, SIGNAL(closed()), this, SIGNAL(closed()));

    START_TIMER(_updateLatencyTimerId, 500);
}

void MainWindowController::resetPipeline()
{
    if (!_pipeline.isNull())
    {
        QGlib::disconnect(_pipeline->bus(), "message", this, &MainWindowController::onBusMessage);
        _pipeline->setState(QGst::StatePaused);
        _pipeline->setState(QGst::StateNull);
        _pipeline.clear();
    }
}

void MainWindowController::playVideo(SocketAddress address, VideoFormat format)
{
    resetPipeline();

    if (!format.isUseable()) {
        LOG_E(LOG_TAG, "play(): Given unusable format, refusing to play");
        stopVideo();
        return;
    }
    _videoFormat = format;

    _pipeline = QGst::Pipeline::create();
    _pipeline->bus()->addSignalWatch();
    QGlib::connect(_pipeline->bus(), "message", this, &MainWindowController::onBusMessage);

    // create a udpsrc to receive the stream
    QString binStr = "udpsrc address=%1 port=%2 reuse=true ! %3 ! videoscale ! video/x-raw,width=%4,height=%5 ! videoconvert ! capsfilter caps=video/x-raw,format=rgb";
    binStr = binStr.arg(address.host.toString(),
                        QString::number(address.port),
                        format.createGstDecodingArgs(),
                        QString::number(format.getWidth()),
                        QString::number(format.getHeight()));

    // create a gstreamer bin from the description
    QGst::BinPtr source = QGst::Bin::fromDescription(binStr);
    // create a gstreamer sink
    QGst::ElementPtr sink = getMainVideoSink();

    _pipeline->add(source, sink);
    source->link(sink);

    _playing = true;
    _pipeline->setState(QGst::StatePlaying);
}

void MainWindowController::stopVideo(VideoTestSrcPattern pattern, bool grayscale)
{
    _videoFormat = VideoFormat();
    resetPipeline();
    //create videotestsrc pipeline for coolness
    QString binStr = "videotestsrc pattern=%1 ! video/x-raw,width=640,height480 ! videoconvert ! capsfilter caps=video/x-raw,format=%2";
    binStr = binStr.arg(QString::number(static_cast<qint32>(pattern)),
                        grayscale ? "gray16" : "rgb");

    _pipeline = QGst::Pipeline::create();
    QGst::BinPtr source = QGst::Bin::fromDescription(binStr);
    QGst::ElementPtr sink = getMainVideoSink();

    _pipeline->add(source, sink);
    source->link(sink);

    _playing = false;
    _pipeline->setState(QGst::StatePlaying);
}

void MainWindowController::onBusMessage(const QGst::MessagePtr &message)
{

}

void MainWindowController::onZeroHudOrientationClicked()
{
    QMetaObject::invokeMethod(_window, "zero");
}

void MainWindowController::setDriveGamepadMode(DriveGamepadMode mode)
{
    _driveMode = mode;
}

DriveGamepadMode MainWindowController::getDriveGamepadMode() const
{
    return _driveMode;
}

void MainWindowController::onLatencyChanged(int latency)
{
    _latency = latency;
}

QGst::ElementPtr MainWindowController::getLeftVideoSink()
{
    return qvariant_cast<QmlGStreamerGlItem*>(_window->property("leftGStreamerSurface"))->videoSink();
}

QGst::ElementPtr MainWindowController::getRightVideoSink()
{
    return qvariant_cast<QmlGStreamerGlItem*>(_window->property("rightGStreamerSurface"))->videoSink();
}

QGst::ElementPtr MainWindowController::getMainVideoSink()
{
    return qvariant_cast<QmlGStreamerGlItem*>(_window->property("mainGStreamerSurface"))->videoSink();
}

void MainWindowController::onSensorUpdate(char tag, int value)
{
    switch (tag) {
    case SensorDataParser::DATATAG_WHEELPOWER_A:
        _window->setProperty("wheelMLPower", value);
        break;
    case SensorDataParser::DATATAG_WHEELPOWER_B:
        _window->setProperty("wheelFLPower", value);
        break;
    case SensorDataParser::DATATAG_WHEELPOWER_C:
        _window->setProperty("wheelFRPower", value);
        break;
    case SensorDataParser::DATATAG_WHEELPOWER_D:
        _window->setProperty("wheelMRPower", value);
        break;
    case SensorDataParser::DATATAG_WHEELPOWER_E:
        _window->setProperty("wheelBRPower", value);
        break;
    case SensorDataParser::DATATAG_WHEELPOWER_F:
        _window->setProperty("wheelBLPower", value);
        break;
    case SensorDataParser::DATATAG_IMUDATA_REAR_PITCH:
        _window->setProperty("rearPitch", value);
        break;
    case SensorDataParser::DATATAG_IMUDATA_REAR_ROLL:
        _window->setProperty("rearRoll", value);
        break;
    case SensorDataParser::DATATAG_IMUDATA_FRONT_PITCH:
        _window->setProperty("frontPitch", value);
        break;
    case SensorDataParser::DATATAG_IMUDATA_FRONT_ROLL:
        _window->setProperty("frontRoll", value);
        break;
    }
}

void MainWindowController::onGamepadUpdate(const GamepadManager::GamepadState *state)
{
    float x = 0, y = 0;
    switch (_driveMode)
    {
    case DriveGamepadMode::SingleStickDrive:
        x = GamepadUtil::axisShortToAxisFloat(state->axisLeftX);
        y = GamepadUtil::axisShortToAxisFloat(state->axisLeftY);
        break;
    case DriveGamepadMode::DualStickDrive:
        // TODO Unsupported currently, this is placeholder code
        x = 0;
        y = GamepadUtil::axisShortToAxisFloat(state->axisLeftY);
        break;
    default: break;
    }

    _window->setProperty("gamepadX", x);
    _window->setProperty("gamepadY", y);
}

void MainWindowController::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == _updateLatencyTimerId)
    {
        int latency = _latency;
        if (latency >= 0) { // Don't add to latency if there's no connection
            latency += _window->property("hudLatency").toInt();
        }
        _window->setProperty("latency", latency);
    }
}

void MainWindowController::setRecordingState(RecordingState state)
{
    switch (state)
    {
    case RecordingState_Idle:
        _window->setProperty("recordingState", "idle");
        break;
    case RecordingState_Waiting:
        _window->setProperty("recordingState", "waiting");
        break;
    case RecordingState_Recording:
        _window->setProperty("recordingState", "recording");
        break;
    }
}

void MainWindowController::setHudVisible(bool visible)
{
    _window->setProperty("hudVisible", visible);
}

bool MainWindowController::isHudVisible() const
{
    return _window->property("hudVisible").toBool();
}

void MainWindowController::setHudParallax(int parallax)
{
    _window->setProperty("hudParallax", parallax);
}

int MainWindowController::getHudParallax() const
{
    return _window->property("hudParallax").toInt();
}

void MainWindowController::setHudLatency(int latency)
{
    _window->setProperty("hudLatency", latency);
}

int MainWindowController::getHudLatency() const
{
    return _window->property("hudLatency").toInt();
}

void MainWindowController::setSideBySideStereo(bool stereo)
{
    _window->setProperty("stereo", stereo);
}

bool MainWindowController::getSideBySideStereo() const
{
    return _window->property("stereo").toBool();
}

} // namespace Soro
