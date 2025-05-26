#!/bin/bash
### title: Tour script for the Coprocessor generation

TOUR_PATH="${OPENASIP_PATH}/testsuite/systemtest/scripts/coproGen_tour"
adf_path="coprogen.adf"

#CVA6
flist_path="${TOUR_PATH}/Flist.cva6"
ariane_path="${TOUR_PATH}/ariane.sv"
svfiles_path="out_cvx/systemverilog"
c_programs="main.c crc.c"
linked_file="final.o"
linker="${cva6_path}/config/gen_from_riscv_config/linker/link.ld"
DV_SIMULATORS="veri-testharness"

#RoCC
roccgen_path="${ROCC_PATH}/generators"
chipyardconfig_path="${roccgen_path}/chipyard/src/main/scala/config"
rocketmain_path="${roccgen_path}/rocket-chip/src/main"

if [[ "${INTERFACE}" -eq 1 ]]; then
    cp ${flist_path} ${cva6_path}/core
    cp ${ariane_path} ${cva6_path}/corev_apu/src/
    cp -r ${svfiles_path} ${cva6_path}/core/

    test_o="${cva6_path}/verif/tests/custom/simple_test/test.o"
    syscall_o="${cva6_path}/verif/tests/custom/common/syscall.o"
    crt_o="${cva6_path}/verif/tests/custom/common/crt.o"

    oacc_flags="--unroll-threshold=0 -I ../../ -d -v -c -O3 --mattr "+c,+m,+zba,+zbb,+zbs,+zbc,+ziscr,+zifencei" --march=riscv32" 
    gcc_flags="-c -O3 -march=rv32imc_zba_zbb_zbs_zbc_zicsr_zifencei -mabi=ilp32 -ffunction-sections -fdata-sections -I ${cva6_path}/verif/tests/custom/env -I ${cva6_path}/verif/tests/custom/common"
    
    riscv-none-elf-gcc ${gcc_flags} -o crt.o ${cva6_path}/verif/tests/custom/common/crt.S
    riscv-none-elf-gcc ${gcc_flags} -o syscall.o ${cva6_path}/verif/tests/custom/common/syscalls.c
    
    oacc-riscv ${oacc_flags} -a ${adf_path} -o compiled.o ${c_programs}
    riscv-none-elf-ld -L ${cva6_path}/util/toolchain-builder/build/gcc/gcc/rv32im/ilp32 -o ${linked_file} -T ${linker} compiled.o crt.o syscall.o -lgcc
    cp ${linked_file} ${cva6_sim_path}

    cd ${cva6_sim_path}

    python3 cva6.py --target cv32a60x --iss=$DV_SIMULATORS --iss_yaml=cva6.yaml \
    --c_tests $linked_file \
    --linker=../../config/gen_from_riscv_config/linker/link.ld \
    --gcc_opts="-static -mcmodel=medany -fvisibility=hidden -nostdlib \
    -nostartfiles -g ../tests/custom/common/syscalls.c \
    ../tests/custom/common/crt.S -lgcc \
    -I../tests/custom/env -I../tests/custom/common"


else
    cp ${TOUR_PATH}/RoCCFragments.scala ${chipyardconfig_path}/fragments
    cp ${TOUR_PATH}/RocketConfigs.scala ${chipyardconfig_path}
    cp -r out_rocc/systemverilog/. ${rocketmain_path}/resources/vsrc
    cp ${TOUR_PATH}/LazyRoCC.scala ${rocketmain_path}/scala/tile
    cp ${adf_path}  ${ROCC_PATH}/tests

    sed 's/RocketConfig/SmallrocketConfig/' ${ROCC_PATH}/variables.mk

    oacc-riscv ${oacc_flags} -c --mattr "+c,+m" -r -a ${adf_path} -o ${ROCC_PATH}/tests/compiled.o ${c_programs}

    cd ${ROCC_PATH}
    source ./env.sh
    cd tests
    
    riscv64-unknown-elf-gcc -L$HOME/local/lib -static -T htif.ld ${ROCC_PATH}/tests/compiled.o -o ${ROCC_PATH}/tests/crc.riscv 
    cd ../sims/verilator
    make CONFIG=SmallrocketConfig run-binary BINARY=../../tests/crc.riscv

fi

exit 0