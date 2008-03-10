#!/bin/bash
../../../src/codesign/Explorer/explore -e InitialMachineExplorer -d data/FFTTest --hdb=data/initial.hdb data/test.dsdb
../../../src/codesign/Explorer/explore -w 1 data/test.dsdb
diff data/InitialFFT.adf 1.adf
