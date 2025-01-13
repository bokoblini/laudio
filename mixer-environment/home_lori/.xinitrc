#!/bin/bash

echo "starting dbus-daemon" >&2
export DBUS_SESSION_BUS_ADDRESS="$(dbus-daemon --session --print-address --fork)"
echo "export DBUS_SESSION_BUS_ADDRESS=\"$DBUS_SESSION_BUS_ADDRESS\"" > ~/.dbus-address
echo "starting pulseaudio" >&2
/usr/bin/pulseaudio --start

/home/lori/amixer-settings/restore.sh > /home/lori/logs/amixer-restore.log
/home/lori/pulse-settings.sh

#echo "starting dbus-monitor"
#(dbus-monitor --session > /home/lori/dbus.log 2>&1)&

echo "starting mixer" >&2

# To ensure that GTK does not try to locate accessibility
# features for about a minute before activating the app.
export GTK_A11Y=none
#export G_MESSAGES_DEBUG=all
/home/lori/laudio/mixer/mixer
#pavucontrol
#xterm
