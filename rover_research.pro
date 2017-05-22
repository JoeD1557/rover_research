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
