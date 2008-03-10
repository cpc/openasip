#!/bin/sh
sox -s -c 2 -w -t .raw -r 44100 $1 -t ossdsp /dev/dsp 
