from disass import *
um = mach.UniversalMachine()
pr = prog.Program.loadFromTPEF("test.tpef", um)
dump(pr, 0, 8)
