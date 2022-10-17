basedir = ${prefix}/share/openasip/hdb
nobase_base_DATA =	$(srcdir)/asic_130nm_1.5V.hdb \
					$(srcdir)/vhdl/fu/* \
					$(srcdir)/vhdl/rf/* \
					$(srcdir)/vhdl/pkg/* \
					$(srcdir)/stratixII.hdb \
					$(srcdir)/stratixII_vhdl/fpga_lsu/* \
					$(srcdir)/stratixII_vhdl/led_io/* \
					$(srcdir)/stratixII_vhdl/rtc/* \
					$(srcdir)/stratixII_vhdl/sram_lsu/* \
					$(srcdir)/avalon.hdb \
					$(srcdir)/avalon_hdb_vhdl/* \
					$(srcdir)/hibi_adapter.hdb \
					$(srcdir)/hibi_adapter_vhdl/* \
					$(srcdir)/stream.hdb \
					$(srcdir)/stream_vhdl/* \
					$(srcdir)/fpu_embedded.hdb \
					$(srcdir)/fpu_embedded_vhdl/* \
					$(srcdir)/sabrewing/* \
					$(srcdir)/fpu_half.hdb \
					$(srcdir)/fpu_half_vhdl/* \
					$(srcdir)/altera_jtag_uart.hdb \
					$(srcdir)/altera_jtag_uart_vhdl/* \
					$(srcdir)/stratixIII.hdb \
					$(srcdir)/stratixIII_vhdl/fpga_lsu/* \
					$(srcdir)/stratixIII_vhdl/led_io/* \
					$(srcdir)/stratixIII_vhdl/pushbutton/* \
					$(srcdir)/stratixIII_vhdl/rtc/* \
					$(srcdir)/stratixIII_vhdl/switch/* \
					$(srcdir)/almaif.hdb \
					$(srcdir)/generate_base32.hdb \
					$(srcdir)/generate_base32/verilog/* \
					$(srcdir)/generate_base32/vhdl/* \
					$(srcdir)/generate_lsu_32.hdb \
					$(srcdir)/generate_lsu/*/ipxact/* \
					$(srcdir)/generate_lsu/*/vhdl/* \
					$(srcdir)/generate_lsu/*/verilog/* \
					$(srcdir)/generate_lsu/shared/* \
					$(srcdir)/generate_rf_iu.hdb \
					$(srcdir)/generate_rf_iu/verilog/* \
					$(srcdir)/almaif_vhdl/* \
					$(srcdir)/xilinx_series7.hdb \
					$(srcdir)/xilinx_vhdl/*

EXTRA_DIST = ${nobase_base_DATA}

generate_lsu_32.hdb: generate_lsu/shared/* generate_lsu/generate.py
	cd generate_lsu; ./generate.py generate_lsu_32.hdb

generate_lsu/*/ipxact/* generate_lsu/*/vhdl/* generate_lsu/*/verilog/*: generate_lsu_32.hdb

clean:
	rm -rf generate_lsu_*.hdb generate_lsu/{32}
