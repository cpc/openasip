TCECC=tcecc
ADF=
TPEF=jpeg.tpef
REPORT=jpeg_sim.txt
SIM_CMD=../sim.cmd

TCECC_FLAGS=-O3

$(TPEF): main.c chenidct.c decode.h huffman.c init.h jpeg2bmp.c decode.c \
global.h huffman.h jfif_read.c  main.c marker.c
	$(TCECC) $(TCECC_FLAGS) -a $(ADF) -o $@ $<

simulate: $(TPEF) $(ADF)
	ttasim -a $(ADF) -p $(TPEF) < $(SIM_CMD) > $(REPORT)

clean:
	rm -f $(TPEF) $(REPORT)

