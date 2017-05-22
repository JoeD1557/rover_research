#include "gstreamerrecorder.h"
#include "soro_core/logger.h"

#include <Qt5GStreamer/QGst/Bus>
#include <Qt5GStreamer/QGlib/Connect>

#define LOG_TAG "GStreamerRecorder" + _name

namespace Soro {

GStreamerRecorder::GStreamerRecorder(SocketAddress mediaAddress, QString name, QObject *parent) : QObject(parent)
{
    _name = name;
    _mediaAddress = mediaAddress;
}

void GStreamerRecorder::begin(quint8 codec, qint64 timestamp)
{
    //TODO
}

void GStreamerRecorder::stop()
{
    if (!_pipeline.isNull())
    {
        LOG_I(LOG_TAG, "Stopping recording");
        _pipeline->bus()->removeSignalWatch();
        _pipeline->setState(QGst::StateNull);
        _pipeline.clear();
        _bin.clear();
    }
}

void GStreamerRecorder::onBusMessage(const QGst::MessagePtr & message)
{
    switch (message->type())
    {
    case QGst::MessageEos:
        LOG_E(LOG_TAG, "onBusMessage(): Received end-of-stream message.");
        stop();
        break;
    case QGst::MessageError:
    {
        QString errorMessage = message.staticCast<QGst::ErrorMessage>()->error().message().toLatin1();
        LOG_E(LOG_TAG, "onBusMessage(): Received error message from gstreamer '" + errorMessage + "'");
        stop();
    }
    default:
        break;
    }
}

} // namespace Soro
