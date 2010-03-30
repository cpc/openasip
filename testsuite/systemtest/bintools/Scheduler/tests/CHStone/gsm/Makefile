TCECC=tcecc
ADF=
TPEF=gsm.tpef
REPORT=gsm_sim.txt
SIM_CMD=../sim.cmd

TCECC_FLAGS=-O3

$(TPEF): gsm.c add.c lpc.c private.h
	$(TCECC) $(TCECC_FLAGS) -a $(ADF) -o $@ $<

simulate: $(TPEF) $(ADF)
	ttasim -a $(ADF) -p $(TPEF) < $(SIM_CMD) > $(REPORT)

clean:
	rm -f $(TPEF) $(REPORT)

