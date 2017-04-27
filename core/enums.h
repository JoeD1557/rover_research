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

/*********************************************************
 * This code can be compiled on a Qt or mbed enviornment *
 *********************************************************/

#include <QtCore>

#ifndef SORO_ENUMS_H
#define SORO_ENUMS_H

/**
 * Shared messages are passed between the rover and mission control. Values in this enum
 * are always appended as a header to these messages to indicate what kind of data they store.
 */
enum SharedMessageType {
    // Start at a high bit number to vastly reduce the chance of mix-ups
    SharedMessage_RoverSharedChannelStateChanged = 1000000000,
    SharedMessage_RoverStatusUpdate,
    SharedMessage_RoverGpsUpdate,
    SharedMessage_RoverMediaServerError,
    SharedMessage_RoverDriveOverrideStart,
    SharedMessage_SensorUpdate,
    SharedMessage_RoverDriveOverrideEnd,
    SharedMessage_StartDataRecording,
    SharedMessage_StopAllCameraStreams,
    SharedMessage_StopDataRecording,
    SharedMessage_StartStereoCameraStream,
    SharedMessage_StartMonoCameraStream,
    SharedMessage_StartAux1CameraStream,
    SharedMessage_RequestActivateAudioStream,
    SharedMessage_RequestDeactivateAudioStream,
    SharedMessage_AudioStreamChanged
};

enum RoverCameraState {
    StreamingCameraState = 1000000200,
    DisabledCameraState,
    UnavailableCameraState
};

enum DriveGamepadMode {
    SingleStickDrive = 1000000300,
    DualStickDrive
};

enum MbedMessageType
{
    // These MUST stay in 8-bit range
    MbedMessage_ArmMaster = 1,
    MbedMessage_ArmGamepad,
    MbedMessage_Drive,
    MbedMessage_Gimbal
};

enum NotificationType
{
    NotificationType_Error,
    NotificationType_Warning,
    NotificationType_Info
};

enum RecordingState
{
    RecordingState_Idle,
    RecordingState_Waiting,
    RecordingState_Recording
};

#endif // SORO_ENUMS_H
