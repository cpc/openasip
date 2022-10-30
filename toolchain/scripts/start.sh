#!/usr/bin/env bash

source /root/scripts/tce-env.sh

Xvfb :0 &
x11vnc -ncache 10 -display :0 &

while true; do sleep 10; done;
