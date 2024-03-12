#!/usr/bin/env python3
from typing import NamedTuple
import sys
import os
import math
import shutil
import sqlite3
import re

class Operation:
    def __init__(self, bus_width: int, access_width: int, load: bool,
                       sign_ext: bool = False, big_endian: bool = False):

        if bus_width > 32 and big_endian:
            print("FATAL: Cannot generate big-endian vector load")
            sys.exit()

        if access_width > 16 and sign_ext:
            print("FATAL: Cannot sign extend a value over 32 bits wide")
            sys.exit()

        self.bus_width = bus_width
        self.access_width = access_width
        self.sign_ext = sign_ext
        self.big_endian = big_endian
        self.load = load

        if load:
            name = "ld"
        else:
            name = "st"

        if big_endian:
            if access_width == 8:
                name += "q"
            elif access_width == 16:
                name += "h"
            elif access_width == 32:
                name += "w"
            else:
                print("FATAL: unexpected big-endian load width: {}".format(access_width))
                sys.exit()
            if load and not sign_ext and access_width < 32:
                name += "u"
        else:
            if load and access_width < 32 and not sign_ext:
                name += "u"
            name += str(self.access_width)

        self.name = name

def write_xml(width: int, source: str, sink: str):

    replaces = [("BUS_NAME", "lsu_{}bit".format(width)),
                ("BIT_WIDTH",str(width)),
                ("BYTE_WIDTH", str(width//8)),
                ("LOW_BITS", str((width//8-1).bit_length()))]

    with open(sink, 'w') as outfile:
        with open(source, 'r') as infile:
            for line in infile:
                for (placeholder, value) in replaces:
                    line = line.replace(placeholder, value)

                outfile.write(line)




def assign(lhs: str, rhs: str, lang: str):
    if lang == "vhdl":
        return "{} <= {};\n".format(lhs, rhs)
    else:
        return "{} = {};\n".format(lhs, rhs)

def case_start(bits: int, bus_width: int, signal: str, lang: str):
    high = (bus_width//8 - 1).bit_length() - 1
    low = high - bits + 1
    if lang == "vhdl":
        return "case {} is".format(slice(signal, high, low, lang))
    else:
        return "unique case ({})".format(slice(signal, high, low, lang))

def case_end(lang: str):
    if lang == "vhdl":
        return "end case;"
    else:
        return "endcase"

def case_select(elements: int, index: int, bits: int, big_endian: bool, lang: str):
    if index == elements - 1:
        if lang == "vhdl":
            select = "  when others => "
        else:
            select = "  default: "
    else:
        select = "{{:0{}b}}".format(bits)

        if big_endian:
            select = select.format(elements-index-1)
        else:
            select = select.format(index)

        if lang == "vhdl":
            select = '  when "{}" => '.format(select);
        else:
            select = "  {}'b{}: ".format(bits, select)

    return select

def concat(lhs: str, rhs: str, lang: str):
    if lang == "vhdl":
        return "{} & {}".format(lhs, rhs)
    else:
        return "{{{0}, {1}}}".format(lhs, rhs)

def extend(width: int, value: str, lang: str):
    if lang == "vhdl":
        return "({}-1 downto 0 => {})".format(int(width), value);
    else:
        return "{{{0}{{{1}}}".format(int(width), value)


def bit_select(signal: str, bit: int, lang: str):
    if lang == "vhdl":
        return "{}({})".format(signal, bit);
    else:
        return "{}[{}]".format(signal, bit);

def zeros(width: int, lang: str):
    if lang == "vhdl":
        return '"' + "0"*width + '"'
    else:
        return "{}'b".format(width) + "0"*width

def one(lang: str):
    if lang == "vhdl":
        return "'1'"
    else:
        return "1'b1"

def ones(width: int, lang: str):
    if lang == "vhdl":
        return '"' + "1"*width + '"'
    else:
        return "{}'b".format(width) + "1"*width

def slice(signal: str, high, low, lang: str):
    if lang == "vhdl":
        return "{}({} downto {})".format(signal, high, low);
    else:
        return "{}[{}:{}]".format(signal, high, low);

def write_load_post_op(op: Operation, filename: str, lang: str):
    if not op.load:
        print("FATAL: Stores have no post-op file")
        sys.exit()

    with open(filename, 'w') as file:
        # To help synthesis tools to optimize, the scalar 8 and 16 wide
        # loads explicitely use the same multiplexer as the 32 bit load.
        # In practice the 32 bit multiplexer is copied in front of all the
        # narrow loads and then a second multiplexer is used to select
        # from its output. The synthesis tool is assumed to be able to
        # remove the identical redundant 32 bit multiplexers.
        # The signal 'load_data_32b' is shared between all scalar loads.
        if op.access_width in [8, 16, 32]:

            # 32 bit multiplexer:
            if op.bus_width == 32:  #no multiplexer needed
                file.write(assign("load_data_32b", "rdata_out_1",lang));
            else:
                subelements = op.bus_width // 32
                bits = (subelements-1).bit_length()

                file.write(case_start(bits, op.bus_width, "addr_low_out_1", lang) + "\n")
                for index in range(subelements):
                    low = index*32
                    high = low + 32-1
                    line = case_select(subelements, index, bits, op.big_endian, lang)
                    data = slice("rdata_out_1", high, low, lang);
                    line += assign("load_data_32b", data, lang)
                    file.write(line)
                file.write(case_end(lang) + "\n")

            if op.access_width == 32:
                file.write(assign("op2", "load_data_32b", lang))
            else:
                # The second multiplexer to select from the 32 bits.
                subelements = 6 - op.access_width // 4 #16 -> 2, 8 -> 4
                bits = (subelements-1).bit_length()
                file.write(case_start(bits, 32, "addr_low_out_1", lang) + "\n")
                for index in range(subelements):
                    low = index*op.access_width
                    high = low + op.access_width-1
                    line = case_select(subelements, index, bits, op.big_endian, lang)
                    data = slice("load_data_32b", high, low, lang);

                    pad_width = 32 - op.access_width;
                    if not op.sign_ext:
                        padding = zeros(pad_width, lang)
                    else:
                        bit = bit_select("load_data_32b", high, lang)
                        padding = extend(pad_width, bit, lang)

                    data = concat(padding, data, lang)
                    line += assign("op2", data, lang)
                    file.write(line)
                file.write(case_end(lang))

        # Normal case for anything other than 8,16 or 32.
        else:
            if op.bus_width == op.access_width:
                file.write(assign("op2", "rdata_out_1", lang));
                return
            subelements = op.bus_width // op.access_width
            bits = (subelements-1).bit_length()

            file.write(case_start(bits, op.bus_width, "addr_low_out_1", lang) + "\n")

            for index in range(subelements):
                low = index*op.access_width
                high = low + op.access_width-1
                line = case_select(subelements, index, bits, op.big_endian, lang)

                data = slice("rdata_out_1", high, low, lang);

                if op.access_width < 32:
                    pad_width = 32 - op.access_width;
                    if not op.sign_ext:
                        padding = zeros(pad_width, lang)
                    else:
                        bit = bit_select("rdata_out_1", high, lang)
                        padding = extend(pad_width, bit, lang)
                    data = concat(padding, data, lang)

                line += assign("op2", data, lang)

                file.write(line)

            file.write(case_end(lang))

def write_store_op(op: Operation, filename: str, lang: str):
    if op.load:
        print("FATAL: Loads share an op file")
        sys.exit()

    with open(filename, 'w') as file:
        file.write(assign("avalid_in_1", one(lang), lang))
        file.write(assign("awren_in_1", one(lang), lang))
        file.write(assign("aaddr_in_1", slice("op1", "addrw_c-1", 0, lang), lang))

        if op.access_width == op.bus_width:
            file.write(assign("adata_in_1", "op2", lang))
            file.write(assign("astrb_in_1", ones(op.bus_width//8, lang), lang))
        # To help synthesis tools, hierarchically use the 32-bit store operation
        # for 8 and 16 bit operations. This contains two muxes, the first for
        # creating operands for 32 bit store, and the second mux for creating
        # an identical 32 bit store mux for every operation. The synth tool is
        # assumed to optimize away the identical redundant muxes. To further
        # help this, the intermediate signals all have the same names so the
        # strobe_32b and write_data_32b are shared between the scalar operations.
        elif op.access_width in [8, 16, 32]:
            # St32 doesn't need the first mux, but still uses the same shared signals.
            if op.access_width == 32:
                file.write(assign("strobe_32b", ones(4,lang), lang))
                file.write(assign("write_data_32b", "op2", lang))
            else:
                file.write(assign("strobe_32b", zeros(4,lang), lang))
                file.write(assign("write_data_32b", zeros(32,lang), lang))
                subelements = 6 -  op.access_width//4 #8 -> 4, 16 -> 2
                bits = (subelements-1).bit_length()
                file.write(case_start(bits, 32, "op1", lang) + "\n")
                for index in range(subelements):
                    low = index*op.access_width
                    high = low + op.access_width-1

                    num_bytes = int(op.access_width // 8)
                    bytes_low = index*num_bytes
                    bytes_high = bytes_low + num_bytes-1

                    line = case_select(subelements, index, bits, op.big_endian, lang);

                    if lang == "verilog":
                        line += " begin"
                    file.write(line + "\n");

                    file.write("    " + assign(slice("strobe_32b", bytes_high, bytes_low, lang),
                                           ones(num_bytes, lang), lang))
                    file.write("    " + assign(slice("write_data_32b", high, low, lang), "op2", lang))

                    if lang == "verilog":
                        file.write("  end\n")
                file.write(case_end(lang) + "\n")

            # Create 32-bit store operation mux. Same for stores 8,16 and 32
            if op.bus_width == 32: #no mux needed
                file.write(assign("adata_in_1", "write_data_32b", lang))
                file.write(assign("astrb_in_1", "strobe_32b", lang))
            else:
                subelements = op.bus_width // 32
                bits = (subelements-1).bit_length()
                file.write(case_start(bits, op.bus_width, "op1", lang) + "\n")

                for index in range(subelements):
                    low = index*32
                    high = low + 32 -1

                    num_bytes = 4
                    bytes_low = index*num_bytes
                    bytes_high = bytes_low + num_bytes-1

                    line = case_select(subelements, index, bits, op.big_endian, lang);

                    if lang == "verilog":
                        line += " begin"
                    file.write(line + "\n");

                    file.write("    " + assign("astrb_in_1", zeros(op.bus_width//8, lang), lang));
                    file.write("    " + assign(slice("astrb_in_1", bytes_high, bytes_low, lang),
                                               "strobe_32b", lang))

                    file.write("    " + assign("adata_in_1", zeros(op.bus_width, lang), lang));
                    file.write("    " + assign(slice("adata_in_1", high, low, lang), "write_data_32b", lang))

                    if lang == "verilog":
                        file.write("  end\n")

                file.write(case_end(lang))

        # Normal case when access_width not 8,16 or 32
        else:
            subelements = op.bus_width // op.access_width
            bits = (subelements-1).bit_length()

            file.write(case_start(bits, op.bus_width, "op1", lang) + "\n")

            for index in range(subelements):
                low = index*op.access_width
                high = low + op.access_width-1

                num_bytes = int(op.access_width // 8)
                bytes_low = index*num_bytes
                bytes_high = bytes_low + num_bytes-1

                line = case_select(subelements, index, bits, op.big_endian, lang);

                if lang == "verilog":
                    line += " begin"
                file.write(line + "\n");

                file.write("    " + assign("astrb_in_1", zeros(op.bus_width//8, lang), lang));
                file.write("    " + assign(slice("astrb_in_1", bytes_high, bytes_low, lang),
                                           ones(num_bytes, lang), lang))

                file.write("    " + assign("adata_in_1", zeros(op.bus_width, lang), lang));
                file.write("    " + assign(slice("adata_in_1", high, low, lang), "op2", lang))

                if lang == "verilog":
                    file.write("  end\n")

            file.write(case_end(lang))

def make_dir(path: str):
    try:
        os.mkdir(path)
    except FileExistsError:
        pass

def add_operation(op: Operation, resource_id: int, c):
    bus_definition = "generate_lsu/{0}/ipxact/lsu_interface_{0}.xml".format(op.bus_width)
    if op.load:
        latency = 2
        main_vhdl = "shared/ldxx.vhdl"
        main_vlog = "shared/ldxx.v"
        post_op_vhdl = "{}/vhdl/{}-post-op.vhdl".format(op.bus_width, op.name)
        post_op_vlog = "{}/verilog/{}-post-op.v".format(op.bus_width, op.name)

        write_load_post_op(op, post_op_vhdl, "vhdl")
        write_load_post_op(op, post_op_vlog, "verilog")
    else:
        latency = 0
        main_vhdl = "{}/vhdl/{}.vhdl".format(op.bus_width, op.name)
        main_vlog = "{}/verilog/{}.v".format(op.bus_width, op.name)
        post_op_vhdl = ""
        post_op_vlog = ""

        write_store_op(op, main_vhdl, "vhdl")
        write_store_op(op, main_vlog, "verilog")

    main_vhdl = "generate_lsu/" + main_vhdl
    main_vlog = "generate_lsu/" + main_vlog
    if post_op_vhdl != "":
        post_op_vhdl = "generate_lsu/" + post_op_vhdl
        post_op_vlog = "generate_lsu/" + post_op_vlog

    c.execute("INSERT INTO operation_implementation VALUES (NULL, ?, ?, ?, ?, ?, ?, ?)",
              (latency, op.name, bus_definition, post_op_vhdl, post_op_vlog,
               "generate_lsu/shared/defaults.vhdl", "generate_lsu/shared/defaults.v"))
    op_impl_id = c.lastrowid;

    c.execute("INSERT INTO block_source_file VALUES (NULL, ?, 0)", (main_vhdl,))
    c.execute("INSERT INTO operation_implementation_source_file VALUES (NULL, ?, ?)",
              (op_impl_id, c.lastrowid))
    c.execute("INSERT INTO block_source_file VALUES (NULL, ?, 1)", (main_vlog,))
    c.execute("INSERT INTO operation_implementation_source_file VALUES (NULL, ?, ?)",
              (op_impl_id, c.lastrowid))
    c.execute("INSERT INTO operation_implementation_resources VALUES (NULL, ?, ?, 1)",
              (op_impl_id, resource_id))

    if (op.access_width <= 32):
        if (op.load):
            c.execute("INSERT INTO operation_implementation_globalsignal VALUES (NULL, ?, ?, ?, ?, ?, ?)",
                      (op_impl_id, "load_data_32b", "32", "Logic", "VHDL", "0"))
            c.execute("INSERT INTO operation_implementation_globalsignal VALUES (NULL, ?, ?, ?, ?, ?, ?)",
                      (op_impl_id, "load_data_32b", "32", "Logic", "VHDL", "0"))
        else:
            c.execute("INSERT INTO operation_implementation_globalsignal VALUES (NULL, ?, ?, ?, ?, ?, ?)",
                      (op_impl_id, "strobe_32b", "4", "Logic", "VHDL", "0"))
            c.execute("INSERT INTO operation_implementation_globalsignal VALUES (NULL, ?, ?, ?, ?, ?, ?)",
                      (op_impl_id, "write_data_32b", "32", "Logic", "VHDL", "0"))
            c.execute("INSERT INTO operation_implementation_globalsignal VALUES (NULL, ?, ?, ?, ?, ?, ?)",
                      (op_impl_id, "strobe_32b", "4", "Logic", "Verilog", "0"))
            c.execute("INSERT INTO operation_implementation_globalsignal VALUES (NULL, ?, ?, ?, ?, ?, ?)",
                      (op_impl_id, "write_data_32b", "32", "Logic", "Verilog", "0"))



def main(hdb_filename):

    RR = r"generate_lsu_([\d]+)\.hdb"
    m = re.match(RR, hdb_filename)
    if not m:
        print("Usage: $0 generate_lsu_N.hdb")
        return 1
    bw = int(m.group(1))
    if bw <= 0:
        print("Usage: $0 generate_lsu_N.hdb")
        return 1

    for bus_width in [bw]:

        make_dir("{}".format(bus_width))
        make_dir("{}/vhdl".format(bus_width))
        make_dir("{}/verilog".format(bus_width))
        make_dir("{}/ipxact".format(bus_width))

        write_xml(bus_width, "shared/lsu_interface.xml.tmpl",
                  "{0}/ipxact/lsu_interface_{0}.xml".format(bus_width))
        reg_file = "{0}/ipxact/lsu_registers_{0}.xml".format(bus_width)
        write_xml(bus_width, "shared/lsu_registers.xml.tmpl", reg_file)

        shutil.copyfile("shared/empty.hdb", "../generate_lsu_{}.hdb".format(bus_width))
        hdb = sqlite3.connect("../generate_lsu_{}.hdb".format(bus_width))

        c = hdb.cursor()
        c.execute("INSERT INTO operation_implementation_resource VALUES (NULL, ?, ?, NULL)",
                  ("lsu_registers_{}".format(bus_width), "generate_lsu/" + reg_file))
        resource_id = c.lastrowid
        c.execute("INSERT INTO block_source_file VALUES (NULL, \"generate_lsu/shared/lsu_registers.vhdl\", 1)")
        c.execute("INSERT INTO operation_implementation_resource_source_file VALUES (NULL, ?, ?)",
                  (resource_id, c.lastrowid))

        for access_width in [8, 16, 32, 64, 128, 256, 512, 1024, 2048]:
            if access_width > bus_width:
                continue
            add_operation(Operation(bus_width, access_width, True), resource_id, c)
            add_operation(Operation(bus_width, access_width, False), resource_id, c)

            # Big-endian loads
            if bus_width == 32:
                add_operation(Operation(bus_width, access_width, True, big_endian=True), resource_id, c)
                add_operation(Operation(bus_width, access_width, False, big_endian=True), resource_id, c)
                if access_width < 32:
                    add_operation(Operation(bus_width, access_width, True, sign_ext=True, big_endian=True), resource_id, c)

            # sign-extended loads
            if access_width > 16:
                continue

            add_operation(Operation(bus_width, access_width, True, sign_ext=True), resource_id, c)

        hdb.commit()
        hdb.close()

main(sys.argv[1])
