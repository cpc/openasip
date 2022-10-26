#!/usr/bin/env bash

source /root/tce-env.sh
cd /root/tce-devel/tce
./autogen.sh
./configure --prefix=$HOME/local
make -j8
make install
