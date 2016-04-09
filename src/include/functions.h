/**
 *  Run the simulation.
 */
void Run_Brookshear(void);

/**
 *  Print heading for output.
 */
void Print_Heading(void);

/**
 *  Create array of instructions from file.
 */
unsigned char* File_Reader(void);

/**
 *  Print the PC, current instruction, and current register values.
 */
void Print_Values(unsigned char*, unsigned char, unsigned char*);

/**
 *  Process instructions; updates memory, program counter, and registers.
 */
void Process_Instruction(unsigned char*, unsigned char*, unsigned char*, unsigned int*);
