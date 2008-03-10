#!/bin/bash

# Formats code lines given as arguments to postscript that is
# formatted in the official review style.

/usr/bin/a2ps --line-numbers=1 -MA4 $* -o -
