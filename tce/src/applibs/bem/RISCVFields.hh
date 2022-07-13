#ifndef RISCV_FIELDS_HH
#define RISCV_FIELDS_HH

#include <map>

const std::map<std::string, int> riscvRTypeOperations = {
    {"add",         0b00000000000110011},
    {"sub",         0b01000000000110011},
    {"xor",         0b00000001000110011},
    {"or",          0b00000001100110011},
    {"and",         0b00000001110110011},
    {"sll",         0b00000000010110011},
    {"srl",         0b00000001010110011},
    {"sra",         0b01000001010110011},
    {"slt",         0b00000000100110011},
    {"sltu",        0b00000000110110011},
    {"mul",         0b00000010000110011},
    {"mulh",        0b00000010010110011},
    {"mulhu",       0b00000010110110011},
    {"mulhsu",      0b00000010100110011},
    {"div",         0b00000011000110011},
    {"divu",        0b00000011010110011},
    {"rem",         0b00000011100110011},
    {"remu",        0b00000011110110011}
};

const std::map<std::string, int> riscvITypeOperations = { 
    {"addi",        0b0000010011         },
    {"xori",        0b1000010011         },
    {"ori",         0b1100010011         },
    {"andi",        0b1110010011         },
    {"slli",        0b00000000010010011  },
    {"srli",        0b00000001010010011  },
    {"srai",        0b01000001010010011  },
    {"slti",        0b0100010011         },
    {"sltiu",       0b0110010011         },
    {"lb",          0b0000000011         },
    {"lh",          0b0010000011         },
    {"lw",          0b0100000011         },
    {"lbu",         0b1000000011         },
    {"lhu",         0b1010000011         },
    {"jalr",         0b0001100111         }
};

const std::map<std::string, int> riscvSTypeOperations = {
    {"sb",      0b0000100011},
    {"sh",      0b0010100011},
    {"sw",      0b0100100011}
};

const std::map<std::string, int> riscvBTypeOperations = {
    {"beq",    0b0001100011},
    {"bne",    0b0011100011},
    {"blt",    0b1001100011},
    {"bge",    0b1011100011},
    {"bltu",   0b1101100011},
    {"bgeu",   0b1111100011}
};

const std::map<std::string, int> riscvUTypeOperations = {
    {"lui",  0b0110111},
    {"auipc",  0b0010111}
};

const std::map<std::string, int> riscvJTypeOperations = {
    {"jal", 0b1101111}
};

const std::map<std::string, std::string> operationNameTable = {
    {"add", "add"}, {"sub", "sub"}, {"xor", "xor"}, {"or", "ior"},
    {"sll", "shl"}, {"srl", "shru"}, {"sra", "shr"}, {"slt", "lt"},
    {"sltu", "ltu"}, {"addi", "add"}, {"xori", "xor"}, {"ori", "ior"},
    {"andi", "and"}, {"slli", "shl"}, {"srli", "shru"}, {"srai", "shr"},
    {"slti", "lt"}, {"sltiu", "ltu"}, {"lb", "ald8"}, {"lh", "ald16"},
    {"lw", "ald32"}, {"lbu", "aldu8"}, {"lhu", "aldu16"}, {"sb", "ast8"},
    {"sh", "ast16"}, {"sw", "ast32"}, {"beq", "beqr"}, {"bne", "bner"},
    {"blt", "bltr"}, {"bge", "bger"}, {"bltu", "bltur"}, {"bgeu", "bgeur"},
    {"jal", "callr"}, {"jalr", "calla"}, {"lui", "move"}, {"auipc", "apc"},
    {"mul", "mul"}, {"mulh", "mulhi"}, {"mulhsu", "mulhisu"},
    {"mulhu", "mulhiu"},{"rem", "rem"}, {"remu", "remu"}, {"div", "div"},
    {"divu", "divu"}
};

#endif
