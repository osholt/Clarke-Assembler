// example.asm
//
// When coupled with the right data, causes the image to become dynamically animated depedning on switch positions.
// Oliver Holt and Robin Fells
// December 2015
//
// Registers cheat sheet
// R0: Current memory value
// R1: Address of LEDs
// R2: Used to set R3 correctly
// R3: Contains current VGA RAM address
// R4: Used to store pixel value (RRRGGGBB) and as temporary register to act as a 1 to increment with.
// R9: Used as padding for JR instructions.
// R10: Starting address of VGA RAM
// R12: Used as temporary register to act as a 1 to increment with.
// R13: Used to define the the highest frequency present on the LED outputs. (f= (72/R13) Hz)
// R20: Used to store the value of a particular colour for red.
// R21: Used to store framce counter (FC).
// R22: Used as temporary variable for colour red.
// R23: Used as tempoirary variable for FC.
// R30: Used to store the value of a particular colour for green.
// R32: Used as temporary variable for colour green.
// R51: Used as part of delay loop.
// R52: Used as part of delay loop.
// R53: Used as part of delay loop.
// R55: Stores address of the switches.
// R56: Used as temporary variable to account for weird stripes when using the switches to define the colours.

/////////////////////////////////////////////////////////////////////////
//							One time Setup
/////////////////////////////////////////////////////////////////////////
//Set Colour values to test
LIL R20, 37
LIU R20, 23	
LIU R20, 60	//Set R20 to 0xE5E5 -- RED

LIL R30, 53	
LIU R30, 20
LIU R30, 38 //Set R30 to 0x3535 -- GREEN

LIL R21, 0	//Set FC to 0

LIL R13, 50 //set R13 to 50   f= (72/R13) Hz

LIL R55, 0	// Set R55 to the address of the switches (0xC000)
LIU R55, 0x38 

START:
LIL R0, 0		// Clear flags register

LIL R10, 0		//R10 = 0x4000
LIU R10, 0
LIU R10, 40		
MOVE R3, R10	//copies R10 to R3 to use it as a temporary variable

LIL R1, 0      // Set R1 to the Address of the LEDs (0xC200)
LIU R1, 0x08 
LIU R1, 0x38
/////////////////////////////////////////////////////////////////////////
//							End of One time Setup
/////////////////////////////////////////////////////////////////////////




/////////////////////////////////////////////////////////////////////////
//							Change text to Green
/////////////////////////////////////////////////////////////////////////

LOOP:

LIL R2, -1		//sets R3 somehow (I worked it out once)
LIU R2, 0x2F 
NAND R3, R2
LIL R2, -1		
LIU R2, 0x37 
NAND R3, R2



LOAD R4, (R0)	// Get the switch values to R4
LOAD R4, (R0)   //R4 contains current working pixel
STORE (R3), R4  // Copy R4 to the the VGA display

LIL R4, 1      // Increment R3 - Whatever value it has is fine each time we go 
               // past this point we get a different value in the VGA RAM
LIL R62, 0		//clears flags register
ADC R3, R4
ADC R0, R4
LIL R62, 0
SUB R10, R0
JR Z, SKIP
LIL R9, 0
SKIP: JR A, LOOP //continue outputting this frame until it the required time has elap
LIL R9, 0

LIL R12, 1
LIL R62, 0

/////////////////////////////////////////////////////////////////////////
//							End of Change text to Green
/////////////////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////////////
//							Start of Delay
/////////////////////////////////////////////////////////////////////////


LIL R50, 1

LIL R51, 0
LIU R51, 31
LIU R51, 63

LIL R52, 1
DEC: LIL R62, 0
ADC R50, R52
JR  NZ, DEC
LIL R9, 0

LIL R62, 0
ADC R51, R52
JR  NZ, DEC
LIL R9, 0

/////////////////////////////////////////////////////////////////////////
//							End of Delay
/////////////////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////////////
//							Change text to Colour defined by switches
/////////////////////////////////////////////////////////////////////////



LIL R0, 0		// Set R0 to the start address of the memory

LIL R10, 0
LIU R10, 0
LIU R10, 40		//R10 = 0x4000
MOVE R3, R10

//LIU R0, 0x38 
LIL R1, 0      // Set R1 to the Address of the LEDs (0xC200)
LIU R1, 0x08 
LIU R1, 0x38


GO:

LIL R2, -1	
LIU R2, 0x2F 
NAND R3, R2
LIL R2, -1		
LIU R2, 0x37 
NAND R3, R2



LOAD R4, (R0)
LOAD R4, (R0)   // Get the switch values to R4


MOVE R23, R21 	//temp value for FC
LIL R62, 0
SUB R23, R13

//LIL R21, 0	//Set FC to 0


MOVE R22, R20
MOVE R32, R30	//Temp values for colours
LIL R62, 0
SUB  R32, R4		
JR  NZ, NEXT	//if zero run following	
LIL R9, 0
MOVE R4, R20	//Set R4 to Red

NEXT:
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
STORE (R3), R4  // Copy R4 to the the VGA display

LIL R4, 1      // Increment R3 - Whatever value it has is fine each time we go 
               // past this point we get a different value in the VGA RAM
LIL R62, 0
ADC R3, R4
ADC R0, R4
LIL R62, 0
SUB R10, R0
JR  Z, SM
LIL R9, 0
JR  A, GO 
SM: LIL R9, 0

LIL R12, 1
LIL R62, 0
ADC R21, R12		//Inc Frame counter



//////////////////Delay
LIL R50, 1

LIL R51, 0
LIU R51, 31
LIU R51, 63

LIL R52, 1
TR: LIL R62, 0
ADC R50, R52
JR  NZ, TR
LIL R9, 0

LIL R62, 0
ADC R51, R52
JR  NZ, TR
LIL R9, 0
////////////////////


LOAD R20, (R55)
LOAD R20, (R55)
STORE (R1), 63

LIL  R56, 0
LIU  R56, 4
STORE (R1), R21
MUL R56, R20

LIL R62, 0
ADC R20, R56 


JR  A, START	//Jump to very beginning
LIL R9, 0		//padding

/////////////////////////////////////////////////////////////////////////
//			End of Change text to Colour defined by switches
/////////////////////////////////////////////////////////////////////////

