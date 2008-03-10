#!/bin/sh
cd ~/testprogs
export TTASIM_OUTPUT_FILE=output.raw
ttasim <<EOF
prog tremor
run
info proc cycles
EOF
