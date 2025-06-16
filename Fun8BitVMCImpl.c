#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>

#define RAM_SIZE 256
#define ROM_SIZE 256
#define STACK_SIZE 16
#define DEBUG 1
#define LIMIT 65536

unsigned long safe = 0;
uint8_t RAM[RAM_SIZE];           // Data RAM
uint8_t ROM[ROM_SIZE];           // Program ROM
uint8_t RAM_POINTER[STACK_SIZE]; // RAM pointer stack to reach full 256
uint8_t ROM_POINTER[STACK_SIZE]; // ROM pointer stack to reach full 256

uint8_t A = 0x00;          // Accumulator
uint8_t PC = 0x00;         // Program Counter
uint8_t PSP = 0x00;        // Stack Pointer when writing program (ROM, grows up)

void write_rom(uint8_t opcode, uint8_t operand) {
    ROM[PSP++] = (opcode << 4) | operand & 0x0F;
}

void p(uint8_t opcode, uint8_t operand){
    ROM[PSP++] = (opcode << 4) | operand & 0x0F;
}

void dump_ram() {
    printf("\n-------------------- RAM  DUMP --------------------\n");
    for (int i = 0; i < RAM_SIZE; i++) {
        if (i % 16 == 0) printf("%02X: ", i);
        printf("%02X ", RAM[i]);
        if (i % 16 == 15) printf("\n");
    }
}

void dump_rom() {
    printf("\n-------------------- ROM  DUMP --------------------\n");
    for (int i = 0; i < ROM_SIZE; i++) {
        if (i % 16 == 0) printf("%02X: ", i);
        printf("%02X ", ROM[i]);
        if (i % 16 == 15) printf("\n");
    }
}

void dump_rom_s() {
    printf("\n------------------ PC_S DUMP ------------------\n");
    for (int i = 0; i < STACK_SIZE; i++) {
        printf("%02X ", ROM_POINTER[i]);
    } printf("\n");
}

void dump_ram_s() {
    printf("\n----------------- RAM_S  DUMP -----------------\n");
    for (int i = 0; i < STACK_SIZE; i++) {
        printf("%02X ", RAM_POINTER[i]);
    } printf("\n");
}

// Instruction Set
enum {
    SYS  = 0x0,    
    LDA  = 0x1,    // A = RAM[*x]
    STA  = 0x2,    // RAM[*x] = A
    ADD  = 0x3,    // A += RAM[*x]
    SUB  = 0x4,    // A -= RAM[*x]
    JMP  = 0x5,    // Jump to ROM[*x]
    JZ   = 0x6,    // Jump to ROM[*x] if A == 0
    SPT  = 0x7,    // RAM *x = A
    GPT  = 0x8,    // A = RAM *x
    PNT  = 0x9,    // ROM *x = A
    RET  = 0xA,    // A = ROM *x
    AND  = 0xB,    // A &= RAM[*x]
    OR   = 0xC,    // A |= RAM[*x]
    XOR  = 0xD,    // A ^= RAM[*x]
    INC  = 0xE,    // A += x
    DEC  = 0xF,    // A -= x
};

void execute() {
    while (safe < LIMIT) {
        uint8_t instr = ROM[PC];
        uint8_t opcode = (instr & 0xF0) >> 4;
        uint8_t operand = instr & 0x0F;
        if (DEBUG){
            ++safe;
        }
        ++PC;
        switch (opcode) {
            case SYS:
                switch (operand){
                    case 0x0:
                        printf("VM exits with no code\n");
                        return;
                    case 0x1:
                        printf("VM exits with code %02X\n", A);
                        return;
                    case 0x2:
                        break;
                    case 0x3:
                        A = 0;
                        break;
                    case 0x4:
                        ++A;
                        break;
                    case 0x5:
                        --A;
                        break;
                    case 0x6:
                        A += 4;
                        break;
                    case 0x7:
                        A -= 4;
                        break;
                    case 0x8:
                        A += 16;
                        break;
                    case 0x9:
                        A -= 16;
                        break;
                    case 0xA:
                        printf("%02X ", A);
                        break;
                    case 0xB:
                        printf("%02X ", PC - 1);
                        break;
                    case 0xC:
                        PC += 15;
                        break;
                    case 0xD:
                        PC -= 17;
                        break;
                    case 0xE:
                        PC = 0;
                        break;
                    case 0xF:
                        if (A == 0) PC = 0;
                        break;
                    default:
                        break;
                }
                break;
            case LDA:
                A = RAM[RAM_POINTER[operand]];
                break;
            case STA:
                RAM[RAM_POINTER[operand]] = A;
                break;
            case ADD:
                A += RAM[RAM_POINTER[operand]];
                break;
            case SUB:
                A -= RAM[RAM_POINTER[operand]];
                break;
            case JMP:
                PC = ROM_POINTER[operand];
                break;
            case JZ:
                if (A == 0) PC = ROM_POINTER[operand];
                break;
            case SPT:
                RAM_POINTER[operand] = A;
                break;
            case GPT:
                A = RAM_POINTER[operand];
                break;
            case PNT:
                ROM_POINTER[operand] = A;
                break;
            case RET:
                A = ROM_POINTER[operand];
                break;
            case AND:
                A &= RAM[RAM_POINTER[operand]];
                break;
            case OR:
                A |= RAM[RAM_POINTER[operand]];
                break;
            case XOR:
                A ^= RAM[RAM_POINTER[operand]];
                break;
            case INC:
                if (operand == 0){
                    A = (A + 16) & 0xFF;
                } else A = (A + operand) & 0xFF;
                break;
            case DEC:
                if (operand == 0){
                    A = (A - 16) & 0xFF;
                } else A = (A - operand) & 0xFF;
                break;
            default:
                return;
        }
        printf("%02X: %02X - %02X\n", PC, instr, A);
    } if (safe >= LIMIT) {
        printf("VM DEBUG time out after %u\n", LIMIT);
    }
}

// Map mnemonics to opcodes
struct {
    const char *mnemonic;
    uint8_t opcode;
} instruction_map[] = {
    {"SYS", 0x0}, {"LDA", 0x1}, {"STA", 0x2}, {"ADD", 0x3},
    {"SUB", 0x4}, {"JMP", 0x5}, {"JZ", 0x6}, {"SPT", 0x7},
    {"GPT", 0x8}, {"PNT", 0x9}, {"RET", 0xA}, {"AND", 0xB},
    {"OR", 0xC}, {"XOR", 0xD}, {"INC", 0xE}, {"DEC", 0xF},
    {NULL, 0} // Sentinel to mark end of map
};

// Function to trim whitespace from a string
void trim_whitespace(char *str) {
    char *end;
    while (isspace((unsigned char)*str)) str++;
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) *end-- = '\0';
}

// Function to parse a single line of assembly
int parse_line(char *line, uint8_t *opcode, uint8_t *operand, int line_num) {
    char mnemonic[10] = {0};
    char operand_str[10] = {0};
    int operand_value;

    // Remove comments and trim
    char *comment = strchr(line, ';');
    if (comment) *comment = '\0';
    trim_whitespace(line);

    // Skip empty lines
    if (strlen(line) == 0) return 0;

    // Parse mnemonic and operand
    if (sscanf(line, "%s %s", mnemonic, operand_str) < 1) {
        fprintf(stderr, "Error: Invalid syntax on line %d: '%s'\n", line_num, line);
        return -1;
    }

    // Convert mnemonic to uppercase for case-insensitive comparison
    for (int i = 0; mnemonic[i]; i++) {
        mnemonic[i] = toupper(mnemonic[i]);
    }

    // Find opcode
    int i;
    for (i = 0; instruction_map[i].mnemonic; i++) {
        if (strcmp(mnemonic, instruction_map[i].mnemonic) == 0) {
            *opcode = instruction_map[i].opcode;
            break;
        }
    }
    if (!instruction_map[i].mnemonic) {
        fprintf(stderr, "Error: Unknown mnemonic '%s' on line %d\n", mnemonic, line_num);
        return -1;
    }

    // Parse operand (expecting hex or decimal)
    if (operand_str[0] != '\0') {
        if (strncmp(operand_str, "0x", 2) == 0) {
            if (sscanf(operand_str + 2, "%x", &operand_value) != 1 || operand_value > 0x0F) {
                fprintf(stderr, "Error: Invalid operand '%s' on line %d (must be 0x0 to 0xF)\n", operand_str, line_num);
                return -1;
            }
        } else {
            if (sscanf(operand_str, "%d", &operand_value) != 1 || operand_value > 15) {
                fprintf(stderr, "Error: Invalid operand '%s' on line %d (must be 0 to 15)\n", operand_str, line_num);
                return -1;
            }
        }
        *operand = (uint8_t)operand_value;
    } else {
        fprintf(stderr, "Error: Missing operand on line %d\n", line_num);
        return -1;
    }

    return 1;
}

int main(int argc, char *argv[]) {
    // Check for correct number of arguments
    if (argc != 2) {
        fprintf(stderr, "Err: No assembly file to execute\n");
        return 1;
    }

    // Open the assembly file
    FILE *file = fopen(argv[1], "r");
    if (!file) {
        fprintf(stderr, "Error: Could not open file '%s'\n", argv[1]);
        return 1;
    }

    // Initialize VM state
    PSP = 0; // Reset program stack pointer
    memset(ROM, 0, ROM_SIZE); // Clear ROM

    // Read and parse the file
    char line[256];
    int line_num = 0;
    int success = 1;

    while (fgets(line, sizeof(line), file)) {
        line_num++;
        // Remove newline
        line[strcspn(line, "\n")] = '\0';

        uint8_t opcode, operand;
        int result = parse_line(line, &opcode, &operand, line_num);
        if (result == -1) {
            success = 0;
            break;
        } else if (result == 0) {
            continue; // Skip empty lines
        }

        // Write instruction to ROM
        write_rom(opcode, operand);
    }

    fclose(file);

    // Execute if parsing was successful
    if (success) {
        execute();
        dump_rom_s();
        dump_rom();
        dump_ram_s();
        dump_ram();
    } else {
        fprintf(stderr, "Error: Failed to load program due to syntax errors\n");
        return 1;
    }

    return 0;
}
