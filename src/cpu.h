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
    MODE_ABSOLUTE,
    MODE_ABSOLUTE_X,
    MODE_ABSOLUTE_Y,
    MODE_ACCUMULATOR,
    MODE_IMMEDIATE,
    MODE_IMPLIED,
    MODE_INDEXED_INDIRECT,
    MODE_INDIRECT,
    MODE_INDIRECT_INDEXED,
    MODE_RELATIVE,
    MODE_ZERO_PAGE,
    MODE_ZERO_PAGE_X,
    MODE_ZERO_PAGE_Y,
} addressing_mode_t;

typedef enum {
    ADC,
    AHX,
    ALR,
    ANC,
    AND,
    ARR,
    ASL,
    AXS,
    BCC,
    BCS,
    BEQ,
    BIT,
    BMI,
    BNE,
    BPL,
    BRK,
    BVC,
    BVS,
    CLC,
    CLD,
    CLI,
    CLV,
    CMP,
    CPX,
    CPY,
    DCP,
    DEC,
    DEX,
    DEY,
    EOR,
    INC,
    INX,
    INY,
    ISB,
    JMP,
    JSR,
    KIL,
    LAS,
    LAX,
    LDA,
    LDX,
    LDY,
    LSR,
    NOP,
    ORA,
    PHA,
    PHP,
    PLA,
    PLP,
    RLA,
    ROL,
    ROR,
    RRA,
    RTI,
    RTS,
    SAX,
    SBC,
    SEC,
    SED,
    SEI,
    SHX,
    SHY,
    SLO,
    SRE,
    STA,
    STX,
    STY,
    TAS,
    TAX,
    TAY,
    TSX,
    TXA,
    TXS,
    TYA,
    XAA,
} instruction_t;

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
} cpu_t;

void cpu_reset(cpu_t *cpu);

uint8_t cpu_read(struct _nes_t *_nes, uint16_t addr);
void cpu_write(struct _nes_t *_nes, uint16_t addr, uint8_t val);

#endif // WASM_NES_CPU_H
