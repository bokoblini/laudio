#!/bin/bash

pacmd set-default-sink alsa_output.platform-soc_sound.stereo-fallback
pacmd set-default-source alsa_input.platform-soc_sound.stereo-fallback
pacmd set-sink-volume 1 52428
pacmd load-module module-loopback latency_msec=1
pacmd load-module module-remap-sink sink_name=mono master=alsa_output.platform-soc_sound.stereo-fallback \
  channels=2 channel_map=mono,mono
pacmd set-default-sink mono
