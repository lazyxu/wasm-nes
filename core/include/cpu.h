//
// Created by meteor on 2018/11/8.
//

#ifndef WASM_NES_CPU_H
#define WASM_NES_CPU_H

#include "port.h"

typedef enum {
    INT_NMI,
    INT_IRQ,
    INT_RESET,
    INT_NONE,
} interrupt_t;

typedef enum {
    ABSOLUTE,
    ABSOLUTE_X,
    ABSOLUTE_Y,
    ACCUMULATOR,
    IMMEDIATE,
    IMPLIED,
    INDEXED_INDIRECT,
    INDIRECT,
    INDIRECT_INDEXED,
    RELATIVE,
    ZERO_PAGE,
    ZERO_PAGE_X,
    ZERO_PAGE_Y,
} addressing_mode_t;

#define OPCODE_LIST\
    OPCODE(ADC)\
    OPCODE(AHX)\
    OPCODE(ALR)\
    OPCODE(ANC)\
    OPCODE(AND)\
    OPCODE(ARR)\
    OPCODE(ASL)\
    OPCODE(AXS)\
    OPCODE(BCC)\
    OPCODE(BCS)\
    OPCODE(BEQ)\
    OPCODE(BIT)\
    OPCODE(BMI)\
    OPCODE(BNE)\
    OPCODE(BPL)\
    OPCODE(BRK)\
    OPCODE(BVC)\
    OPCODE(BVS)\
    OPCODE(CLC)\
    OPCODE(CLD)\
    OPCODE(CLI)\
    OPCODE(CLV)\
    OPCODE(CMP)\
    OPCODE(CPX)\
    OPCODE(CPY)\
    OPCODE(DCP)\
    OPCODE(DEC)\
    OPCODE(DEX)\
    OPCODE(DEY)\
    OPCODE(EOR)\
    OPCODE(INC)\
    OPCODE(INX)\
    OPCODE(INY)\
    OPCODE(ISB)\
    OPCODE(JMP)\
    OPCODE(JSR)\
    OPCODE(KIL)\
    OPCODE(LAS)\
    OPCODE(LAX)\
    OPCODE(LDA)\
    OPCODE(LDX)\
    OPCODE(LDY)\
    OPCODE(LSR)\
    OPCODE(NOP)\
    OPCODE(ORA)\
    OPCODE(PHA)\
    OPCODE(PHP)\
    OPCODE(PLA)\
    OPCODE(PLP)\
    OPCODE(RLA)\
    OPCODE(ROL)\
    OPCODE(ROR)\
    OPCODE(RRA)\
    OPCODE(RTI)\
    OPCODE(RTS)\
    OPCODE(SAX)\
    OPCODE(SBC)\
    OPCODE(SEC)\
    OPCODE(SED)\
    OPCODE(SEI)\
    OPCODE(SHX)\
    OPCODE(SHY)\
    OPCODE(SLO)\
    OPCODE(SRE)\
    OPCODE(STA)\
    OPCODE(STX)\
    OPCODE(STY)\
    OPCODE(TAS)\
    OPCODE(TAX)\
    OPCODE(TAY)\
    OPCODE(TSX)\
    OPCODE(TXA)\
    OPCODE(TXS)\
    OPCODE(TYA)\
    OPCODE(XAA)

#define OPCODE(arg1) arg1,
typedef enum {
    OPCODE_LIST
} opcode_t;

#undef OPCODE

#define OPCODE(arg1) #arg1,
/**
 * Names of the opcodes.
 */
static const char * const g_opcode_names[] =
{
  OPCODE_LIST
};

#undef OPCODE

typedef struct {
    struct _nes_t *nes;
    uint16_t pc;
    uint8_t sp;
    uint8_t a;
    uint8_t x;
    uint8_t y;
    uint8_t ps;
    uint8_t ram[0x800];
    interrupt_t interrupt;
    addressing_mode_t addressing_mode;
    uint16_t cycles;
    uint16_t stall;
} cpu_t;

uint8_t cpu_read(struct _nes_t *_nes, uint16_t addr);
void cpu_write(struct _nes_t *_nes, uint16_t addr, uint8_t val);

void cpu_reset(cpu_t *cpu);

#ifdef TEST_CPU
int cpu_status(FILE *stream, cpu_t *cpu);
#endif // TEST_CPU

uint8_t cpu_step(cpu_t *cpu);

#endif // WASM_NES_CPU_H
