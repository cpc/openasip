TCECC=tcecc
ADF=
TPEF=aes.tpef
REPORT=aes_sim.txt
SIM_CMD=../sim.cmd

TCECC_FLAGS=-O3

$(TPEF): aes.c aes.h aes_dec.c aes_enc.c aes_func.c aes_key.c
	$(TCECC) $(TCECC_FLAGS) -a $(ADF) -o $@ $<

simulate: $(TPEF)
	ttasim -a $(ADF) -p $(TPEF) < $(SIM_CMD) > $(REPORT)

clean:
	rm -f $(TPEF) $(REPORT)

