void Run_Brookshear(void)
{
    // Four bits in each index, 5th index contains the entire second byte.
    unsigned int instr_bits[5] = { 0 };

    unsigned char* memory = File_Reader();
    unsigned char program_counter = 0;
    unsigned char registers[N_REGISTERS] = { 0 };

    // Begin simulation loop.
    while (instr_bits[0] != 0xC)
    {
        // Split instruction bits.
        instr_bits[0] = (memory[program_counter] & 0xf0) >> 0x4;
        instr_bits[1] = memory[program_counter] & 0xf;
        instr_bits[2] = (memory[program_counter + 1] & 0xf0) >> 0x4;
        instr_bits[3] = memory[program_counter + 1] & 0xf;
        instr_bits[4] = memory[program_counter + 1];

        Print_Values(memory, program_counter, registers);
        Process_Instruction(&memory, &program_counter, &registers, &instr_bits);
    }

    free(memory);
}

unsigned char* File_Reader(void)
{
    FILE* fp = fopen(INSTR_FILE, "r");
    if (!fp)
    {
        fprintf(stderr, "Error! Can't open %s.\n", INSTR_FILE);
        exit(1);
    }

    unsigned char buffer[BUFF_SIZE] = { '\0' };
    unsigned int instruction = 0;
    unsigned char* memory = (char*)malloc(MEM_SIZE * sizeof(unsigned char));

    // -1 to account for the first increment in the first iteration.
    int i = -1;

    bool halt_exists = false;
    while (fgets(buffer, BUFF_SIZE, fp))
    {
        if (buffer[0] == 'C')
        {
            halt_exists = true;
        }

        // Split each line into two bytes, store in memory array.
        sscanf(buffer, "%X", &instruction);
        memory[++i] = (instruction & 0xff00) >> 0x8;
        memory[++i] = instruction & 0xff;
    }

    if (!halt_exists)
    {
        fprintf(stderr, "Error! %s doesn't contain a halt (C000) instruction.\n", INSTR_FILE);
        free(memory);
        exit(1);
    }

    // We're good.
    fclose(fp);
    return memory;
}

void Print_Values   (
                        unsigned char* _memory,
                        unsigned char _program_counter,
                        unsigned char* _registers
                    )
{
    fprintf(stdout, "%02X %02X%02X - [", _program_counter, _memory[_program_counter], _memory[_program_counter + 1]);
    for (int i = 0; i < 16; ++i)
    {
        // Proper formatting for final register, as per assignment brief.
        if (i == 15)
        {
            fprintf(stdout, "%02X]\n", _registers[i]);
        }
        else
        {
            fprintf(stdout, "%02X ", _registers[i]);
        }
    }
}

void Process_Instruction    (
                                unsigned char* _memory,
                                unsigned char* _program_counter,
                                unsigned char* _registers,
                                unsigned int* _instruction
                            )
{
    // Load register B with the value at memory address CD.
    if (_instruction[0] == 0x1)
    {
        _registers[_instruction[1]] = _memory[_instruction[4]];
        *_program_counter += 2;
    }

    // Load register B with the value CD.
    else if (_instruction[0] == 0x2)
    {
        _registers[_instruction[1]] = _instruction[4];
        *_program_counter += 2;
    }

    // Store the value in register B at memory address CD.
    else if (_instruction[0] == 0x3)
    {
        _memory[_instruction[4]] = _registers[_instruction[1]];
        *_program_counter += 2;
    }

    // Copy/move the value in register C to register D.
    else if (_instruction[0] == 0x4)
    {
        _registers[_instruction[3]] = _registers[_instruction[2]];
        *_program_counter += 2;
    }

    // Add the values in registers B and C and put the answer in register D.
    else if (_instruction[0] == 0x5)
    {
        _registers[_instruction[3]] = _registers[_instruction[1]] + _registers[_instruction[2]];
        *_program_counter += 2;
    }

    // OR the values in registers B and C and put the answer in register D.
    else if (_instruction[0] == 0x7)
    {
        _registers[_instruction[3]] = _registers[_instruction[1]] | _registers[_instruction[2]];
        *_program_counter += 2;
    }

    // AND the values in registers B and C and put the answer in register D.
    else if (_instruction[0] == 0x8)
    {
        _registers[_instruction[3]] = _registers[_instruction[1]] & _registers[_instruction[2]];
        *_program_counter += 2;
    }

    // XOR the values in registers B and C and put the answer in register D.
    else if (_instruction[0] == 0x9)
    {
        _registers[_instruction[3]] = _registers[_instruction[1]] ^ _registers[_instruction[2]];
        *_program_counter += 2;
    }

    // Rotate the contents of register B CD times to the right.
    else if (_instruction[0] == 0xA)
    {
        for (unsigned int i = 0; i < _instruction[4]; ++i)
        {
            unsigned int temp = _registers[_instruction[1]] & 0x01;
            _registers[_instruction[1]] = (_registers[_instruction[1]] >> 1) | (_registers[_instruction[1]] & temp) << 7;
        }

        *_program_counter += 2;
    }

    // Jump to the instruction located at memory address CD if the value in register B is equal to the value in register 0.
    else if (_instruction[0] == 0xB)
    {
        if ((_registers[_instruction[1]] == 0) || (_registers[_instruction[1]] == _registers[0]))
        {
            *_program_counter = _instruction[4];
        }
        else
        {
            *_program_counter += 2;
        }
    }
}
