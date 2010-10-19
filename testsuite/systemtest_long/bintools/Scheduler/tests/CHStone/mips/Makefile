TCECC=tcecc
ADF=
TPEF=mips.tpef
REPORT=mips_sim.txt
SIM_CMD=../sim.cmd

TCECC_FLAGS=-O3

$(TPEF): mips.c imem.h
	$(TCECC) $(TCECC_FLAGS) -a $(ADF) -o $@ $<

simulate: $(TPEF) $(ADF)
	ttasim -a $(ADF) -p $(TPEF) < $(SIM_CMD) > $(REPORT)

clean:
	rm -f $(TPEF) $(REPORT)

