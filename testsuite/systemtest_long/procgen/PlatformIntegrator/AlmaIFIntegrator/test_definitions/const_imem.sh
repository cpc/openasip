ADF="mach/mach.adf"
TB_SRC="../tb/tta-almaif-tb-no-imem.vhdl ../prog_imem_pkg.vhdl"
ICD_ARGS="debugger:external"
DMEM="onchip"
IMEM="vhdl_array"
SRC="src/fib.c"
HEADER="result_templates/no_imem_header.txt"
DATA="result_templates/default_data.txt"
EXTRA_OPTS=""
EXTRA_PIG_OPTS="-f vhdl"
HDBS="generate_base32.hdb,generate_lsu_32.hdb,generate_rf_iu.hdb"
