## Copyright 2017 The University of Oklahoma.
##
## Licensed under the Apache License, Version 2.0 (the "License");
## you may not use this file except in compliance with the License.
## You may obtain a copy of the License at
##
##     http://www.apache.org/licenses/LICENSE-2.0
##
## Unless required by applicable law or agreed to in writing, software
## distributed under the License is distributed on an "AS IS" BASIS,
## WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
## See the License for the specific language governing permissions and
## limitations under the License.

TEMPLATE = subdirs

SUBDIRS =\
    soro_core \
    video_streamer \
    audio_streamer \
    research_rover \
    research_control

video_streamer.depends = soro_core
audio_streamer.depends = soro_core
research_rover.depends = soro_core audio_streamer video_streamer
research_control.depends = soro_core audio_streamer video_streamer
