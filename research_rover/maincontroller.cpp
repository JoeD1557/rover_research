/*
 * Copyright 2016 The University of Oklahoma.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "maincontroller.h"
#include "soro_core/logger.h"
#include "soro_core/confloader.h"
#include "usbcameraenumerator.h"

#define LOG_TAG "ResearchRover"

namespace Soro {

MainController::MainController(QObject *parent) : QObject(parent)
{
    // Must initialize once the event loop has started.
    // This can be accomplished using a single shot timer.
    QTimer::singleShot(1, this, SLOT(init()));
}

void MainController::init() {
    LOG_I(LOG_TAG, "*************Initializing core networking*****************");

    _driveChannel = Channel::createServer(this, NETWORK_ALL_DRIVE_CHANNEL_PORT, CHANNEL_NAME_DRIVE,
                              Channel::UdpProtocol, QHostAddress::Any);
    _mainChannel = Channel::createServer(this, NETWORK_ALL_MAIN_CHANNEL_PORT, CHANNEL_NAME_MAIN,
                              Channel::TcpProtocol, QHostAddress::Any);

    if (_driveChannel->getState() == Channel::ErrorState) {
        LOG_E(LOG_TAG, "The drive channel experienced a fatal error during initialization");
        exit(1); return;
    }
    if (_mainChannel->getState() == Channel::ErrorState) {
        LOG_E(LOG_TAG, "The shared channel experienced a fatal error during initialization");
        exit(1); return;
    }

    _driveChannel->open();
    _mainChannel->open();

    // observers for network channel connectivity changes
    connect(_mainChannel, &Channel::stateChanged, this, &MainController::mainChannelStateChanged);
    connect(_driveChannel, &Channel::stateChanged, this, &MainController::driveChannelStateChanged);


    LOG_I(LOG_TAG, "All network channels initialized successfully");

    LOG_I(LOG_TAG, "*****************Initializing MBED systems*******************");

    // create mbed channels
    _mbed = new MbedChannel(SocketAddress(QHostAddress::Any, NETWORK_ROVER_MBED_PORT), MBED_ID, this);

    // observers for mbed events
    connect(_mbed, &MbedChannel::messageReceived, this, &MainController::mbedMessageReceived);
    connect(_mbed, &MbedChannel::stateChanged, this, &MainController::mbedChannelStateChanged);

    // observers for network channels message received
    connect(_driveChannel, &Channel::messageReceived, this, &MainController::driveChannelMessageReceived);
    connect(_mainChannel, &Channel::messageReceived, this, &MainController::mainChannelMessageReceived);

    LOG_I(LOG_TAG, "*****************Initializing GPS system*******************");

    _gpsServer = new GpsServer(SocketAddress(QHostAddress::Any, NETWORK_ROVER_GPS_PORT), this);
    connect(_gpsServer, &GpsServer::gpsUpdate, this, &MainController::gpsUpdate);

    LOG_I(LOG_TAG, "*****************Initializing Video system*******************");

    _mainCameraServer = new VideoServer(MEDIAID_MAIN_CAMERA, NETWORK_ALL_MAIN_CAMERA_PORT, this);
    _aux1CameraServer = new VideoServer(MEDIAID_AUX1_CAMERA, NETWORK_ALL_AUX1_CAMERA_PORT, this);

    connect(_mainCameraServer, &VideoServer::error, this, &MainController::mediaServerError);
    connect(_aux1CameraServer, &VideoServer::error, this, &MainController::mediaServerError);

    UsbCameraEnumerator cameras;
    cameras.loadCameras();

    QFile camFile(QCoreApplication::applicationDirPath() + "/../config/research_cameras.conf");
    if (!camFile.exists()) {
        LOG_E(LOG_TAG, "The camera configuration file ../config/research_cameras.conf does not exist.");
        exit(1);
    }
    else {
        ConfLoader camConfig;
        camConfig.load(camFile);

        const UsbCamera* stereoRight = cameras.find(camConfig.value("sr_matchName"),
                                        camConfig.value("sr_matchDevice"),
                                        camConfig.value("sr_matchVendorId"),
                                        camConfig.value("sr_matchProductId"),
                                        camConfig.value("sr_matchSerial"));

        const UsbCamera* stereoLeft = cameras.find(camConfig.value("sr_matchName"),
                                        camConfig.value("sl_matchDevice"),
                                        camConfig.value("sl_matchVendorId"),
                                        camConfig.value("sl_matchProductId"),
                                        camConfig.value("sl_matchSerial"));

        const UsbCamera* aux1 = cameras.find(camConfig.value("a1_matchName"),
                                        camConfig.value("a1_matchDevice"),
                                        camConfig.value("a1_matchVendorId"),
                                        camConfig.value("a1_matchProductId"),
                                        camConfig.value("a1_matchSerial"));

        if (stereoRight) {
            _stereoRCameraDevice = stereoRight->device;
            _monoCameraDevice = _stereoRCameraDevice;
            LOG_I(LOG_TAG, "Right stereo camera found: " + stereoRight->toString());
        }
        else {
            LOG_E(LOG_TAG, "Right stereo camera couldn't be found using provided definition.");
        }
        if (stereoLeft) {
            _stereoLCameraDevice = stereoLeft->device;
            if (!stereoRight) {
                _monoCameraDevice = _stereoLCameraDevice;
            }
            LOG_I(LOG_TAG, "Left stereo camera found: " + stereoLeft->toString());
        }
        else {
            LOG_E(LOG_TAG, "Left stereo camera couldn't be found using provided definition.");
        }
        if (aux1) {
            _aux1CameraDevice = aux1->device;
            LOG_I(LOG_TAG, "Aux1 camera found: " + aux1->toString());
        }
        else {
            LOG_E(LOG_TAG, "Aux1 camera couldn't be found using provided definition.");
        }

    }

    LOG_I(LOG_TAG, "*****************Initializing Audio system*******************");

    _audioServer = new AudioServer(MEDIAID_AUDIO, NETWORK_ALL_AUDIO_PORT, this);

    connect(_audioServer, &AudioServer::error, this, &MainController::mediaServerError);

    LOG_I(LOG_TAG, "*****************Initializing Data Recording System*******************");

    _sensorDataSeries = new SensorDataParser(this);
    _gpsDataSeries = new GpsCsvSeries(this);
    _dataRecorder = new CsvRecorder(this);

    _dataRecorder->setUpdateInterval(50);
    _dataRecorder->addColumn(_sensorDataSeries->getWheelPowerASeries());
    _dataRecorder->addColumn(_sensorDataSeries->getWheelPowerBSeries());
    _dataRecorder->addColumn(_sensorDataSeries->getWheelPowerCSeries());
    _dataRecorder->addColumn(_sensorDataSeries->getWheelPowerDSeries());
    _dataRecorder->addColumn(_sensorDataSeries->getWheelPowerESeries());
    _dataRecorder->addColumn(_sensorDataSeries->getWheelPowerFSeries());
    _dataRecorder->addColumn(_sensorDataSeries->getImuRearYawSeries());
    _dataRecorder->addColumn(_sensorDataSeries->getImuRearPitchSeries());
    _dataRecorder->addColumn(_sensorDataSeries->getImuRearRollSeries());
    _dataRecorder->addColumn(_sensorDataSeries->getImuFrontYawSeries());
    _dataRecorder->addColumn(_sensorDataSeries->getImuFrontPitchSeries());
    _dataRecorder->addColumn(_sensorDataSeries->getImuFrontRollSeries());
    _dataRecorder->addColumn(_gpsDataSeries->getLatitudeSeries());
    _dataRecorder->addColumn(_gpsDataSeries->getLongitudeSeries());
    connect(_gpsServer, &GpsServer::gpsUpdate, _gpsDataSeries, &GpsCsvSeries::addLocation);
    connect(_mbed, &MbedChannel::messageReceived, _sensorDataSeries, &SensorDataParser::newData);

    LOG_I(LOG_TAG, "-------------------------------------------------------");
    LOG_I(LOG_TAG, "-------------------------------------------------------");
    LOG_I(LOG_TAG, "-------------------------------------------------------");
    LOG_I(LOG_TAG, "Initialization complete");
    LOG_I(LOG_TAG, "-------------------------------------------------------");
    LOG_I(LOG_TAG, "-------------------------------------------------------");
    LOG_I(LOG_TAG, "-------------------------------------------------------");
}

void MainController::mainChannelStateChanged(Channel::State state) {
    if (state == Channel::ConnectedState) {
        // send all status information since we just connected
        // TODO there is an implementation bug where a Channel will not send messages immediately after it connects
        QTimer::singleShot(1000, this, SLOT(sendSystemStatusMessage()));
    }
}

void MainController::sendSystemStatusMessage() {
    QByteArray message;
    QDataStream stream(&message, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::BigEndian);

    MainMessageType messageType = MainMessageType_RoverStatusUpdate;

    stream << static_cast<qint32>(messageType);
    stream << (_mbed->getState() == MbedChannel::ConnectedState);
    _mainChannel->sendMessage(message);
}

bool MainController::startDataRecording(QDateTime startTime) {
    LOG_I(LOG_TAG, "Starting test log with start time of " + QString::number(startTime.toMSecsSinceEpoch()));

    return _dataRecorder->startLog(startTime);
}

void MainController::stopDataRecording() {
    LOG_I(LOG_TAG, "Ending test log");

    _dataRecorder->stopLog();
}

void MainController::driveChannelStateChanged(Channel::State state) {
    if (state != Channel::ConnectedState) {
        //Send a stop command to the rover
        char stopMessage[DriveMessage::RequiredSize];
        DriveMessage::setGamepadData_DualStick(stopMessage, 0, 0, 0);
        _mbed->sendMessage(stopMessage, DriveMessage::RequiredSize);
    }
}

void MainController::mbedChannelStateChanged(MbedChannel::State state) {
    Q_UNUSED(state);
    sendSystemStatusMessage();
}

void MainController::driveChannelMessageReceived(const char* message, Channel::MessageSize size) {
    char header = message[0];
    MbedMessageType messageType;
    reinterpret_cast<qint32&>(messageType) = (qint32)reinterpret_cast<unsigned char&>(header);
    switch (messageType) {
    case MbedMessage_Drive:
        _mbed->sendMessage(message, (int)size);
        break;
    default:
        LOG_E(LOG_TAG, "Received invalid message from mission control on drive control channel");
        break;
    }
}

void MainController::mediaServerError(MediaServer *server, QString message) {
    QByteArray byeArray;
    QDataStream stream(&byeArray, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::BigEndian);
    MainMessageType messageType = MainMessageType_RoverMediaServerError;

    stream << messageType;
    stream <<(qint32)server->getMediaId();
    stream << message;
    _mainChannel->sendMessage(byeArray);
}

/* Main channel message handler
 */
void MainController::mainChannelMessageReceived(const char* message, Channel::MessageSize size) {
    QByteArray byteArray = QByteArray::fromRawData(message, size);
    QDataStream stream(byteArray);
    MainMessageType messageType;

    stream >> reinterpret_cast<qint32&>(messageType);
    switch (messageType) {
    case MainMessageType_RequestActivateAudioStream: {
        //
        // Start audio stream
        //
        QString profileString;
        stream >> profileString;
        GStreamerUtil::AudioProfile profile(profileString);
        _audioServer->start(profile);
    }
        break;
    case MainMessageType_RequestDeactivateAudioStream:
        //
        // Stop audio stream
        //
        _audioServer->stop();
        break;
    case MainMessageType_StartStereoCameraStream: {
        //
        // Start STEREO main camera stream
        //
        QString profileString;
        bool vaapi;
        stream >> profileString;
        stream >> vaapi;
        GStreamerUtil::VideoProfile profile(profileString);
        if (!_stereoRCameraDevice.isEmpty() && !_stereoLCameraDevice.isEmpty()) {
            _mainCameraServer->start(_stereoLCameraDevice, _stereoRCameraDevice, profile, vaapi);
        }
    }
        break;
    case MainMessageType_StartMonoCameraStream: {
        //
        // Start MONO main camera stream
        //
        QString profileString;
        bool vaapi;
        stream >> profileString;
        stream >> vaapi;
        GStreamerUtil::VideoProfile profile(profileString);
        if (!_monoCameraDevice.isEmpty()) {
            _mainCameraServer->start(_monoCameraDevice, profile, vaapi);
        }
    }
        break;
    case MainMessageType_StopAllCameraStreams:
        //
        // Stop all camera streams
        //
        _mainCameraServer->stop();
        _aux1CameraServer->stop();
        break;
    case MainMessageType_StartAux1CameraStream:{
        //
        // Start AUX1 camera stream
        //
        QString profileString;
        bool vaapi;
        stream >> profileString;
        stream >> vaapi;
        GStreamerUtil::VideoProfile profile(profileString);
        if (!_aux1CameraDevice.isEmpty()) {
            _aux1CameraServer->start(_aux1CameraDevice, profile, vaapi);
        }
    }
        break;
    case MainMessageType_StartDataRecording: {
        //
        // Start data recording
        //
        qint64 startTime;
        stream >> startTime;
        if (startDataRecording(QDateTime::fromMSecsSinceEpoch(startTime))) {
            // Echo the message back to mission control
            QByteArray byteArray;
            QDataStream stream(&byteArray, QIODevice::WriteOnly);
            MainMessageType messageType = MainMessageType_StartDataRecording;
            stream << static_cast<qint32>(messageType);
            _mainChannel->sendMessage(byteArray);
        }
    }
        break;
    case MainMessageType_StopDataRecording:
        //
        // Stop data recording
        //
        stopDataRecording();
        break;
    default:
        LOG_W(LOG_TAG, "Got unknown shared channel message");
        break;
    }
}

void MainController::mbedMessageReceived(const char* message, int size) {
    // Forward the message to mission control (MbedDataParser instance will take care of logging it)

    QByteArray byteArray;
    QDataStream stream(&byteArray, QIODevice::WriteOnly);
    MainMessageType messageType = MainMessageType_SensorUpdate;

    stream << static_cast<qint32>(messageType);
    stream << QByteArray(message, size);

    _mainChannel->sendMessage(byteArray);
}

void MainController::gpsUpdate(NmeaMessage message) {
    // Forward this update to mission control
    QByteArray byteArray;
    QDataStream stream(&byteArray, QIODevice::WriteOnly);
    MainMessageType messageType = MainMessageType_RoverGpsUpdate;
    stream.setByteOrder(QDataStream::BigEndian);

    stream << static_cast<qint32>(messageType);
    stream << message;

    _mainChannel->sendMessage(byteArray);
}

MainController::~MainController() {

}

} // namespace Soro

