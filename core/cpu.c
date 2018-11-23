//
// Created by meteor on 2018/11/8.
//

#include "cpu.h"
#include "controller.h"
#include "nes.h"
#include "ppu.h"

/**
 * the opcode for each instruction
 */
const opcode_t g_opcode_list[] = {
    BRK, ORA, KIL, SLO, NOP, ORA, ASL, SLO, PHP, ORA, ASL, ANC, NOP, ORA, ASL, SLO, BPL, ORA, KIL, SLO, NOP, ORA,
    ASL, SLO, CLC, ORA, NOP, SLO, NOP, ORA, ASL, SLO, JSR, AND, KIL, RLA, BIT, AND, ROL, RLA, PLP, AND, ROL, ANC,
    BIT, AND, ROL, RLA, BMI, AND, KIL, RLA, NOP, AND, ROL, RLA, SEC, AND, NOP, RLA, NOP, AND, ROL, RLA, RTI, EOR,
    KIL, SRE, NOP, EOR, LSR, SRE, PHA, EOR, LSR, ALR, JMP, EOR, LSR, SRE, BVC, EOR, KIL, SRE, NOP, EOR, LSR, SRE,
    CLI, EOR, NOP, SRE, NOP, EOR, LSR, SRE, RTS, ADC, KIL, RRA, NOP, ADC, ROR, RRA, PLA, ADC, ROR, ARR, JMP, ADC,
    ROR, RRA, BVS, ADC, KIL, RRA, NOP, ADC, ROR, RRA, SEI, ADC, NOP, RRA, NOP, ADC, ROR, RRA, NOP, STA, NOP, SAX,
    STY, STA, STX, SAX, DEY, NOP, TXA, XAA, STY, STA, STX, SAX, BCC, STA, KIL, AHX, STY, STA, STX, SAX, TYA, STA,
    TXS, TAS, SHY, STA, SHX, AHX, LDY, LDA, LDX, LAX, LDY, LDA, LDX, LAX, TAY, LDA, TAX, LAX, LDY, LDA, LDX, LAX,
    BCS, LDA, KIL, LAX, LDY, LDA, LDX, LAX, CLV, LDA, TSX, LAS, LDY, LDA, LDX, LAX, CPY, CMP, NOP, DCP, CPY, CMP,
    DEC, DCP, INY, CMP, DEX, AXS, CPY, CMP, DEC, DCP, BNE, CMP, KIL, DCP, NOP, CMP, DEC, DCP, CLD, CMP, NOP, DCP,
    NOP, CMP, DEC, DCP, CPX, SBC, NOP, ISB, CPX, SBC, INC, ISB, INX, SBC, NOP, SBC, CPX, SBC, INC, ISB, BEQ, SBC,
    KIL, ISB, NOP, SBC, INC, ISB, SED, SBC, NOP, ISB, NOP, SBC, INC, ISB,
};

/**
 * the addressing mode for each instruction
 */
const uint8_t g_mode[] = {
    5, 6, 5, 6, 10, 10, 10, 10, 5, 4, 3, 4, 0, 0, 0, 0, 9, 8, 5, 8, 11, 11, 11, 11, 5, 2, 5, 2, 1, 1, 1, 1,
    0, 6, 5, 6, 10, 10, 10, 10, 5, 4, 3, 4, 0, 0, 0, 0, 9, 8, 5, 8, 11, 11, 11, 11, 5, 2, 5, 2, 1, 1, 1, 1,
    5, 6, 5, 6, 10, 10, 10, 10, 5, 4, 3, 4, 0, 0, 0, 0, 9, 8, 5, 8, 11, 11, 11, 11, 5, 2, 5, 2, 1, 1, 1, 1,
    5, 6, 5, 6, 10, 10, 10, 10, 5, 4, 3, 4, 7, 0, 0, 0, 9, 8, 5, 8, 11, 11, 11, 11, 5, 2, 5, 2, 1, 1, 1, 1,
    4, 6, 4, 6, 10, 10, 10, 10, 5, 4, 5, 4, 0, 0, 0, 0, 9, 8, 5, 8, 11, 11, 12, 12, 5, 2, 5, 2, 1, 1, 2, 2,
    4, 6, 4, 6, 10, 10, 10, 10, 5, 4, 5, 4, 0, 0, 0, 0, 9, 8, 5, 8, 11, 11, 12, 12, 5, 2, 5, 2, 1, 1, 2, 2,
    4, 6, 4, 6, 10, 10, 10, 10, 5, 4, 5, 4, 0, 0, 0, 0, 9, 8, 5, 8, 11, 11, 11, 11, 5, 2, 5, 2, 1, 1, 1, 1,
    4, 6, 4, 6, 10, 10, 10, 10, 5, 4, 5, 4, 0, 0, 0, 0, 9, 8, 5, 8, 11, 11, 11, 11, 5, 2, 5, 2, 1, 1, 1, 1,
};

/**
 * the size of each instruction in bytes
 */
const uint8_t g_size[] = {
    1, 2, 1, 2, 2, 2, 2, 2, 1, 2, 1, 2, 3, 3, 3, 3, 2, 2, 1, 2, 2, 2, 2, 2, 1, 3, 1, 3, 3, 3, 3, 3, 3, 2, 1, 2, 2,
    2, 2, 2, 1, 2, 1, 2, 3, 3, 3, 3, 2, 2, 1, 2, 2, 2, 2, 2, 1, 3, 1, 3, 3, 3, 3, 3, 1, 2, 1, 2, 2, 2, 2, 2, 1, 2,
    1, 2, 3, 3, 3, 3, 2, 2, 1, 2, 2, 2, 2, 2, 1, 3, 1, 3, 3, 3, 3, 3, 1, 2, 1, 2, 2, 2, 2, 2, 1, 2, 1, 2, 3, 3, 3,
    3, 2, 2, 1, 2, 2, 2, 2, 2, 1, 3, 1, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 1, 2, 3, 3, 3, 3, 2, 2, 1, 2,
    2, 2, 2, 2, 1, 3, 1, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 1, 2, 3, 3, 3, 3, 2, 2, 1, 2, 2, 2, 2, 2, 1,
    3, 1, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 1, 2, 3, 3, 3, 3, 2, 2, 1, 2, 2, 2, 2, 2, 1, 3, 1, 3, 3, 3,
    3, 3, 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 1, 2, 3, 3, 3, 3, 2, 2, 1, 2, 2, 2, 2, 2, 1, 3, 1, 3, 3, 3, 3, 3,
};

/**
 * the number of cycles used by each instruction, not including conditional cycles
 */
const uint8_t g_cycle[] = {
    7, 6, 2, 8, 3, 3, 5, 5, 3, 2, 2, 2, 4, 4, 6, 6, 2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7, 6, 6, 2, 8, 3,
    3, 5, 5, 4, 2, 2, 2, 4, 4, 6, 6, 2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7, 6, 6, 2, 8, 3, 3, 5, 5, 3, 2,
    2, 2, 3, 4, 6, 6, 2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7, 6, 6, 2, 8, 3, 3, 5, 5, 4, 2, 2, 2, 5, 4, 6,
    6, 2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7, 2, 6, 2, 6, 3, 3, 3, 3, 2, 2, 2, 2, 4, 4, 4, 4, 2, 6, 2, 6,
    4, 4, 4, 4, 2, 5, 2, 5, 5, 5, 5, 5, 2, 6, 2, 6, 3, 3, 3, 3, 2, 2, 2, 2, 4, 4, 4, 4, 2, 5, 2, 5, 4, 4, 4, 4, 2,
    4, 2, 4, 4, 4, 4, 4, 2, 6, 2, 8, 3, 3, 5, 5, 2, 2, 2, 2, 4, 4, 6, 6, 2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4,
    7, 7, 2, 6, 2, 8, 3, 3, 5, 5, 2, 2, 2, 2, 4, 4, 6, 6, 2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
};

/**
 * the number of cycles used by each instruction when a page is crossed
 */
const uint8_t g_page_cycle[] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 0, 0, 0, 0, 0,
    1, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0,
};

#define RAM (cpu->ram)
#define CYCLES (cpu->cycles)

/* ---------------------------------------------------- register ---------------------------------------------------- */
#define PC (cpu->pc)
#define SP (cpu->sp)
#define A (cpu->a)
#define X (cpu->x)
#define Y (cpu->y)
#define PS (cpu->ps)

#define C_FLAG (1 << 0)
#define Z_FLAG (1 << 1)
#define I_FLAG (1 << 2)
#define D_FLAG (1 << 3)
#define B_FLAG (1 << 4)
#define R_FLAG (1 << 5)
#define U_FLAG R_FLAG
#define O_FLAG (1 << 6)
#define V_FLAG O_FLAG
#define N_FLAG (1 << 7)

#define C ((PS >> 0) & 1)
#define Z ((PS >> 1) & 1)
#define I ((PS >> 2) & 1)
#define D ((PS >> 3) & 1)
#define B ((PS >> 4) & 1)
#define R ((PS >> 5) & 1)
#define U R
#define O ((PS >> 6) & 1)
#define V O
#define N ((PS >> 7) & 1)

#define SET_FLAG(v)                                                                                                    \
    do {                                                                                                               \
        PS |= (v);                                                                                                     \
    } while (0)
#define CLR_FLAG(v)                                                                                                    \
    do {                                                                                                               \
        PS &= ~(v);                                                                                                    \
    } while (0)

#define TST_FLAG(c, v)                                                                                                 \
    do {                                                                                                               \
        if (c) {                                                                                                       \
            PS |= (v);                                                                                                 \
        } else {                                                                                                       \
            PS &= ~(v);                                                                                                \
        }                                                                                                              \
    } while (0)
#define CHK_FLAG(v) (PS & (v))

/**
 * set the zero flag if the argument is zero
 */
#define SET_Z_FLAG_IF_ZERO(v)                                                                                          \
    do {                                                                                                               \
        if ((v) == 0)                                                                                                  \
            SET_FLAG(Z_FLAG);                                                                                          \
        else                                                                                                           \
            CLR_FLAG(Z_FLAG);                                                                                          \
    } while (0)

/**
 * set the negative flag if the argument is negative (high bit is set)
 */
#define SET_N_FLAG_IF_NEGATIVE(v)                                                                                      \
    do {                                                                                                               \
        if ((v & N_FLAG) != 0)                                                                                         \
            SET_FLAG(N_FLAG);                                                                                          \
        else                                                                                                           \
            CLR_FLAG(N_FLAG);                                                                                          \
    } while (0)

/**
 * setZN sets the zero flag and the negative flag
 */
#define SET_ZN_FLAG(v)                                                                                                 \
    do {                                                                                                               \
        PS &= ~(Z_FLAG | N_FLAG);                                                                                      \
        if (v == 0)                                                                                                    \
            SET_FLAG(Z_FLAG);                                                                                          \
        else                                                                                                           \
            PS |= v & N_FLAG;                                                                                          \
    } while (0)

/* ----------------------------------------------------- memory ----------------------------------------------------- */

uint8_t cpu_read(nes_t *nes, uint16_t addr) {
    TRACE_MSG("cpu read: %x\n", addr);
    ASSERT(nes != NULL);
    ASSERT(nes->ppu != NULL);
    ASSERT(nes->apu != NULL);
    if (addr < 0x2000) {
        ASSERT(nes->cpu != NULL);
        ASSERT(nes->cpu->ram != NULL);
        return nes->cpu->ram[addr & 0x7ff];
    } else if (addr < 0x4000) {
        return ppu_read_register(nes->ppu, 0x2000 + ((addr - 0x2000) & 7));
    } else if (addr < 0x4020) {
        switch (addr) {
        case 0x4014: return ppu_read_register(nes->ppu, addr);
        case 0x4015: return apu_read_register(nes->apu, addr);
        case 0x4016: return controller_read(&nes->controller[0]);
        case 0x4017: return controller_read(&nes->controller[1]);
        default: DEBUG_MSG("unhandled I/O Registers II read at addr: %X\n", addr); return 0;
        }
    } else if (addr < 0x6000) {
        DEBUG_MSG("TODO: I/O registers\n");
        return 0;
    } else if (addr >= 0x6000) {
        ASSERT(nes->mmc != NULL);
        ASSERT(nes->mmc->mapper != NULL);
        ASSERT(nes->mmc->mapper->mapper_cpu_read != NULL);
        return nes->mmc->mapper->mapper_cpu_read(nes->mmc, addr);
    }
    ASSERT(false);
    return 0;
}

void cpu_write(nes_t *nes, uint16_t addr, uint8_t val) {
    DEBUG_MSG("cpu write: %x <- %x\n", addr, val);
    ASSERT(nes != NULL);
    ASSERT(nes->ppu != NULL);
    ASSERT(nes->apu != NULL);
    if (addr < 0x2000) {
        ASSERT(nes->cpu != NULL);
        ASSERT(nes->cpu->ram != NULL);
        nes->cpu->ram[addr & 0x7ff] = val;
    } else if (addr < 0x4000) {
        DEBUG_MSG("unhandled I/O Registers II write at addr: %X <- %X\n", addr, val);
        ppu_write_register(nes->ppu, 0x2000 | (addr & 7), val);
    } else if (addr < 0x4020) {
        switch (addr) {
        case 0x4014: ppu_write_register(nes->ppu, addr, val);
        case 0x4015: apu_write_register(nes->apu, addr, val);
        case 0x4016: controller_write(&nes->controller[0], val);
        case 0x4017: controller_write(&nes->controller[1], val);
        default: DEBUG_MSG("unhandled I/O Registers II write at addr: %X <- %X\n", addr, val);
        }
    } else if (addr < 0x6000) {
        DEBUG_MSG("TODO: I/O registers\n");
    } else if (addr >= 0x6000) {
        nes->mmc->mapper->mapper_cpu_write(nes->mmc, addr, val);
    }
}

/**
 * read two bytes using read to return a double-word val
 */
uint16_t cpu_read16(cpu_t *cpu, uint16_t addr) {
    ASSERT(cpu->nes != NULL);
    uint8_t lo = cpu_read(cpu->nes, addr);
    uint8_t hi = cpu_read(cpu->nes, addr + 1);
    return ((hi & 0xff) << 8) | (lo & 0xff);
}

/**
 *  emulate a 6502 bug that caused the low byte to wrap without incrementing the high byte
 */
uint16_t cpu_read16bug(cpu_t *cpu, uint16_t addr) {
    ASSERT(cpu != NULL);
    ASSERT(cpu->nes != NULL);
    uint16_t a = addr;
    uint16_t b = (a & 0xFF00) | ((a + 1) & 0xff);
    uint16_t lo = cpu_read(cpu->nes, a);
    uint16_t hi = cpu_read(cpu->nes, b);
    return (hi << 8) | lo;
}

#define STACK_TOP 0x100
static void push(cpu_t *cpu, uint8_t val) {
    RAM[(SP--) | STACK_TOP] = (val);
}
static uint8_t pop(cpu_t *cpu) {
    return RAM[(++SP) | STACK_TOP];
}
static void push16(cpu_t *cpu, uint16_t val) {
    push(cpu, val >> 8);
    push(cpu, val);
}
static uint16_t pop16(cpu_t *cpu) {
    uint8_t lo = pop(cpu);
    uint8_t hi = pop(cpu);
    return (hi << 8) | lo;
}
#define PUSH(v) push(cpu, v)
#define POP() pop(cpu)
#define PUSHW(v) push16(cpu, v)
#define POPW() pop16(cpu)

#define READ(addr) cpu_read(cpu->nes, addr)
#define READW(addr) cpu_read16(cpu, addr)
#define READW_BUG(addr) cpu_read16bug(cpu, addr)

#define WRITE(addr, byte) cpu_write(cpu->nes, addr, byte)

/**
 * returns true if the two addresses reference different pages
 */
#define PAGE_DIFF(a, b) ((a & 0xFF00) != (b & 0xFF00))

/**
 * add a cycle for taking a branch and adds another cycle if the branch jumps to a new page
 */
#define ADD_BRANCH_CYCLES(pc, addr)                                                                                    \
    do {                                                                                                               \
        CYCLES += PAGE_DIFF(pc, addr) ? 2 : 1;                                                                         \
    } while (0)
/**
 * compares a and b, set flags
 */
#define COMPARE(a, b)                                                                                                  \
    do {                                                                                                               \
        SET_ZN_FLAG(a - b);                                                                                            \
        if (a >= b)                                                                                                    \
            SET_FLAG(C_FLAG);                                                                                          \
        else                                                                                                           \
            CLR_FLAG(C_FLAG);                                                                                          \
    } while (0)

/* --------------------------------------------------- interrupt ---------------------------------------------------- */
#define NMI_VECTOR 0xFFFA
#define RST_VECTOR 0xFFFC
#define IRQ_VECTOR 0xFFFE

#define SET_IRQ(irq)                                                                                                   \
    do {                                                                                                               \
        cpu->interrupt = irq;                                                                                          \
    } while (0)
#define INTERRUPT()                                                                                                    \
    do {                                                                                                               \
        PUSHW(PC);                                                                                                     \
        CLR_FLAG(B_FLAG);                                                                                              \
        PUSH(PS);                                                                                                      \
        SET_FLAG(I_FLAG);                                                                                              \
        CYCLES += 7;                                                                                                   \
    } while (0)

/**
 * causes a Non-Maskable Interrupt to occur on the next cycle
 */
void cpu_nmi(cpu_t *cpu) {
    SET_IRQ(INT_NMI);
}

/**
 * causes an IRQ interrupt to occur on the next cycle
 */
void cpu_irq(cpu_t *cpu) {
    if (!CHK_FLAG(I_FLAG)) {
        SET_IRQ(INT_IRQ);
    }
}

/**
 * NMI - Non-Maskable Interrupt
 * "Non-maskable" means that no state inside the CPU can prevent the NMI from being processed as an interrupt.
 * However, most boards that use a 6502 CPU's /NMI line allow the CPU to disable the generation of /NMI signals by
 * writing to a memory-mapped I/O device. In the case of the NES, the /NMI line is connected to the NES PPU and is used
 * to detect vertical blanking.
 */
#define NMI()                                                                                                          \
    do {                                                                                                               \
        INTERRUPT();                                                                                                   \
        PC = READW(NMI_VECTOR);                                                                                        \
    } while (0)

/**
 * RESET - RESET Interrupt
 */
#define RESET()                                                                                                        \
    do {                                                                                                               \
        INTERRUPT();                                                                                                   \
        PC = READW(RST_VECTOR);                                                                                        \
    } while (0)

/**
 *  IRQ - IRQ Interrupt
 */
#define IRQ()                                                                                                          \
    do {                                                                                                               \
        INTERRUPT();                                                                                                   \
        PC = READW(IRQ_VECTOR);                                                                                        \
    } while (0)

/* ------------------------------------------------- main functions ------------------------------------------------- */

void cpu_reset(cpu_t *cpu) {
    DEBUG_MSG("cpu reset\n");
    ASSERT(cpu != NULL);
    PC = READW(RST_VECTOR);
    SP = 0xFD;
    A = 0;
    X = 0;
    Y = 0;
    PS = 0x24;
    ASSERT(RAM != NULL);
    memset(RAM, 0, sizeof(RAM));
    SET_IRQ(INT_NONE);
}

#ifdef DEBUG
/**
 * print the current CPU state
 */
int cpu_status(FILE *stream, cpu_t *cpu) {
    ASSERT(stream != NULL);
    uint8_t opcode = READ(PC);
    uint8_t size = g_size[opcode];
    const char *const _name = g_opcode_names[g_opcode_list[opcode]];
    const char *name = _name;
    if (strcmp(_name, "NOP") == 0 && opcode != 0xEA) {
        name = "*NOP";
    }
    if (strcmp(_name, "LAX") == 0) {
        name = "*LAX";
    }
    if (strcmp(_name, "SAX") == 0) {
        name = "*SAX";
    }
    if (strcmp(_name, "SBC") == 0 && opcode == 0xEB) {
        name = "*SBC";
    }
    if (strcmp(_name, "DCP") == 0) {
        name = "*DCP";
    }
    if (strcmp(_name, "ISB") == 0) {
        name = "*ISB";
    }
    if (strcmp(_name, "SLO") == 0) {
        name = "*SLO";
    }
    if (strcmp(_name, "RLA") == 0) {
        name = "*RLA";
    }
    if (strcmp(_name, "SRE") == 0) {
        name = "*SRE";
    }
    if (strcmp(_name, "RRA") == 0) {
        name = "*RRA";
    }
    switch (size) {
    case 1:
        return fprintf(stream, "%04X  %02X       %4s             A:%02X X:%02X Y:%02X P:%02X SP:%02X CYC:%3d\n", PC,
                       opcode, name, A, X, Y, PS, SP, (CYCLES * 3) % 341);
        break;
    case 2:
        return fprintf(stream, "%04X  %02X %02X    %4s             A:%02X X:%02X Y:%02X P:%02X SP:%02X CYC:%3d\n", PC,
                       opcode, READ(PC + 1), name, A, X, Y, PS, SP, (CYCLES * 3) % 341);
        break;
    case 3:
        return fprintf(stream, "%04X  %02X %02X %02X %4s             A:%02X X:%02X Y:%02X P:%02X SP:%02X CYC:%3d\n", PC,
                       opcode, READ(PC + 1), READ(PC + 2), name, A, X, Y, PS, SP, (CYCLES * 3) % 341);
        break;
    default: ASSERT(false); break;
    }
    ASSERT(false);
    return 0;
}
#endif // DEBUG

uint8_t cpu_step(cpu_t *cpu) {
    DEBUG_MSG("PC: %X\n", PC);
    if (cpu->stall > 0) {
        cpu->stall--;
        return 1;
    }
    uint16_t cycles = CYCLES;

    // interrupt
    switch (cpu->interrupt) {
    case INT_IRQ: IRQ(); break;
    case INT_NMI: NMI(); break;
    default: break;
    }
    SET_IRQ(INT_NONE);
    uint8_t instruction = READ(PC);

    addressing_mode_t mode = g_mode[instruction];

    // addressing mode
    uint16_t address;
    bool page_crossed;
    uint8_t offset;
    switch (mode) {
    case ABSOLUTE: address = READW(PC + 1); break;
    case ABSOLUTE_X:
        address = READW(PC + 1) + X;
        page_crossed = PAGE_DIFF(address - X, address);
        break;
    case ABSOLUTE_Y:
        address = READW(PC + 1) + Y;
        page_crossed = PAGE_DIFF(address - Y, address);
        break;
    case ACCUMULATOR: address = 0; break;
    case IMMEDIATE: address = PC + 1; break;
    case IMPLIED: address = 0; break;
    case INDEXED_INDIRECT: address = READW_BUG((READ(PC + 1) + X) & 0xff); break;
    case INDIRECT: address = READW_BUG(READW(PC + 1)); break;
    case INDIRECT_INDEXED:
        address = READW_BUG(READ(PC + 1)) + Y;
        page_crossed = PAGE_DIFF(address - Y, address);
        break;
    case RELATIVE:
        offset = READ(PC + 1);
        address = offset < 0x80 ? PC + 2 + offset : PC + 2 + offset - 0x100;
        break;
    case ZERO_PAGE: address = READ(PC + 1); break;
    case ZERO_PAGE_X: address = (READ(PC + 1) + X) & 0xff; break;
    case ZERO_PAGE_Y: address = (READ(PC + 1) + Y) & 0xff; break;
    }
    if (page_crossed) {
        CYCLES += g_page_cycle[instruction];
    }

    // instructions
    PC += g_size[instruction];
    uint16_t pc = PC;
    CYCLES += g_cycle[instruction];
    uint8_t value, a, b, c;
    switch (g_opcode_list[instruction]) {
    case ADC: // Add with Carry
        a = A;
        b = READ(address);
        c = C;
        A = a + b + c;
        SET_ZN_FLAG(A);
        TST_FLAG(((uint16_t)a + (uint16_t)b + (uint16_t)c) > 0xFF, C_FLAG);
        TST_FLAG(((a ^ b) & 0x80) == 0 && ((a ^ A) & 0x80) != 0, V_FLAG);
    case AHX: ASSERT(false); break;
    case ALR: ASSERT(false); break;
    case ANC: ASSERT(false); break;
    case AND: // Logical AND
        A = A & READ(address);
        SET_ZN_FLAG(A);
        break;
    case ARR: ASSERT(false); break;
    case ASL: // Arithmetic Shift Left
        if (mode == ACCUMULATOR) {
            TST_FLAG((A >> 7) & 1, C_FLAG);
            A <<= 1;
            SET_ZN_FLAG(A);
        } else {
            value = READ(address);
            TST_FLAG((value >> 7) & 1, C_FLAG);
            value <<= 1;
            WRITE(address, value);
            SET_ZN_FLAG(value);
        }
        break;
    case AXS: ASSERT(false); break;
    case BCC: // Branch if Carry Clear
        if (C == 0) {
            PC = address;
            ADD_BRANCH_CYCLES(pc, address);
        }
        break;
    case BCS: // Branch if Carry Set
        if (C == 1) {
            PC = address;
            ADD_BRANCH_CYCLES(pc, address);
        }
        break;
    case BEQ: // Branch if Equal
        if (Z == 1) {
            PC = address;
            ADD_BRANCH_CYCLES(pc, address);
        }
        break;
    case BIT: // Bit Test
        value = READ(address);
        TST_FLAG((value >> 6) & 1, V_FLAG);
        SET_Z_FLAG_IF_ZERO(value & A);
        SET_N_FLAG_IF_NEGATIVE(value);
        break;
    case BMI: // Branch if Minus
        if (N == 1) {
            PC = address;
            ADD_BRANCH_CYCLES(pc, address);
        }
        break;
    case BNE: // Branch if Not Equal
        if (Z == 0) {
            PC = address;
            ADD_BRANCH_CYCLES(pc, address);
        }
        break;
    case BPL: // Branch if Positive
        if (N == 0) {
            PC = address;
            ADD_BRANCH_CYCLES(pc, address);
        }
        break;
    case BRK: // Force Interrupt
        PUSHW(PC);
        PUSH(PS | 0x10);
        SET_FLAG(I_FLAG);
        PC = READW(IRQ_VECTOR);
        break;
    case BVC: // Branch if Overflow Clear
        if (V == 0) {
            PC = address;
            ADD_BRANCH_CYCLES(pc, address);
        }
        break;
    case BVS: // Branch if Overflow Set
        if (V == 1) {
            PC = address;
            ADD_BRANCH_CYCLES(pc, address);
        }
        break;
    case CLC: // Clear Carry Flag
        CLR_FLAG(C_FLAG);
        break;
    case CLD: // Clear Decimal Mode
        CLR_FLAG(D_FLAG);
        break;
    case CLI: // Clear Interrupt Disable
        CLR_FLAG(I_FLAG);
        break;
    case CLV: // Clear Overflow Flag
        CLR_FLAG(V_FLAG);
        break;
    case CMP: // Compare
        value = READ(address);
        COMPARE(A, value);
        break;
    case CPX: // Compare X Register
        value = READ(address);
        COMPARE(X, value);
        break;
    case CPY: // Compare Y Register
        value = READ(address);
        COMPARE(Y, value);
        break;
    case DCP: // DEC CMP
        value = READ(address) - 1;
        WRITE(address, value);
        SET_ZN_FLAG(value);
        COMPARE(A, READ(address));
        break;
    case DEC: // Decrement Memory
        value = READ(address) - 1;
        WRITE(address, value);
        SET_ZN_FLAG(value);
        break;
    case DEX: // Decrement X Register
        X -= 1;
        SET_ZN_FLAG(X);
        break;
    case DEY: // Decrement Y Register
        Y -= 1;
        SET_ZN_FLAG(Y);
        break;
    case EOR: // Exclusive OR
        A ^= READ(address);
        SET_ZN_FLAG(A);
        break;
    case INC: // Increment Memory
        value = READ(address) + 1;
        WRITE(address, value);
        SET_ZN_FLAG(value);
        break;
    case INX: // Increment X Register
        X += 1;
        SET_ZN_FLAG(X);
        break;
    case INY: // Increment Y Register
        Y += 1;
        SET_ZN_FLAG(Y);
        break;
    case ISB: // INC SBC
        value = READ(address) + 1;
        WRITE(address, value);
        SET_ZN_FLAG(value);
        a = A;
        b = READ(address);
        c = C;
        A = a - b - (1 - c);
        SET_ZN_FLAG(A);
        TST_FLAG(((uint16_t)a - (uint16_t)b - (uint16_t)(1 - c)) >= 0, C_FLAG);
        TST_FLAG(((a ^ b) & 0x80) != 0 && ((a ^ A) & 0x80) != 0, V_FLAG);
        break;
    case JMP: // Jump
        PC = address;
        break;
    case JSR: // Jump to Subroutine
        PUSHW(PC - 1);
        PC = address;
        break;
    case KIL: ASSERT(false); break;
    case LAS: ASSERT(false); break;
    case LAX:
        value = READ(address);
        A = value;
        X = value;
        SET_ZN_FLAG(value);
        break;
    case LDA: // Load Accumulator
        A = READ(address);
        SET_ZN_FLAG(A);
        break;
    case LDX: // Load X Register
        X = READ(address);
        SET_ZN_FLAG(X);
        break;
    case LDY: // Load Y Register
        Y = READ(address);
        SET_ZN_FLAG(Y);
        break;
    case LSR: // Logical Shift Right
        if (mode == ACCUMULATOR) {
            TST_FLAG(A & 1, C_FLAG);
            A >>= 1;
            SET_ZN_FLAG(A);
        } else {
            value = READ(address);
            TST_FLAG(value & 1, C_FLAG);
            value >>= 1;
            WRITE(address, value);
            SET_ZN_FLAG(value);
        }
        break;
    case NOP: // No Operation
        break;
    case ORA: // Logical Inclusive OR
        A |= READ(address);
        SET_ZN_FLAG(A);
        break;
    case PHA: // Push Accumulator
        PUSH(A);
        break;
    case PHP: // Push Processor Status
        PUSH(PS | 0x10);
        break;
    case PLA: // Pull Accumulator
        A = POP();
        SET_ZN_FLAG(A);
        break;
    case PLP: // Pull Processor Status
        PS = (POP() & 0xEF) | 0x20;
        break;
    case RLA: // ROL AND
        c = C;
        if (mode == ACCUMULATOR) {
            TST_FLAG((A >> 7) & 1, C_FLAG);
            A = (A << 1) | c;
            SET_ZN_FLAG(A);
        } else {
            value = READ(address);
            TST_FLAG((value >> 7) & 1, C_FLAG);
            value = (value << 1) | c;
            WRITE(address, value);
            SET_ZN_FLAG(value);
        }
        A &= READ(address);
        SET_ZN_FLAG(A);
        break;
    case ROL: // Rotate Left
        c = C;
        if (mode == ACCUMULATOR) {
            TST_FLAG((A >> 7) & 1, C_FLAG);
            A = (A << 1) | c;
            SET_ZN_FLAG(A);
        } else {
            value = READ(address);
            TST_FLAG((value >> 7) & 1, C_FLAG);
            value = (value << 1) | c;
            WRITE(address, value);
            SET_ZN_FLAG(value);
        }
        break;
    case ROR: // Rotate Right
        c = C;
        if (mode == ACCUMULATOR) {
            TST_FLAG(A & 1, C_FLAG);
            A = (A >> 1) | (c << 7);
            SET_ZN_FLAG(A);
        } else {
            value = READ(address);
            TST_FLAG(value & 1, C_FLAG);
            value = (value >> 1) | (c << 7);
            WRITE(address, value);
            SET_ZN_FLAG(value);
        }
        break;
    case RRA: // ROR ADC
        c = C;
        if (mode == ACCUMULATOR) {
            TST_FLAG(A & 1, C_FLAG);
            A = (A >> 1) | (c << 7);
            SET_ZN_FLAG(A);
        } else {
            value = READ(address);
            TST_FLAG(value & 1, C_FLAG);
            value = (value >> 1) | (c << 7);
            WRITE(address, value);
            SET_ZN_FLAG(value);
        }
        a = A;
        b = READ(address);
        c = C;
        A = a + b + c;
        SET_ZN_FLAG(A);
        TST_FLAG(((uint16_t)a + (uint16_t)b + (uint16_t)c) > 0xFF, C_FLAG);
        TST_FLAG(((a ^ b) & 0x80) == 0 && ((a ^ A) & 0x80) != 0, V_FLAG);
        break;
    case RTI: // Return from Interrupt
        PS = (POP() & 0xEF) | 0x20;
        PC = POPW();
        break;
    case RTS: // Return from Subroutine
        PC = POPW() + 1;
        break;
    case SAX: WRITE(address, A & X); break;
    case SBC: // Subtract with Carry
        a = A;
        b = READ(address);
        c = C;
        A = a - b - (1 - c);
        SET_ZN_FLAG(A);
        TST_FLAG((a - b - (1 - c)) >= 0, C_FLAG);
        TST_FLAG(((a ^ b) & 0x80) != 0 && ((a ^ A) & 0x80) != 0, V_FLAG);
        break;
    case SEC: // Set Carry Flag
        SET_FLAG(C_FLAG);
        break;
    case SED: // Set Decimal Flag
        SET_FLAG(D_FLAG);
        break;
    case SEI: // Set Interrupt Disable
        SET_FLAG(I_FLAG);
        break;
    case SHX: ASSERT(false); break;
    case SHY: ASSERT(false); break;
    case SLO: // ASL ORA
        if (mode == ACCUMULATOR) {
            TST_FLAG((A >> 7) & 1, C_FLAG);
            A <<= 1;
            SET_ZN_FLAG(A);
        } else {
            value = READ(address);
            TST_FLAG((value >> 7) & 1, C_FLAG);
            value <<= 1;
            WRITE(address, value);
            SET_ZN_FLAG(value);
        }
        A |= READ(address);
        SET_ZN_FLAG(A);
        break;
    case SRE: // LSR EOR
        if (mode == ACCUMULATOR) {
            TST_FLAG(A & 1, C_FLAG);
            A >>= 1;
            SET_ZN_FLAG(A);
        } else {
            value = READ(address);
            TST_FLAG(value & 1, C_FLAG);
            value >>= 1;
            WRITE(address, value);
            SET_ZN_FLAG(value);
        }
        A ^= READ(address);
        SET_ZN_FLAG(A);
        break;
    case STA: // Store Accumulator
        WRITE(address, A);
        break;
    case STX: // Store X Register
        WRITE(address, X);
        break;
    case STY: // Store Y Register
        WRITE(address, Y);
        break;
    case TAS: ASSERT(false); break;
    case TAX: // Transfer Accumulator to X
        X = A;
        SET_ZN_FLAG(X);
        break;
    case TAY: // Transfer Accumulator to Y
        Y = A;
        SET_ZN_FLAG(Y);
        break;
    case TSX: // Transfer Stack Pointer to X
        X = SP;
        SET_ZN_FLAG(X);
        break;
    case TXA: // Transfer Index X to Accumulator
        A = X;
        SET_ZN_FLAG(A);
        break;
    case TXS: // Transfer Index X to Stack Pointer
        SP = X;
        break;
    case TYA: // Transfer Index Y to Accumulator
        A = Y;
        SET_ZN_FLAG(A);
        break;
    case XAA: ASSERT(false); break;
    }
    return CYCLES - cycles;
}