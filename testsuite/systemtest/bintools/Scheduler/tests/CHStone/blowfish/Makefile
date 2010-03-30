TCECC=tcecc
ADF=
TPEF=bf.tpef
REPORT=bf_sim.txt
SIM_CMD=../sim.cmd

TCECC_FLAGS=-O3

$(TPEF): bf.c bf_cfb64.c bf_enc.c  bf_locl.h  bf_pi.h  bf_skey.c  blowfish.h
	$(TCECC) $(TCECC_FLAGS) -a $(ADF) -o $@ $<

simulate: $(TPEF) $(ADF)
	ttasim -a $(ADF) -p $(TPEF) < $(SIM_CMD) > $(REPORT)

clean:
	rm -f $(TPEF) $(REPORT)

