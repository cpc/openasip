#!/usr/bin/python3
# This script performs a binary search to find the maximum frequency
# (within .01 ns) for a TTA design using Vivado.
import subprocess
import os
import sys
import time

verbose = False # for printing debugging messages
logging = True  # save Vivado output to vivado.log files

def run_implementation(fmax):
    if verbose:
        print(time.asctime( time.localtime(time.time())))
        print("Trying frequency: {:6.2f} MHz".format(1000.0/fmax))

    constraints = os.path.join(os.path.dirname(__file__), "constraints.xdc")
    with open(constraints, 'w') as f:
        f.write("create_clock -period {} [get_ports clk]".format(fmax))

    with open(os.devnull, 'w') as dev_null:
        source_script = os.path.join(os.path.dirname(__file__), "timing.tcl")
        command = ["vivado", "-mode", "batch", "-source", source_script,
                   "-nojournal", "-notrace"]
        if not logging:
            command.append("-nolog")

        return subprocess.call(command, stdout=dev_null, stderr=dev_null)


if __name__ == "__main__":

    verbose = "-v" in sys.argv or "-verbose" in sys.argv
    logging = "-l" in sys.argv or "-log" in sys.argv
    test_freq = "--tf" in sys.argv or "--test_freq" in sys.argv

    if test_freq:
        period = 1000.0 / int(sys.argv[sys.argv.index("--tf")+1])

        if run_implementation(period):
            print("failed")
        else:
            print("passed")
        quit(1)


    # Establish lower limit and screen out problems in RTL
    if run_implementation(40.0):
        print("Implementation failed or didn't reach timing at 25 MHz, aborting.")
        quit(1)

    min_period = 0.0
    max_period = 40.0

    # Guess at a better starting point: 200 MHz
    if run_implementation(5.0):
        min_period = 5.0
    else:
        max_period = 5.0

    # Basic binary search
    while max_period - min_period > 0.01:
        test_period = (max_period + min_period)/2
        if run_implementation(test_period):
            # Sometimes when vivado fails it does not produce below files at all,
            # so we need to try again.
            try:
                os.remove("final_timing.txt")
                os.remove("pre_place_timing.txt")
            except:
                if verbose:
                    print("Failed, retrying ...")
                continue

            min_period = test_period

        else:
            max_period = test_period
            os.rename("final_timing.txt", "passed_final_timing.txt")
            os.rename("pre_place_timing.txt", "passed_pre_place_timing.txt")

    print("Tmin:", max_period, "Fmax:", 1000/max_period)
