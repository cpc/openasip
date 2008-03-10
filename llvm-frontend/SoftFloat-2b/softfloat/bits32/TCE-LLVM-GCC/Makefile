
PROCESSOR_H = ../../../processors/TCE-GCC.h
SOFTFLOAT_MACROS = ../softfloat-macros

TPEF = .tpef
OBJ = .o
EXE = .bc
INCLUDES = -I. -I..
COMPILE_C = tcecc -c -o $@ $(INCLUDES) -I- -O2
LINK = tcecc -o $@

ALL: softfloat$(OBJ) timesoftfloat$(EXE) timesoftfloat$(TPEF)

milieu.h: $(PROCESSOR_H)
	touch milieu.h

softfloat$(OBJ): milieu.h softfloat.h softfloat-specialize $(SOFTFLOAT_MACROS) ../softfloat.c
	$(COMPILE_C) ../softfloat.c

timesoftfloat$(OBJ): milieu.h softfloat.h ../timesoftfloat.c
	$(COMPILE_C) ../timesoftfloat.c

timesoftfloat$(EXE): softfloat$(OBJ) timesoftfloat$(OBJ)
	$(LINK) softfloat$(OBJ) timesoftfloat$(OBJ)

timesoftfloat$(TPEF): timesoftfloat$(EXE)
	tcecc -a 1_bus.adf -s 1_bus_seq.conf -o timesoftfloat$(TPEF) timesoftfloat$(EXE)

clean:
	rm -f softfloat$(OBJ) timesoftfloat$(OBJ) timesoftfloat$(EXE) timesoftfloat$(TPEF)
