#include "assemble.h"
#include "instruction_assembler.h"
#include <stdio.h>
#include <stdlib.h>

void write_uint32_le(FILE *file, uint32_t value) {
  fputc(value & 0xFF, file);
  fputc((value >> 8) & 0xFF, file);
  fputc((value >> 16) & 0xFF, file);
  fputc((value >> 24) & 0xFF, file);
}
/*
int main() {
  // movz x0,#1
  parsed_line_t line1 = {
      .type = LINE_INSTRUCTION,
      .instr = {.mnemonic = "movz",
                .mnemonic_tok = TOKEN_MOVZ,
                .instr_type = INSTR_DATA_PROCESSING,
                .operand_count = 2,
                .operands = {// Rd: x0
                             {.type = OPERAND_REGISTER,
                              .reg = {.reg_num = 0, .is_64bit = true}},
                             // Immediate: #1
                             {.type = OPERAND_IMMEDIATE, .immediate = 1}}}};

  // movz x1,#5
  parsed_line_t line2 = {
      .type = LINE_INSTRUCTION,
      .instr = {.mnemonic = "movz",
                .mnemonic_tok = TOKEN_MOVZ,
                .instr_type = INSTR_DATA_PROCESSING,
                .operand_count = 2,
                .operands = {// Rd: x1
                             {.type = OPERAND_REGISTER,
                              .reg = {.reg_num = 1, .is_64bit = true}},
                             // Immediate: #5
                             {.type = OPERAND_IMMEDIATE, .immediate = 5}}}};

  // loop:
  parsed_line_t line3 = {.type = LINE_LABEL, .label = {.name = "loop"}};

  // mul x2,x1,x0
  parsed_line_t line4 = {
      .type = LINE_INSTRUCTION,
      .instr = {.mnemonic = "mul",
                .mnemonic_tok = TOKEN_MUL,
                .instr_type = INSTR_DATA_PROCESSING,
                .operand_count = 3,
                .operands = {// Rd: x2
                             {.type = OPERAND_REGISTER,
                              .reg = {.reg_num = 2, .is_64bit = true}},
                             // Rn: x1
                             {.type = OPERAND_REGISTER,
                              .reg = {.reg_num = 1, .is_64bit = true}},
                             // Rm: x0
                             {.type = OPERAND_REGISTER,
                              .reg = {.reg_num = 0, .is_64bit = true}}}}};

  // add x0,x2,#0
  parsed_line_t line5 = {
      .type = LINE_INSTRUCTION,
      .instr = {.mnemonic = "add",
                .mnemonic_tok = TOKEN_ADD,
                .instr_type = INSTR_DATA_PROCESSING,
                .operand_count = 3,
                .operands = {// Rd: x0
                             {.type = OPERAND_REGISTER,
                              .reg = {.reg_num = 0, .is_64bit = true}},
                             // Rn: x2
                             {.type = OPERAND_REGISTER,
                              .reg = {.reg_num = 2, .is_64bit = true}},
                             // Immediate: #0
                             {.type = OPERAND_IMMEDIATE, .immediate = 0}}}};

  // sub x1,x1,#1
  parsed_line_t line6 = {
      .type = LINE_INSTRUCTION,
      .instr = {.mnemonic = "sub",
                .mnemonic_tok = TOKEN_SUB,
                .instr_type = INSTR_DATA_PROCESSING,
                .operand_count = 3,
                .operands = {// Rd: x1
                             {.type = OPERAND_REGISTER,
                              .reg = {.reg_num = 1, .is_64bit = true}},
                             // Rn: x1
                             {.type = OPERAND_REGISTER,
                              .reg = {.reg_num = 1, .is_64bit = true}},
                             // Immediate: #1
                             {.type = OPERAND_IMMEDIATE, .immediate = 1}}}};

  // cmp x1,#0
  parsed_line_t line7 = {
      .type = LINE_INSTRUCTION,
      .instr = {.mnemonic = "cmp",
                .mnemonic_tok = TOKEN_CMP,
                .instr_type = INSTR_DATA_PROCESSING,
                .operand_count = 2,
                .operands = {// First operand: x1
                             {.type = OPERAND_REGISTER,
                              .reg = {.reg_num = 1, .is_64bit = true}},
                             // Second operand: #0
                             {.type = OPERAND_IMMEDIATE, .immediate = 0}}}};

  // b.ne loop
  parsed_line_t line8 = {
      .type = LINE_INSTRUCTION,
      .instr = {.mnemonic = "b.ne",
                .mnemonic_tok = TOKEN_B_NE,
                .instr_type = INSTR_BRANCH,
                .operand_count = 1,
                .operands = {// Branch target: loop (label)
                             {.type = OPERAND_LITERAL_ADDRESS,
                              .literal_address = 0x8}}}};

  // movz x3,#0x100
  parsed_line_t line9 = {
      .type = LINE_INSTRUCTION,
      .instr = {.mnemonic = "movz",
                .mnemonic_tok = TOKEN_MOVZ,
                .instr_type = INSTR_DATA_PROCESSING,
                .operand_count = 2,
                .operands = {// Rd: x3
                             {.type = OPERAND_REGISTER,
                              .reg = {.reg_num = 3, .is_64bit = true}},
                             // Immediate: #0x100
                             {.type = OPERAND_IMMEDIATE, .immediate = 0x100}}}};

  // str x2,[x3]
  parsed_line_t line10 = {
      .type = LINE_INSTRUCTION,
      .instr = {
          .mnemonic = "str",
          .mnemonic_tok = TOKEN_STR,
          .instr_type = INSTR_LOAD_STORE,
          .operand_count = 2,
          .operands = {// Rt: x2 (source register)
                       {.type = OPERAND_REGISTER,
                        .reg = {.reg_num = 2, .is_64bit = true}},
                       // Memory operand: [x3] (base register only, no offset)
                       {.type = OPERAND_MEMORY_UNSIGNED_OFFSET,
                        .memory = {
                            .base_reg = 3,  // x3
                            .offset_imm = 0 // implicit #0 offset
                        }}}}};

  // and x0, x0, x0
  parsed_line_t line11 = {
      .type = LINE_INSTRUCTION,
      .instr = {.mnemonic = "and",
                .mnemonic_tok = TOKEN_AND,
                .instr_type = INSTR_DATA_PROCESSING,
                .operand_count = 3,
                .operands = {// Rd: x0
                             {.type = OPERAND_REGISTER,
                              .reg = {.reg_num = 0, .is_64bit = true}},
                             // Rn: x0
                             {.type = OPERAND_REGISTER,
                              .reg = {.reg_num = 0, .is_64bit = true}},
                             // Rm: x0
                             {.type = OPERAND_REGISTER,
                              .reg = {.reg_num = 0, .is_64bit = true}}}}};

  // Assemble instructions (skipping label)
  u32 instr1 = assemble_instruction(&line1, 0x0); // movz x0,#1
  u32 instr2 = assemble_instruction(&line2, 0x4); // movz x1,#5
  // line3 is label, skip
  u32 instr4 = assemble_instruction(&line4, 0x8);    // mul x2,x1,x0
  u32 instr5 = assemble_instruction(&line5, 0xc);    // add x0,x2,#0
  u32 instr6 = assemble_instruction(&line6, 0x10);   // sub x1,x1,#1
  u32 instr7 = assemble_instruction(&line7, 0x14);   // cmp x1,#0
  u32 instr8 = assemble_instruction(&line8, 0x18);   // b.ne loop
  u32 instr9 = assemble_instruction(&line9, 0x1c);   // movz x3,#0x100
  u32 instr10 = assemble_instruction(&line10, 0x20); // str x2,[x3]
  u32 instr11 = assemble_instruction(&line11, 0x24); // and x0,x0,x0

  printf("movz x0,#1:        %08x\n", instr1);
  printf("movz x1,#5:        %08x\n", instr2);
  printf("mul x2,x1,x0:      %08x\n", instr4);
  printf("add x0,x2,#0:      %08x\n", instr5);
  printf("sub x1,x1,#1:      %08x\n", instr6);
  printf("cmp x1,#0:         %08x\n", instr7);
  printf("b.ne loop:         %08x\n", instr8);
  printf("movz x3,#0x100:    %08x\n", instr9);
  printf("str x2,[x3]:       %08x\n", instr10);
  printf("and x0,x0,x0:      %08x\n", instr11);

  FILE *in = fopen("prog.out", "wb");
  write_uint32_le(in, instr1);
  write_uint32_le(in, instr2);
  write_uint32_le(in, instr4);
  write_uint32_le(in, instr5);
  write_uint32_le(in, instr6);
  write_uint32_le(in, instr7);
  write_uint32_le(in, instr8);
  write_uint32_le(in, instr9);
  write_uint32_le(in, instr10);
  write_uint32_le(in, instr11);

  fclose(in);

  return EXIT_SUCCESS;
}
  */
