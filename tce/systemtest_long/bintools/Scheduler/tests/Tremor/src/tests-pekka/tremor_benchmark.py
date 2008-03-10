#!/usr/bin/python
# -*- coding: latin-1 -*-
import logging
import sys
import re
import os
import time
from shutil import copy

cyclesToRun = 2000000000
timesToRepeat = 3
durations = []

def resultString(durationSec):
    tempSec = durationSec

    durationDescription = '';

    hours = tempSec / (60*60)
    tempSec = tempSec - hours*60*60

    if hours > 0:
        durationDescription = durationDescription + str(hours) + "h"

    minutes = tempSec / 60
    tempSec = tempSec - minutes*60

    if minutes > 0 or hours > 0:
        durationDescription = durationDescription + str(minutes) + "m"    
            
    seconds = tempSec

    durationDescription = durationDescription + str(seconds) + "s "

    clockRateKHz = round((float(cyclesToRun) / float(durationSec)) / 1000, 1)

    return durationDescription + str(clockRateKHz) + "KHz"


def averageString(durations):
    global cyclesToRun
    tempSec = 0
    for duration in durations:
        tempSec = tempSec + duration
    
    durationSec = tempSec / len(durations)

    return 'avg: ' + resultString(durationSec) + ' ' + \
           '(%d repetitions, %d cycles each)' % \
           (len(durations), cyclesToRun)


stillToRepeat = timesToRepeat
while stillToRepeat > 0:

    print "%d: running tremor for %d cycles:" % (stillToRepeat, cyclesToRun),
    sys.stdout.flush()
    startTime = int(time.time())
    os.popen('./run_tremor.sh ' + str(cyclesToRun));
    endTime = int(time.time())
    durationSec = endTime - startTime
    
    durations.append(durationSec)

    print resultString(durationSec) + " " + averageString(durations)
    stillToRepeat = stillToRepeat - 1



logline = averageString(durations)
try:
    copy('results.backup1', 'results.backup2');
except:
    pass

try:
    copy('results.txt', 'results.backup1');
except:
    pass

logfile = file("results.txt", "a+")
'Thu, 28 Jun 2001 14:17:15 +0000'

logfile.write(time.strftime('%d.%m.%y %H:%M:%S ' + logline + '\n'));

logfile.close()
