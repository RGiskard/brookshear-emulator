/**
 *  Run the simulation.
 */
void _runBrookshear(void);

/**
 *  Print heading for output.
 */
void _printHeading(void);

/**
 *  Create array of instructions from file.
 */
unsigned char* _fileReader(void);

/**
 *  Print the PC, current instruction, and current register values.
 */
void _printValues(unsigned char*, unsigned char, unsigned char*);

/**
 *  Process instructions; updates memory, program counter, and registers.
 */
void _processInstruction(unsigned char*, unsigned char*, unsigned char*, unsigned int*);
