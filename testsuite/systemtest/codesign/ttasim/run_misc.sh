#!/bin/bash
cp data/ttasim-init .ttasim-init
rm -f .ttasim-history another-history
../../../../openasip/src/codesign/ttasim/ttasim
test -a .ttasim-history && cat .ttasim-history && rm -f .ttasim-history
test -a another-history && cat another-history && rm -f another-history
rm .ttasim-init
