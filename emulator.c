#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define BYTE 8

typedef struct ConditionCodes
{
  uint8_t    z:1;
  uint8_t    s:1;
  uint8_t    p:1;
  uint8_t    cy:1;
  uint8_t    ac:1;
  uint8_t    pad:3;
}ConditionCodes;

typedef struct State8080
{
  uint8_t    a;
  uint8_t    b;
  uint8_t    c;
  uint8_t    d;
  uint8_t    e;
  uint8_t    h;
  uint8_t    l;
  uint16_t    sp;
  uint16_t    pc;
  uint8_t     *memory;
  struct      ConditionCodes      cc;
  uint8_t     int_enable;
}State8080;

void error()
{
  perror("Error: ");
  exit(1);
}

uint32_t getFileSize(int fd)
{
  struct stat *fileInfo = (struct stat *) malloc(sizeof(struct stat));
  if(fileInfo == NULL)
  {
    error();
  }

  if(fstat(fd, fileInfo) == -1)
  {
    error();
  }

  off_t fileSize = fileInfo->st_size;
  free(fileInfo);

  return (uint32_t) fileSize;
}

int openFile(char *file)
{
  int fd;
  if((fd = open(file,  O_RDONLY)) == -1)
  {
    error();
  }
  return fd;
}

uint8_t* loadBuffer(int fd, uint32_t fileSize)
{
  //memory map file
  uint8_t *mapedFile = mmap(NULL, fileSize, PROT_READ, MAP_PRIVATE, fd, 0);
  if(mapedFile == MAP_FAILED)
  {
    error();
  }
  close(fd);
  return mapedFile;
}

uint8_t disasemble8080(uint8_t *buffer, uint32_t pc)
{
  uint8_t *code = &buffer[pc];
  int opbytes = 1;
  //
  printf ("%04x ", pc);
  switch (*code)
  {
    case 0x00: printf("NOP\n"); break;
    case 0x01: printf("LXI    B,#$%02x%02x\n", code[2], code[1]); opbytes=3; break;
    case 0x02: printf("STAX   B\n"); break;
    case 0x03: printf("INX    B\n"); break;
    case 0x04: printf("INR    B\n"); break;
    case 0x05: printf("DCR    B\n"); break;
    case 0x06: printf("MVI    B,#$%02x\n", code[1]); opbytes=2; break;
    case 0x07: printf("RLC\n"); break;
    case 0x08: printf("NOP\n"); break;
    case 0x09: printf("DAD    B\n"); break;
    case 0x0A: printf("LDAX   B\n"); break;
    case 0x0B: printf("DCX    B\n"); break;
    case 0x0C: printf("INR    C\n"); break;
    case 0x0D: printf("DRC    C\n"); break;
    case 0x0E: printf("MVI    C,#$%02x\n", code[1]); opbytes=2; break;
    case 0x0F: printf("RRC\n"); break;
    case 0x10: printf("NOP\n"); break;
    case 0x11: printf("LXI    D,#$%02x%02x\n", code[2], code[1]); opbytes=3; break;
    case 0x12: printf("STAX   D\n"); break;
    case 0x13: printf("INX    D\n"); break;
    case 0x14: printf("INR    D\n"); break;
    case 0x15: printf("DCR    D\n"); break;
    case 0x16: printf("MVI    D,#$%02x\n", code[1]); opbytes=2; break;
    case 0x17: printf("RAL\n"); break;
    case 0x18: printf("NOP\n"); break;
    case 0x19: printf("DAD    D\n"); break;
    case 0x1A: printf("LDAX   D\n"); break;
    case 0x1B: printf("DCX    D\n"); break;
    case 0x1C: printf("INR    E\n"); break;
    case 0x1D: printf("DCR    E\n"); break;
    case 0x1E: printf("MVI    E,#$%02x\n", code[1]); opbytes=2; break;
    case 0x1F: printf("RAR\n"); break;
    case 0x20: printf("NOP\n"); break;
    case 0x21: printf("LXI    H,#$%02x%02x\n", code[2], code[1]); opbytes=3; break;
    case 0x22: printf("SHLD   #$%02x%02x\n", code[2], code[1]); opbytes=3; break;
    case 0x23: printf("INX    H\n"); break;
    case 0x24: printf("INR    H\n"); break;
    case 0x25: printf("DCR    H\n"); break;
    case 0x26: printf("MVI    H,#$%02x\n", code[1]); opbytes=2; break;
    case 0x27: printf("DAA\n"); break;
    case 0x28: printf("NOP\n"); break;
    case 0x29: printf("DAD    H\n"); break;
    case 0x2A: printf("LHLD   #$%02x%02x\n", code[2], code[1]); opbytes=3; break;
    case 0x2B: printf("DCX    H\n"); break;
    case 0x2C: printf("INR    L\n"); break;
    case 0x2D: printf("DCR    L\n"); break;
    case 0x2E: printf("MVI    L,#$%02x\n", code[1]); opbytes=2; break;
    case 0x2F: printf("CMA\n"); break;
    case 0x30: printf("NOP\n"); break;
    case 0x31: printf("LXI,   SP,#$%02x%02x\n", code[2], code[1]); opbytes=3; break;
    case 0x32: printf("STA    #$%02x%02x\n", code[2], code[1]); opbytes=3; break;
    case 0x33: printf("INX    SP\n"); break;
    case 0x34: printf("INR    M\n"); break;
    case 0x35: printf("DCR    M\n"); break;
    case 0x36: printf("MVI    M,#$%02x\n", code[1]); opbytes=2; break;
    case 0x37: printf("STC\n"); break;
    case 0x38: printf("NOP\n"); break;
    case 0x39: printf("DAD    SP\n"); break;
    case 0x3A: printf("LDA    #$%02x%02x\n", code[2], code[1]); opbytes=3; break;
    case 0x3B: printf("DCX    SP\n"); break;
    case 0x3C: printf("INR    A\n"); break;
    case 0x3D: printf("DCR    A\n"); break;
    case 0x3E: printf("MVI    A,#$%02x\n", code[1]); opbytes=2; break;
    case 0x3F: printf("CMC\n"); break;
    case 0x40: printf("MOV    B,B\n"); break;
    case 0x41: printf("MOV    B,C\n"); break;
    case 0x42: printf("MOV    B,D\n"); break;
    case 0x43: printf("MOV    B,E\n"); break;
    case 0x44: printf("MOV    B,H\n"); break;
    case 0x45: printf("MOV    B,L\n"); break;
    case 0x46: printf("MOV    B,M\n"); break;
    case 0x47: printf("MOV    B,A\n"); break;
    case 0x48: printf("MOV    C,B\n"); break;
    case 0x49: printf("MOV    C,C\n"); break;
    case 0x4A: printf("MOV    C,D\n"); break;
    case 0x4B: printf("MOV    C,E\n"); break;
    case 0x4C: printf("MOV    C,H\n"); break;
    case 0x4D: printf("MOV    C,L\n"); break;
    case 0x4E: printf("MOV    C,M\n"); break;
    case 0x4F: printf("MOV    C,A\n"); break;
    case 0x50: printf("MOV    D,B\n"); break;
    case 0x51: printf("MOV    D,C\n"); break;
    case 0x52: printf("MOV    D,D\n"); break;
    case 0x53: printf("MOV    D,E\n"); break;
    case 0x54: printf("MOV    D,H\n"); break;
    case 0x55: printf("MOV    D,L\n"); break;
    case 0x56: printf("MOV    D,M\n"); break;
    case 0x57: printf("MOV    D,A\n"); break;
    case 0x58: printf("MOV    E,B\n"); break;
    case 0x59: printf("MOV    E,C\n"); break;
    case 0x5A: printf("MOV    E,D\n"); break;
    case 0x5B: printf("MOV    E,E\n"); break;
    case 0x5C: printf("MOV    E,H\n"); break;
    case 0x5D: printf("MOV    E,L\n"); break;
    case 0x5E: printf("MOV    E,M\n"); break;
    case 0x5F: printf("MOV    E,A\n"); break;
    case 0x60: printf("MOV    H,B\n"); break;
    case 0x61: printf("MOV    H,C\n"); break;
    case 0x62: printf("MOV    H,D\n"); break;
    case 0x63: printf("MOV    H,E\n"); break;
    case 0x64: printf("MOV    H,H\n"); break;
    case 0x65: printf("MOV    H,L\n"); break;
    case 0x66: printf("MOV    H,M\n"); break;
    case 0x67: printf("MOV    H,A\n"); break;
    case 0x68: printf("MOV    L,B\n"); break;
    case 0x69: printf("MOV    L,C\n"); break;
    case 0x6A: printf("MOV    L,D\n"); break;
    case 0x6B: printf("MOV    L,E\n"); break;
    case 0x6C: printf("MOV    L,H\n"); break;
    case 0x6D: printf("MOV    L,L\n"); break;
    case 0x6E: printf("MOV    L,M\n"); break;
    case 0x6F: printf("MOV    L,A\n"); break;
    case 0x70: printf("MOV    M,B\n"); break;
    case 0x71: printf("MOV    M,C\n"); break;
    case 0x72: printf("MOV    M,D\n"); break;
    case 0x73: printf("MOV    M,E\n"); break;
    case 0x74: printf("MOV    M,H\n"); break;
    case 0x75: printf("MOV    M,L\n"); break;
    case 0x76: printf("HLT\n"); break;
    case 0x77: printf("MOV    M,A\n"); break;
    case 0x78: printf("MOV    A,B\n"); break;
    case 0x79: printf("MOV    A,C\n"); break;
    case 0x7A: printf("MOV    A,D\n"); break;
    case 0x7B: printf("MOV    A,E\n"); break;
    case 0x7C: printf("MOV    A,H\n"); break;
    case 0x7D: printf("MOV    A,L\n"); break;
    case 0x7E: printf("MOV    A,M\n"); break;
    case 0x7F: printf("MOV    A,A\n"); break;
    case 0x80: printf("ADD    B\n"); break;
    case 0x81: printf("ADD    C\n"); break;
    case 0x82: printf("ADD    D\n"); break;
    case 0x83: printf("ADD    E\n"); break;
    case 0x84: printf("ADD    H\n"); break;
    case 0x85: printf("ADD    L\n"); break;
    case 0x86: printf("ADD    M\n"); break;
    case 0x87: printf("ADD    A\n"); break;
    case 0x88: printf("ADC    B\n"); break;
    case 0x89: printf("ADC    C\n"); break;
    case 0x8A: printf("ADC    D\n"); break;
    case 0x8B: printf("ADC    E\n"); break;
    case 0x8C: printf("ADC    H\n"); break;
    case 0x8D: printf("ADC    L\n"); break;
    case 0x8E: printf("ADC    M\n"); break;
    case 0x8F: printf("ADC    A\n"); break;
    case 0x90: printf("SUB    B\n"); break;
    case 0x91: printf("SUB    C\n"); break;
    case 0x92: printf("SUB    D\n"); break;
    case 0x93: printf("SUB    E\n"); break;
    case 0x94: printf("SUB    H\n"); break;
    case 0x95: printf("SUB    L\n"); break;
    case 0x96: printf("SUB    M\n"); break;
    case 0x97: printf("SUB    A\n"); break;
    case 0x98: printf("SBB    B\n"); break;
    case 0x99: printf("SBB    C\n"); break;
    case 0x9A: printf("SBB    D\n"); break;
    case 0x9B: printf("SBB    E\n"); break;
    case 0x9C: printf("SBB    H\n"); break;
    case 0x9D: printf("SBB    L\n"); break;
    case 0x9E: printf("SBB    M\n"); break;
    case 0x9F: printf("SBB    A\n"); break;
    case 0xA0: printf("ANA    B\n"); break;
    case 0xA1: printf("ANA    C\n"); break;
    case 0xA2: printf("ANA    D\n"); break;
    case 0xA3: printf("ANA    E\n"); break;
    case 0xA4: printf("ANA    H\n"); break;
    case 0xA5: printf("ANA    L\n"); break;
    case 0xA6: printf("ANA    M\n"); break;
    case 0xA7: printf("ANA    A\n"); break;
    case 0xA8: printf("XRA    B\n"); break;
    case 0xA9: printf("XRA    C\n"); break;
    case 0xAA: printf("XRA    D\n"); break;
    case 0xAB: printf("XRA    E\n"); break;
    case 0xAC: printf("XRA    H\n"); break;
    case 0xAD: printf("XRA    L\n"); break;
    case 0xAE: printf("XRA    M\n"); break;
    case 0xAF: printf("XRA    A\n"); break;
    case 0xB0: printf("ORA    B\n"); break;
    case 0xB1: printf("ORA    C\n"); break;
    case 0xB2: printf("ORA    D\n"); break;
    case 0xB3: printf("ORA    E\n"); break;
    case 0xB4: printf("ORA    H\n"); break;
    case 0xB5: printf("ORA    L\n"); break;
    case 0xB6: printf("ORA    M\n"); break;
    case 0xB7: printf("ORA    A\n"); break;
    case 0xB8: printf("CMP    B\n"); break;
    case 0xB9: printf("CMP    C\n"); break;
    case 0xBA: printf("CMP    D\n"); break;
    case 0xBB: printf("CMP    E\n"); break;
    case 0xBC: printf("CMP    H\n"); break;
    case 0xBD: printf("CMP    L\n"); break;
    case 0xBE: printf("CMP    M\n"); break;
    case 0xBF: printf("CMP    A\n"); break;
    case 0xC0: printf("RNZ\n"); break;
    case 0xC1: printf("POP    B\n"); break;
    case 0xC2: printf("JNZ    #$%02x%02x\n", code[2], code[1]); opbytes=3; break;
    case 0xC3: printf("JMP    #$%02x%02x\n", code[2], code[1]); opbytes=3; break;
    case 0xC4: printf("CNZ    #$%02x%02x\n", code[2], code[1]); opbytes=3; break;
    case 0xC5: printf("PUSH   B\n"); break;
    case 0xC6: printf("ADI    #$%02x\n", code[1]); opbytes=2; break;
    case 0xC7: printf("RST    0\n"); break;
    case 0xC8: printf("RZ\n"); break;
    case 0xC9: printf("RET\n"); break;
    case 0xCA: printf("JZ     #$%02x%02x\n", code[2], code[1]); opbytes=3; break;
    case 0xCB: printf("JMP    #$%02x%02x\n", code[2], code[1]); opbytes=3; break;
    case 0xCC: printf("CZ     #$%02x%02x\n", code[2], code[1]); opbytes=3; break;
    case 0xCD: printf("CALL   #$%02x%02x\n", code[2], code[1]); opbytes=3; break;
    case 0xCE: printf("ACI    #$%02x\n", code[1]); opbytes=2; break;
    case 0xCF: printf("RST    1\n"); break;
    case 0xD0: printf("RNC\n"); break;
    case 0xD1: printf("POP    D\n"); break;
    case 0xD2: printf("JNC    #$%02x%02x\n", code[2], code[1]); opbytes=3; break;
    case 0xD3: printf("OUT    #$%02x\n", code[1]); opbytes=2; break;
    case 0xD4: printf("CNC    #$%02x%02x\n", code[2], code[1]); opbytes=3; break;
    case 0xD5: printf("PUSH   D\n"); break;
    case 0xD6: printf("SUI    #$%02x\n", code[1]); opbytes=2; break;
    case 0xD7: printf("RST    2\n"); break;
    case 0xD8: printf("RC\n"); break;
    case 0xD9: printf("RET\n"); break;
    case 0xDA: printf("JC     #$%02x%02x\n", code[2], code[1]); opbytes=3; break;
    case 0xDB: printf("IN     #$%02x\n", code[1]); opbytes=2; break;
    case 0xDC: printf("CC     #$%02x%02x\n", code[2], code[1]); opbytes=3; break;
    case 0xDD: printf("CALL   #$%02x%02x\n", code[2], code[1]); opbytes=3; break;
    case 0xDE: printf("SBI    #$%02x\n", code[1]); opbytes=2; break;
    case 0xDF: printf("RST    3\n"); break;
    case 0xE0: printf("RPO\n"); break;
    case 0xE1: printf("POP    H\n"); break;
    case 0xE2: printf("JPO    #$%02x%02x\n", code[2], code[1]); opbytes=3; break;
    case 0xE3: printf("XTHL\n"); break;
    case 0xE4: printf("CPO    #$%02x%02x\n", code[2], code[1]); opbytes=3; break;
    case 0xE5: printf("PUSH   H\n"); break;
    case 0xE6: printf("ANI    #$%02x\n", code[1]); opbytes=2; break;
    case 0xE7: printf("RST    4\n"); break;
    case 0xE8: printf("RPE\n"); break;
    case 0xE9: printf("PCHL\n"); break;
    case 0xEA: printf("JPE     #$%02x%02x\n", code[2], code[1]); opbytes=3; break;
    case 0xEB: printf("XCHG\n"); break;
    case 0xEC: printf("CPE     #$%02x%02x\n", code[2], code[1]); opbytes=3; break;
    case 0xED: printf("CALL   #$%02x%02x\n", code[2], code[1]); opbytes=3; break;
    case 0xEE: printf("XRI    #$%02x\n", code[1]); opbytes=2; break;
    case 0xEF: printf("RST    5\n"); break;
    case 0xF0: printf("RP\n"); break;
    case 0xF1: printf("POP    PSW\n"); break;
    case 0xF2: printf("JP     #$%02x%02x\n", code[2], code[1]); opbytes=3; break;
    case 0xF3: printf("DI\n"); break;
    case 0xF4: printf("CP     #$%02x%02x\n", code[2], code[1]); opbytes=3; break;
    case 0xF5: printf("PUSH   PSW\n"); break;
    case 0xF6: printf("ORI    #$%02x\n", code[1]); opbytes=2; break;
    case 0xF7: printf("RST    6\n"); break;
    case 0xF8: printf("RM\n"); break;
    case 0xF9: printf("SPHL\n"); break;
    case 0xFA: printf("JM     #$%02x%02x\n", code[2], code[1]); opbytes=3; break;
    case 0xFB: printf("EI\n"); break;
    case 0xFC: printf("CM     #$%02x%02x\n", code[2], code[1]); opbytes=3; break;
    case 0xFD: printf("CALL   #$%02x%02x\n", code[2], code[1]); opbytes=3; break;
    case 0xFE: printf("CPI    #$%02x\n", code[1]); opbytes=2; break;
    case 0xFF: printf("RST    7\n"); break;
    default: printf("Code not found\n");
  }

  return opbytes;
}

void disasemble(char *file)
{
  int fd = openFile(file);
  uint32_t fileSize = getFileSize(fd);
  uint8_t *buffer = loadBuffer(fd, fileSize);
  uint32_t pc = 0;
  while(pc < fileSize)
  {
    pc += disasemble8080(buffer, pc);
  }
}

uint16_t parity(uint8_t value)
{
  int bitSet = 0;
  for(uint8_t bitIndex = 0; bitIndex < BYTE; bitIndex++)
  {
    if ((0x1 << bitIndex) & value)
    {
      bitSet++;
    }
  }

  // returns 1 if the number of bits set is even
  return (bitSet % 2) == 0;
}

void setAddFlags(State8080 *state, uint16_t answer)
{
  state->cc.z = ((answer & 0xFF) == 0);
  state->cc.s = ((answer & 0x80) != 0);
  state->cc.cy = (answer > 0xFF);
  state->cc.p = parity(answer & 0xFF);
  return;
}

void setSubFlags(State8080 *state, uint16_t answer)
{
  state->cc.z = ((answer & 0xFF) == 0);
  state->cc.s = ((answer & 0x80) != 0);
  state->cc.cy = !(answer > 0xFF);
  state->cc.p = parity(answer & 0xFF);
  return;
}

void setLogicFlags(State8080 *state, uint8_t answer)
{
  state->cc.z = (answer == 0);
  state->cc.s = ((answer & 0x80) != 0);
  state->cc.cy = 0;
  state->cc.p = parity(answer);
  return;
}

// update stack info when calling a subroutine
void updateStack(State8080 *state)
{
  uint16_t ret = state->pc + 3;   //return
  state->memory[state->sp - 1] = (ret >> 8) & 0xFF;
  state->memory[state->sp - 2] = (ret & 0xFF);
  state->sp = state->sp - 2;
}

int emulate8080(State8080 *state)
{
  uint8_t *opcode = &(state->memory[state->pc]);

  switch(*opcode)
  {
    case 0x00: break;   //NOP
    case 0x01:          //LXI   B,word
      state->c = opcode[1];
      state->b = opcode[2];
      state->pc += 2;
      break;
    // case 0x02: printf("STAX   B\n"); break;
    // case 0x03: printf("INX    B\n"); break;
    // case 0x04: printf("INR    B\n"); break;
    // case 0x05: printf("DCR    B\n"); break;
    // case 0x06: printf("MVI    B,#$%02x\n", code[1]); opbytes=2; break;
    // case 0x07: printf("RLC\n"); break;
    case 0x08: break;   //NOP
    // case 0x09: printf("DAD    B\n"); break;
    // case 0x0A: printf("LDAX   B\n"); break;
    // case 0x0B: printf("DCX    B\n"); break;
    // case 0x0C: printf("INR    C\n"); break;
    // case 0x0D: printf("DRC    C\n"); break;
    // case 0x0E: printf("MVI    C,#$%02x\n", code[1]); opbytes=2; break;
    // case 0x0F: printf("RRC\n"); break;
    case 0x10: break;   //NOP
    // case 0x11: printf("LXI    D,#$%02x%02x\n", code[2], code[1]); opbytes=3; break;
    // case 0x12: printf("STAX   D\n"); break;
    // case 0x13: printf("INX    D\n"); break;
    // case 0x14: printf("INR    D\n"); break;
    // case 0x15: printf("DCR    D\n"); break;
    // case 0x16: printf("MVI    D,#$%02x\n", code[1]); opbytes=2; break;
    // case 0x17: printf("RAL\n"); break;
    case 0x18: break;    //NOP
    // case 0x19: printf("DAD    D\n"); break;
    // case 0x1A: printf("LDAX   D\n"); break;
    // case 0x1B: printf("DCX    D\n"); break;
    // case 0x1C: printf("INR    E\n"); break;
    // case 0x1D: printf("DCR    E\n"); break;
    // case 0x1E: printf("MVI    E,#$%02x\n", code[1]); opbytes=2; break;
    // case 0x1F: printf("RAR\n"); break;
    case 0x20: break;   //NOP
    // case 0x21: printf("LXI    H,#$%02x%02x\n", code[2], code[1]); opbytes=3; break;
    // case 0x22: printf("SHLD   #$%02x%02x\n", code[2], code[1]); opbytes=3; break;
    // case 0x23: printf("INX    H\n"); break;
    // case 0x24: printf("INR    H\n"); break;
    // case 0x25: printf("DCR    H\n"); break;
    // case 0x26: printf("MVI    H,#$%02x\n", code[1]); opbytes=2; break;
    // case 0x27: printf("DAA\n"); break;
    case 0x28: break;   //NOP
    // case 0x29: printf("DAD    H\n"); break;
    // case 0x2A: printf("LHLD   #$%02x%02x\n", code[2], code[1]); opbytes=3; break;
    // case 0x2B: printf("DCX    H\n"); break;
    // case 0x2C: printf("INR    L\n"); break;
    // case 0x2D: printf("DCR    L\n"); break;
    // case 0x2E: printf("MVI    L,#$%02x\n", code[1]); opbytes=2; break;
    // case 0x2F: printf("CMA\n"); break;
    case 0x30: break;   //NOP
    // case 0x31: printf("LXI,   SP,#$%02x%02x\n", code[2], code[1]); opbytes=3; break;
    // case 0x32: printf("STA    #$%02x%02x\n", code[2], code[1]); opbytes=3; break;
    // case 0x33: printf("INX    SP\n"); break;
    // case 0x34: printf("INR    M\n"); break;
    // case 0x35: printf("DCR    M\n"); break;
    // case 0x36: printf("MVI    M,#$%02x\n", code[1]); opbytes=2; break;
    // case 0x37: printf("STC\n"); break;
    case 0x38: break;   //NOP
    // case 0x39: printf("DAD    SP\n"); break;
    // case 0x3A: printf("LDA    #$%02x%02x\n", code[2], code[1]); opbytes=3; break;
    // case 0x3B: printf("DCX    SP\n"); break;
    // case 0x3C: printf("INR    A\n"); break;
    // case 0x3D: printf("DCR    A\n"); break;
    // case 0x3E: printf("MVI    A,#$%02x\n", code[1]); opbytes=2; break;
    // case 0x3F: printf("CMC\n"); break;
    case 0x40: break;                         //MOV B,B
    case 0x41: state->b = state->c; break;    //MOV B,C
    case 0x42: state->b = state->d; break;    //MOV B,D
    case 0x43: state->b = state->e; break;    //MOV B,E
    case 0x44: state->b = state->h; break;    //MOV B,H
    case 0x45: state->b = state->l; break;    //MOV B,L
    case 0x46:                                //MOV B,M
    {
      uint16_t offset = (state->h << 8) | (state->l);
      state->b = state->memory[offset];
    }
    case 0x47: state->b = state->a; break;    //MOV B,A
    case 0x48: state->c = state->b; break;    //MOV C,B
    case 0x49: break;                         //MOV C,C
    case 0x4A: state->c = state->d; break;    //MOV C,D
    case 0x4B: state->c = state->e; break;    //MOV C,E
    case 0x4C: state->c = state->h; break;    //MOV C,H
    case 0x4D: state->c = state->l; break;    //MOV C,L
    case 0x4E: //MOV C,M
    {
      uint16_t offset = (state->h << 8) | (state->l);
      state->c = state->memory[offset];
    }
    case 0x4F: state->c = state->a; break;    //MOV C,A
    case 0x50: state->d = state->b; break;    //MOV D,B
    case 0x51: state->d = state->c; break;    //MOV D,C
    case 0x52: break;                         //MOV D,D
    case 0x53: state->d = state->e; break;    //MOV D,E
    case 0x54: state->d = state->h; break;    //MOV D,H
    case 0x55: state->d = state->l; break;    //MOV D,L
    case 0x56: //MOV D,M
    {
      uint16_t offset = (state->h << 8) | (state->l);
      state->d = state->memory[offset];
    }
    case 0x57: state->d = state->a; break;    //MOV D,A
    case 0x58: state->e = state->b; break;    //MOV E,B
    case 0x59: state->e = state->c; break;    //MOV E,C
    case 0x5A: state->e = state->d; break;    //MOV E,D
    case 0x5B: break;                         //MOV E,E
    case 0x5C: state->e = state->h; break;    //MOV E,H
    case 0x5D: state->e = state->l; break;    //MOV E,L
    case 0x5E:                                //MOV E,M
    {
      uint16_t offset = (state->h << 8) | (state->l);
      state->e = state->memory[offset];
    }
    case 0x5F: state->e = state->a; break;    //MOV E,A
    case 0x60: state->h = state->b; break;    //MOV H,B
    case 0x61: state->h = state->c; break;    //MOV H,C
    case 0x62: state->h = state->d; break;    //MOV H,D
    case 0x63: state->h = state->e; break;    //MOV H,E
    case 0x64: break;                         //MOV H,H
    case 0x65: state->h = state->l; break;    //MOV H,L
    case 0x66:                                //MOV H,M
    {
      uint16_t offset = (state->h << 8) | (state->l);
      state->h = state->memory[offset];
    }
    case 0x67: state->h = state->a; break;    //MOV H,A
    case 0x68: state->l = state->b; break;    //MOV L,B
    case 0x69: state->l = state->c; break;    //MOV L,C
    case 0x6A: state->l = state->d; break;    //MOV L,D
    case 0x6B: state->l = state->e; break;    //MOV L,E
    case 0x6C: state->l = state->h; break;    //MOV L,H
    case 0x6D: break;                         //MOV L,L
    case 0x6E:                                //MOV L,M
    {
      uint16_t offset = (state->h << 8) | (state->l);
      state->l = state->memory[offset];
    }
    case 0x6F: state->l = state->a; break;    //MOV A,A
    case 0x70:                                //MOV M,B
    {
      uint16_t location = (state->h << 8) | (state->l);
      state->memory[location] = state->b;
      break;
    }
    case 0x71:                              //MOV M,C
    {
      uint16_t location = (state->h << 8) | (state->l);
      state->memory[location] = state->c;
      break;
    }
    case 0x72:                              //MOV M,D
    {
      uint16_t location = (state->h << 8) | (state->l);
      state->memory[location] = state->d;
      break;
    }
    case 0x73:                                //MOV M,E
    {
      uint16_t location = (state->h << 8) | (state->l);
      state->memory[location] = state->e;
      break;
    }
    case 0x74:                                //MOV M,H
    {
      uint16_t location = (state->h << 8) | (state->l);
      state->memory[location] = state->h;
      break;
    }
    case 0x75:                                //MOV M,L
    {
      uint16_t location = (state->h << 8) | (state->l);
      state->memory[location] = state->l;
      break;
    }
    // case 0x76: printf("HLT\n"); break;
    case 0x77:                                //MOV M,A
    {
      uint16_t location = (state->h << 8) | (state->l);
      state->memory[location] = state->a;
      break;
    }
    case 0x78: state->a = state->b; break;    //MOV A,B
    case 0x79: state->a = state->c; break;    //MOV A,C
    case 0x7A: state->a = state->d; break;    //MOV A,D
    case 0x7B: state->a = state->e; break;    //MOV A,E
    case 0x7C: state->a = state->h; break;    //MOV A,H
    case 0x7D: state->a = state->l; break;    //MOV A,L
    case 0x7E:                                //MOV A,M
    {
      uint16_t offset = (state->h << 8) | (state->l);
      state->a = state->memory[offset];
    }
    case 0x7F: break;                         //MOV A,A
    case 0x80:    //ADD   B
      {
        // do the math with higher precision so we can capture the carry out
        uint16_t answer = (uint16_t)state->a + (uint16_t)state->b;

        // Zero flag: if a result is zero, set the flag to zero, else clear the
        // flag
        if ((answer & 0xFF) == 0)
        {
          state->cc.z = 1;
        }
        else
        {
          state->cc.z = 0;
        }

        // Sign flag: if bit 7 is set, set the sign flag
        // else, clear the sign flag
        if(answer & 0x80)
        {
          state->cc.s = 1;
        }
        else
        {
          state->cc.s = 0;
        }

        // Carry flag
        if(answer > 0xFF)
        {
          state->cc.cy = 1;
        }
        else
        {
          state->cc.cy = 0;
        }

        // Parity is handled by a subroutine
        state->cc.p = parity(answer & 0xFF);

        state->a = answer & 0xFF;
        break;
      }
    case 0x81:    //ADD  C
    {
      uint16_t answer = (uint16_t) state->a + (uint16_t) state->c;
      setAddFlags(state, answer);
      state->a = answer & 0xFF;
      break;
    }
    case 0x82:   //ADD   D
    {
      uint16_t answer = (uint16_t) state->a + (uint16_t) state->d;
      setAddFlags(state, answer);
      state->a = answer & 0xFF;
      break;
    }
    case 0x83:    //ADD   E
    {
      uint16_t answer = (uint16_t) state->a + (uint16_t) state->e;
      setAddFlags(state, answer);
      state->a = answer & 0xFF;
      break;
    }
    case 0x84:    //ADD   H
    {
      uint16_t answer = (uint16_t) state->a + (uint16_t) state->h;
      setAddFlags(state, answer);
      state->a = answer & 0xFF;
      break;
    }
    case 0x85:    //ADD   L
    {
      uint16_t answer = (uint16_t) state->a + (uint16_t) state->l;
      setAddFlags(state, answer);
      state->a = answer & 0xFF;
      break;
    }
    case 0x86:    //ADD   M
    {
      uint16_t offset = (state->h << BYTE) | (state->l);
      uint16_t answer = (uint16_t) state->a + (uint16_t) state->memory[offset];
      setAddFlags(state, answer);
      state->a = answer & 0xFF;
      break;
    }
    case 0x87:    //ADD   A
    {
      uint16_t answer = (uint16_t) state->a + (uint16_t) state->h;
      setAddFlags(state, answer);
      state->a = answer & 0xFF;
      break;
    }
    case 0x88:    //ADC B
    {
      uint16_t answer = (uint16_t) state->a + (uint16_t) state->b +
                        (uint16_t) state->cc.cy;
      setAddFlags(state, answer);
      state->a = answer & 0xFF;
      break;
    }
    case 0x89:     //ADC C
    {
      uint16_t answer = (uint16_t) state->a + (uint16_t) state->c +
                        (uint16_t) state->cc.cy;
      setAddFlags(state, answer);
      state->a = answer & 0xFF;
      break;
    }
    case 0x8A:    //ADC D
    {
      uint16_t answer = (uint16_t) state->a + (uint16_t) state->d +
                        (uint16_t) state->cc.cy;
      setAddFlags(state, answer);
      state->a = answer & 0xFF;
      break;
    }
    case 0x8B:    //ADC E
    {
      uint16_t answer = (uint16_t) state->a + (uint16_t) state->e +
                        (uint16_t) state->cc.cy;
      setAddFlags(state, answer);
      state->a = answer & 0xFF;
      break;
    }
    case 0x8C:    //ADC H
    {
      uint16_t answer = (uint16_t) state->a + (uint16_t) state->h +
                        (uint16_t) state->cc.cy;
      setAddFlags(state, answer);
      state->a = answer & 0xFF;
      break;
    }
    case 0x8D:     //ADC L
    {
      uint16_t answer = (uint16_t) state->a + (uint16_t) state->l +
                        (uint16_t) state->cc.cy;
      setAddFlags(state, answer);
      state->a = answer & 0xFF;
      break;
    }
    case 0x8E:     //ADC M
    {
      uint16_t offset = (state->h << BYTE) | state->l;
      uint16_t answer = (uint16_t) state->a + (uint16_t) state->memory[offset] +
                        (uint16_t) state->cc.cy;
      setAddFlags(state, answer);
      state->a = answer & 0xFF;
      break;
    }
    case 0x8F:     //ADC A
    {
      uint16_t answer = (uint16_t) state->a + (uint16_t) state->a +
                        (uint16_t) state->cc.cy;
      setAddFlags(state, answer);
      state->a = answer & 0xFF;
      break;
    }
    case 0x90:     //SUB B
    {
      uint16_t answer = (uint16_t) state->a  + ((~state->b) + 1);
      setSubFlags(state, answer);
      state->a = answer & 0xFF;
      break;
    }
    case 0x91:     //SUB C
    {
      uint16_t answer = (uint16_t) state->a  + ((~state->c) + 1);
      setSubFlags(state, answer);
      state->a = answer & 0xFF;
      break;
    }
    case 0x92:     //SUB D
    {
      uint16_t answer = (uint16_t) state->a  + ((~state->d) + 1);
      setSubFlags(state, answer);
      state->a = answer & 0xFF;
      break;
    }
    case 0x93:     //SUB E
    {
      uint16_t answer = (uint16_t) state->a  + ((~state->e) + 1);
      setSubFlags(state, answer);
      state->a = answer & 0xFF;
      break;
    }
    case 0x94:     //SUB H
    {
      uint16_t answer = (uint16_t) state->a  + ((~state->h) + 1);
      setSubFlags(state, answer);
      state->a = answer & 0xFF;
      break;
    }
    case 0x95:     //SUB L
    {
      uint16_t answer = (uint16_t) state->a  + ((~state->l) + 1);
      setSubFlags(state, answer);
      state->a = answer & 0xFF;
      break;
    }
    case 0x96:     //SUB M
    {
      uint16_t offset = (state->h << BYTE) | (state->l);
      uint16_t answer = (uint16_t) state->a  + ((~state->memory[offset]) + 1);
      setSubFlags(state, answer);
      state->a = answer & 0xFF;
      break;
    }
    case 0x97:     //SUB A
    {
      uint16_t answer = (uint16_t) state->a  + ((~state->a) + 1);
      setSubFlags(state, answer);
      state->a = answer & 0xFF;
      break;
    }
    case 0x98:     //SBB B
    {
      uint16_t answer = (uint16_t) state->a  + ((~(state->b + state->cc.cy)) + 1);
      setSubFlags(state, answer);
      state->a = answer & 0xFF;
      break;
    }
    case 0x99:     //SBB C
    {
      uint16_t answer = (uint16_t) state->a  + ((~(state->c + state->cc.cy)) + 1);
      setSubFlags(state, answer);
      state->a = answer & 0xFF;
      break;
    }
    case 0x9A:     //SBB D
    {
      uint16_t answer = (uint16_t) state->a  + ((~(state->d + state->cc.cy)) + 1);
      setSubFlags(state, answer);
      state->a = answer & 0xFF;
      break;
    }
    case 0x9B:     //SBB E
    {
      uint16_t answer = (uint16_t) state->a  + ((~(state->e + state->cc.cy)) + 1);
      setSubFlags(state, answer);
      state->a = answer & 0xFF;
      break;
    }
    case 0x9C:     //SBB H
    {
      uint16_t answer = (uint16_t) state->a  + ((~(state->h + state->cc.cy)) + 1);
      setSubFlags(state, answer);
      state->a = answer & 0xFF;
      break;
    }
    case 0x9D:     //SBB L
    {
      uint16_t answer = (uint16_t) state->a  + ((~(state->l + state->cc.cy)) + 1);
      setSubFlags(state, answer);
      state->a = answer & 0xFF;
      break;
    }
    case 0x9E:     //SBB M
    {
      uint16_t offset = (state->h << BYTE) | (state->l);
      uint16_t answer = (uint16_t) state->a  + ((~(state->memory[offset] + state->cc.cy)) + 1);
      setSubFlags(state, answer);
      state->a = answer & 0xFF;
      break;
    }
    case 0x9F:     //SBB A
    {
      uint16_t answer = (uint16_t) state->a  + ((~(state->a + state->cc.cy)) + 1);
      setSubFlags(state, answer);
      state->a = answer & 0xFF;
      break;
    }
    case 0xA0:     //ANA B
    {
      state->a &= state->b;
      setLogicFlags(state, state->a);
      break;
    }
    case 0xA1:     //ANA C
    {
      state->a &= state->c;
      setLogicFlags(state, state->a);
      break;
    }
    case 0xA2:     //ANA D
    {
      state->a &= state->d;
      setLogicFlags(state, state->a);
      break;
    }
    case 0xA3:     //ANA E
    {
      state->a &= state->e;
      setLogicFlags(state, state->a);
      break;
    }
    case 0xA4:     //ANA H
    {
      state->a &= state->h;
      setLogicFlags(state, state->a);
      break;
    }
    case 0xA5:     //ANA L
    {
      state->a &= state->l;
      setLogicFlags(state, state->a);
      break;
    }
    case 0xA6:     //ANA M
    {
      uint16_t location = (state->h << BYTE) | (state->l);
      state->a &= state->memory[location];
      setLogicFlags(state, state->a);
      break;
    }
    case 0xA7:     //ANA A
    {
      state->a &= state->a;
      setLogicFlags(state, state->a);
      break;
    }
    case 0xA8:     //XRA B
    {
      state->a ^= state->b;
      setLogicFlags(state, state->a);
      break;
    }
    case 0xA9:     //XRA C
    {
      state->a ^= state->c;
      setLogicFlags(state, state->a);
      break;
    }
    case 0xAA:     //XRA D
    {
      state->a ^= state->d;
      setLogicFlags(state, state->a);
      break;
    }
    case 0xAB:     //XRA E
    {
      state->a ^= state->e;
      setLogicFlags(state, state->a);
      break;
    }
    case 0xAC:     //XRA H
    {
      state->a ^= state->h;
      setLogicFlags(state, state->a);
      break;
    }
    case 0xAD:     //XRA L
    {
      state->a ^= state->l;
      setLogicFlags(state, state->a);
      break;
    }
    case 0xAE:     //XRA M
    {
      uint16_t location = (state->h << BYTE) | (state->l);
      state->a ^= state->memory[location];
      setLogicFlags(state, state->a);
      break;
    }
    case 0xAF:     //XRA A
    {
      state->a ^= state->a;
      setLogicFlags(state, state->a);
      break;
    }
    case 0xB0:     //ORA
    {
      state->a |= state->b;
      setLogicFlags(state, state->a);
      break;
    }
    case 0xB1:     //ORA
    {
      state->a |= state->c;
      setLogicFlags(state, state->a);
      break;
    }
    case 0xB2:     //ORA
    {
      state->a |= state->d;
      setLogicFlags(state, state->a);
      break;
    }
    case 0xB3:     //ORA
    {
      state->a |= state->e;
      setLogicFlags(state, state->a);
      break;
    }
    case 0xB4:     //ORA
    {
      state->a |= state->h;
      setLogicFlags(state, state->a);
      break;
    }
    case 0xB5:     //ORA
    {
      state->a |= state->l;
      setLogicFlags(state, state->a);
      break;
    }
    case 0xB6:     //ORA
    {
      uint16_t location = (state->h << BYTE) | (state->l);
      state->a |= state->memory[location];
      setLogicFlags(state, state->a);
      break;
    }
    case 0xB7:     //ORA
    {
      state->a |= state->a;
      setLogicFlags(state, state->a);
      break;
    }
    // case 0xB8: printf("CMP    B\n"); break;
    // case 0xB9: printf("CMP    C\n"); break;
    // case 0xBA: printf("CMP    D\n"); break;
    // case 0xBB: printf("CMP    E\n"); break;
    // case 0xBC: printf("CMP    H\n"); break;
    // case 0xBD: printf("CMP    L\n"); break;
    // case 0xBE: printf("CMP    M\n"); break;
    // case 0xBF: printf("CMP    A\n"); break;
    // case 0xC0: printf("RNZ\n"); break;
    // case 0xC1: printf("POP    B\n"); break;
    case 0xC2:     //JNZ
    {
      if(state->cc.z == 0)
      {
        return state->pc = (opcode[2] << 8) | opcode[1];
      }
      // Branch not taken
      else
      {
        state->pc += 2;
      }
      break;
    }
    case 0xC3:     //JMP ADDRESS
    {
      return state->pc = (opcode[2] << 8) | opcode[1];
    }
    case 0xC4:     //CNZ ADDRESS
    if(state->cc.z == 1)
    {
      updateStack(state);
      return state->pc = (opcode[2] << 8) | opcode[1];
    }
    // Branch not taken
    else
    {
      state->pc += 2;
    }
    break;
    // case 0xC5: printf("PUSH   B\n"); break;
    case 0xC6:    //ADI   byte
    {
      uint16_t answer = (uint16_t)state->a + (uint16_t)opcode[1];
      setAddFlags(state, answer);
      state->pc += 1;
      break;
    }
    // case 0xC7: printf("RST    0\n"); break;
    // case 0xC8: printf("RZ\n"); break;
    case 0xC9:     //RET
    {
      state->pc = state->memory[state->sp] | (state->memory[state->sp + 1] << 8);
      state->sp += 2;
      break;
    }
    case 0xCA:    //JZ
    {
      if(state->cc.z == 1)
      {
        return state->pc = (opcode[2] << 8) | opcode[1];
      }
      // Branch not taken
      else
      {
        state->pc += 2;
      }
      break;
    }
    case 0xCB:    //JMP ADDRESS
    {
      return state->pc = (opcode[2] << 8) | opcode[1];
    }
    case 0xCC:     //CZ ADDRESS
    {
      if(state->cc.z == 0)
      {
        updateStack(state);
        return state->pc = (opcode[2] << 8) | opcode[1];
      }
      else
      {
          state->pc += 2;
      }
      break;
    }
    case 0xCD:     //CALL ADDRESS  //maybe ret should be +2 instead???
    {
      updateStack(state);
      return state->pc = (opcode[2] << 8) | opcode[1];
    }
    // case 0xCE: printf("ACI    #$%02x\n", code[1]); opbytes=2; break;
    // case 0xCF: printf("RST    1\n"); break;
    // case 0xD0: printf("RNC\n"); break;
    // case 0xD1: printf("POP    D\n"); break;
    case 0xD2:     //JNC
    {
      if(state->cc.cy == 0)
      {
        return state->pc = (opcode[2] << 8) | opcode[1];
      }
      // Branch not taken
      else
      {
        state->pc += 2;
      }
      break;
    }
    // case 0xD3: printf("OUT    #$%02x\n", code[1]); opbytes=2; break;
    case 0xD4:     //CNC ADDRESS
    {
      if(state->cc.cy == 0)
      {
        updateStack(state);
        return state->pc = (opcode[2] << 8) | opcode[1];
      }
      else
      {
          state->pc += 2;
      }
      break;
    }
    // case 0xD5: printf("PUSH   D\n"); break;
    // case 0xD6: printf("SUI    #$%02x\n", code[1]); opbytes=2; break;
    // case 0xD7: printf("RST    2\n"); break;
    // case 0xD8: printf("RC\n"); break;
    case 0xD9:     //RET
    {
      state->pc = state->memory[state->sp] | (state->memory[state->sp + 1] << 8);
      state->sp += 2;
      break;
    }
    case 0xDA:     //JC
    {
      if(state->cc.cy == 1)
      {
        return state->pc = (opcode[2] << 8) | opcode[1];
      }
      // Branch not taken
      else
      {
        state->pc += 2;
      }
      break;
    }
    // case 0xDB: printf("IN     #$%02x\n", code[1]); opbytes=2; break;
    case 0xDC:    //CC ADDRESS
    {
      if(state->cc.cy == 1)
      {
        updateStack(state);
        return state->pc = (opcode[2] << 8) | opcode[1];
      }
      else
      {
          state->pc += 2;
      }
      break;
    }
    case 0xDD:    //CALL ADDRESS
    {
      updateStack(state);
      return state->pc = (opcode[2] << 8) | opcode[1];
    }
    // case 0xDE: printf("SBI    #$%02x\n", code[1]); opbytes=2; break;
    // case 0xDF: printf("RST    3\n"); break;
    // case 0xE0: printf("RPO\n"); break;
    // case 0xE1: printf("POP    H\n"); break;
    case 0xE2:     //JPO
    {
      if(state->cc.p == 0)
      {
        return state->pc = (opcode[2] << 8) | opcode[1];
      }
      // Branch not taken
      else
      {
        state->pc += 2;
      }
      break;
    }
    // case 0xE3: printf("XTHL\n"); break;
    case 0xE4:     //CPO ADDRESS
    {
      if(state->cc.p == 0)
      {
        updateStack(state);
        return state->pc = (opcode[2] << 8) | opcode[1];
      }
      else
      {
          state->pc += 2;
      }
      break;
    }
    // case 0xE5: printf("PUSH   H\n"); break;
    // case 0xE6: printf("ANI    #$%02x\n", code[1]); opbytes=2; break;
    // case 0xE7: printf("RST    4\n"); break;
    // case 0xE8: printf("RPE\n"); break;
    // case 0xE9: printf("PCHL\n"); break;
    case 0xEA:     //JPE
    {
      if(state->cc.p == 1)
      {
        return state->pc = (opcode[2] << 8) | opcode[1];
      }
      // Branch not taken
      else
      {
        state->pc += 2;
      }
      break;
    }
    // case 0xEB: printf("XCHG\n"); break;
    case 0xEC:     //CPE ADDRESS
    {
      if(state->cc.p == 1)
      {
        updateStack(state);
        return state->pc = (opcode[2] << 8) | opcode[1];
      }
      else
      {
          state->pc += 2;
      }
      break;
    }
    case 0xED:     //CALL ADDRESS
    {
      updateStack(state);
      return state->pc = (opcode[2] << 8) | opcode[1];
    }
    // case 0xEE: printf("XRI    #$%02x\n", code[1]); opbytes=2; break;
    // case 0xEF: printf("RST    5\n"); break;
    // case 0xF0: printf("RP\n"); break;
    // case 0xF1: printf("POP    PSW\n"); break;
    // case 0xF2:     //JP
    {
      if(state->cc.s == 0)
      {
        return state->pc = (opcode[2] << 8) | opcode[1];
      }
      // Branch not taken
      else
      {
        state->pc += 2;
      }
      break;
    }
    // case 0xF3: printf("DI\n"); break;
    case 0xF4:     //CP ADDRESS
    {
      if(state->cc.s == 0)
      {
        updateStack(state);
        return state->pc = (opcode[2] << 8) | opcode[1];
      }
      else
      {
          state->pc += 2;
      }
      break;
    }
    // case 0xF5: printf("PUSH   PSW\n"); break;
    // case 0xF6: printf("ORI    #$%02x\n", code[1]); opbytes=2; break;
    // case 0xF7: printf("RST    6\n"); break;
    // case 0xF8: printf("RM\n"); break;
    // case 0xF9: printf("SPHL\n"); break;
    case 0xFA:     //JM
    {
      if(state->cc.s == 1)
      {
        return state->pc = (opcode[2] << 8) | opcode[1];
      }
      // Branch not taken
      else
      {
        state->pc += 2;
      }
      break;
    }
    // case 0xFB: printf("EI\n"); break;
    case 0xFC:     //CM ADDRESS
    {
      if(state->cc.s == 1)
      {
        updateStack(state);
        return state->pc = (opcode[2] << 8) | opcode[1];
      }
      else
      {
          state->pc += 2;
      }
      break;
    }
    case 0xFD:     //CALL ADRESS
    {
      updateStack(state);
      return state->pc = (opcode[2] << 8) | opcode[1];
    }
    // case 0xFE: printf("CPI    #$%02x\n", code[1]); opbytes=2; break;
    // case 0xFF: printf("RST    7\n"); break;
    default: printf("Code not found\n"); exit(1);
  }
  return state->pc += 1;
}

int main(void)
{
  // disasemble("invaders.f");
  return 0;
}
