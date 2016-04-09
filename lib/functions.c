#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "macros.h"
#include "functions.h"

void _runBrookshear(void) {
  _printHeading();

  // Four bits in each index, 5th index contains the entire second byte.
  unsigned int instr_bits[5]            = { 0 };
  unsigned char* memory                 = _fileReader();
  unsigned char program_counter         = 0;
  unsigned char registers[N_REGISTERS]  = { 0 };

  while (instr_bits[0] != 0xC) {
    instr_bits[0] = (memory[program_counter] & 0xf0) >> 0x4;
    instr_bits[1] = memory[program_counter] & 0xf;
    instr_bits[2] = (memory[program_counter + 1] & 0xf0) >> 0x4;
    instr_bits[3] = memory[program_counter + 1] & 0xf;
    instr_bits[4] = memory[program_counter + 1];

    _printValues(memory, program_counter, registers);
    _processInstruction(&memory[0], &program_counter, &registers[0], &instr_bits[0]);
  }

  free(memory);
}

void _printHeading(void) {
  fprintf(stdout, "PC INST - [R0 R1 R2 R3 R4 R5 R6 R7 R8 R9 RA RB RC RD RE RF]\n");
}

unsigned char* _fileReader(void) {
  FILE* fp = fopen(INSTR_FILE, "r");
  if (!fp) {
    fprintf(stderr, "Error! Can't open %s.\n", INSTR_FILE);
    exit(1);
  }

  char buffer[BUFF_SIZE]    = { '\0' };
  unsigned int instruction  = 0;
  unsigned char* memory     = (unsigned char*)malloc(MEM_SIZE * sizeof(unsigned char));

  // -1 to account for the initial increment,
  int i = -1;
  bool halt_exists = false;
  while (fgets(buffer, BUFF_SIZE, fp)) {
    if (buffer[0] == 'C') {
        halt_exists = true;
    }

    // Split each line into two bytes, store in memory array.
    sscanf(buffer, "%X", &instruction);
    memory[++i] = (instruction & 0xff00) >> 0x8;
    memory[++i] = instruction & 0xff;
  }

  if (!halt_exists) {
    fprintf(stderr, "Error! %s doesn't contain a halt (C000) instruction.\n", INSTR_FILE);
    free(memory);
    exit(1);
  }

  fclose(fp);
  return memory;
}

void _printValues(unsigned char*    _memory,
                  unsigned char     _program_counter,
                  unsigned char*    _registers)
{
  fprintf(stdout, "%02X %02X%02X - [", _program_counter, _memory[_program_counter], _memory[_program_counter + 1]);

  for (int i = 0; i < 16; ++i) {
    if (i == 15) {
      fprintf(stdout, "%02X]\n", _registers[i]);
    } else {
      fprintf(stdout, "%02X ", _registers[i]);
    }
  }
}

void _processInstruction(unsigned char*   _memory,
                         unsigned char*   _program_counter,
                         unsigned char*   _registers,
                         unsigned int*    _instruction)
{
  switch(_instruction[0]) {

    // Load register B with the value at memory address CD.
    case 0x1:
      _registers[_instruction[1]] = _memory[_instruction[4]];
      break;

    // Load register B with the value CD.
    case 0x2:
      _registers[_instruction[1]] = _instruction[4];
      break;

    // Store the value in register B at memory address CD.
    case 0x3:
      _memory[_instruction[4]] = _registers[_instruction[1]];
      break;

    // Copy/move the value in register C to register D.
    case 0x4:
      _registers[_instruction[3]] = _registers[_instruction[2]];
      break;

    // Add the values in registers B and C and put the answer in register D.
    case 0x5:
      _registers[_instruction[3]] = _registers[_instruction[1]] + _registers[_instruction[2]];
      break;

    // OR the values in registers B and C and put the answer in register D.
    case 0x7:
      _registers[_instruction[3]] = _registers[_instruction[1]] | _registers[_instruction[2]];
      break;

    // AND the values in registers B and C and put the answer in register D.
    case 0x8:
      _registers[_instruction[3]] = _registers[_instruction[1]] & _registers[_instruction[2]];
      break;

    // XOR the values in registers B and C and put the answer in register D.
    case 0x9:
      _registers[_instruction[3]] = _registers[_instruction[1]] ^ _registers[_instruction[2]];
      break;

    // Rotate the contents of register B CD times to the right.
    case 0xA:
      for (unsigned int i = 0; i < _instruction[4]; ++i) {
          unsigned int temp = _registers[_instruction[1]] & 0x01;
          _registers[_instruction[1]] = (_registers[_instruction[1]] >> 1) | (_registers[_instruction[1]] & temp) << 7;
      }
      break;

    /**
     *  Jump to the instruction located at memory address CD if the value in register B is equal to the value in
     *  register 0.
     */
    case 0xB:
      *_program_counter = (_registers[_instruction[1]] == 0) || (_registers[_instruction[1]] == _registers[0])
        ? _instruction[4]
        // Cast to prevent comparison warning.
        : *_program_counter + (unsigned)2;
      return;

    case 0xC:
      return;

    default:
      fprintf(stderr, "Error: Failed reading instruction.");
      return;
  }

  *_program_counter += 2;
}
