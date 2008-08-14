This directory is reserved for short-running system tests. No test in this
set is expected to run for more than 10 seconds in a P4/3GHz machine.
These tests are expected to be fast so they can be executed painlessly before
every commit.

Software Requirements

1. Editline library
2. SQLite3 client
3. GHDL (optional)
4. Python 2.4 or newer

1. Editline library

Some of the system tests require a full installation of Editline library.
This is because the output and behavior of some of the command line
applications is slightly different when using Editline versus a plain
stdin/stdout based console handling.

See the README-INSTALL file for more information on the Editline library.

2. SQLite 3 client

Some of the system tests require a SQLite 3 DB client (as an executable file 
called 'sqlite3') and available in PATH.

3. GHDL

GHDL is a free VHDL compiler/simulator. This is used by some of the tests
of Processor Generator / Program Image Generator. In case 'ghdl' is not
found in PATH, the tests that require GHDL to verify the correctness are
disabled.

4. Python 2.4 or newer

Scheduler test bench script is written in Python, so you need the Python
interpreter installed in order to run the scheduler tests.
