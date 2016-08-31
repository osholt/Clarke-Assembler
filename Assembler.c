/*
Copyright and created by Oliver Holt (osholt@me.com).

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.


*/

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

void removePunctuation(char *p);

typedef struct //stores label information
{
	int lineNumber;
	char name[300];
} sLabel;

typedef struct //stores instruction information
{
	int instruction;
	int src;
	int dest;
	int offset;
	int condition;
	int hex;
} sInstruction;


int main(int argc, char *argv[])
{
	char* inFileName;
	char* outFileName;
	int display;


	/*Conditional statements to assess whether the arguments passed to the program are
	valid and if so then assign inFileName, outFileName and display as appropriate*/
	if (argc < 3)											//Initial check to make sure correct number of arguments are passed to the program 
	{														//Prevents memory access violation if no arguments are passed and exits gracefully
		printf("Invalid number of arguments.\n");
		printf("Press enter to exit");
		while (getchar() != '\n');
		exit(1);
	}

	if (argc == 3)											//First of 3 scenarios: do not display anything, just assemble.
	{
		inFileName = argv[1];
		outFileName = argv[2];
		display = 0;
	}
	else if ((argc == 4)&(strcmp(argv[1], "-d") == 0))		//Second of 3 scenrions: display and assemble.
	{
		inFileName = argv[2];
		outFileName = argv[3];
		display = -1;

	}
	else													//Third of 3 scenarios: the given parameters don't make sense.
	{
		printf("Invalid number of arguments and flags.\n");
		printf("Press enter to exit");
		while (getchar() != '\n');
		exit(1);
	}

	/*If the program has made it to this point, it means the arguements
	passed to it from the command line appeared to be valid.*/
	if (display != 0)
	{
		printf("This is free software created by osholt licenced under the GPLv3. For more information, check the source file.\n");
		printf("Now attempting to assemble the code from %s into %s.\n\n", inFileName, outFileName);
	}

	//arrarys of strings to use in comparisons later
	char instructions[16][6] = { {"JR"},{"LOAD"},{"STORE"},{"MOVE"},{"NAND"},{"NOR"},{"ROL"},{"ROR"},{"LIL"},{"LIU"},{"ADC"},{"SUB"},{"DIV"},{"MOD"},{"MUL"},{"MUH"} };
	char conditions[8][4] = { {"C,"},{"Z,"},{"N,"},{"P,"},{"V,"},{"NC,"},{"NZ,"},{"A,"} };

	FILE* InFile;
	InFile = fopen(inFileName, "r");

	FILE* TempFile;
	TempFile = fopen("temp.txt", "w");

	FILE* OutFile;
	OutFile = fopen(outFileName, "w");


	//code to refactor file to remove unnecessary whitespace and comments
	char lineBuffer[300];
	while (1)
	{
		if (fgets(lineBuffer, 300, InFile) == NULL)
		{
			break;
		}

		int clear = 0;
		for (int i = 0; i < 300; i++)			//strips comments
		{
			if (lineBuffer[i] == '/')
			{
				if (lineBuffer[i + 1] == '/' )
				{
					clear = 1;
				}
			}
			if (clear == 1)
			{
				lineBuffer[i] = '\n';
				clear = 2;
			}
			else if (clear == 2)
			{
				lineBuffer[i] = '\0';
			}
		}
		
		char* bufferOffsetPtr = lineBuffer;
		int offset = 0;
		for (int i = 0; i < 300; i++)			//strips leading whitespace
		{
			if (lineBuffer[i] == ' ')
			{
				offset++;
			}
			else if (lineBuffer[i] == '\t')
			{
				offset++;
			}
			else
			{
				break;
			}
		}
		bufferOffsetPtr += offset;

		int strip = 0;
		for (int i = 99; i > -1; i--)			//strips trailing whitespace
		{
			if (strip == 0)
			{
				if (lineBuffer[i] == '\n')
				{
					strip = 1;
				}
			}
			if (strip == 1)
			{
				if (lineBuffer[i - 1] == ' ')
				{
					lineBuffer[i] = '\0';
					lineBuffer[i - 1] = '\n';
				}
				else if (lineBuffer[i - 1] == '\t')
				{
					lineBuffer[i] = '\0';
					lineBuffer[i - 1] = '\n';
				}
				else
				{
					break;
				}
			}
		}

		for (int i = 0; i < 300; i++)			//forces labels to be on a line that is executed
		{
			if (lineBuffer[i] == ':')
			{
				if (lineBuffer[i + 1] == '\n')
				{
					lineBuffer[i + 1] = ' ';
					lineBuffer[i + 2] = '\0';
				}
			}
		}

		if (bufferOffsetPtr[0] != '\n')				//strips empty lines
		{
			fprintf(TempFile, "%s", bufferOffsetPtr); //puts the file back together with all the comments etc. removed.
		}
	}
	fclose(InFile);
	fclose(TempFile);

	TempFile = fopen("temp.txt", "r");
	int labelNum = 0;
	while (1)										//counts number of labels
	{
		if (fgets(lineBuffer, 300, TempFile) == NULL)
		{
			break;
		}
		for (int i = 0; i < 300; i++)
		{
			if (lineBuffer[i] == ':')
			{
				labelNum++;
			}
		}
	}
	fclose(TempFile);
	if (display != 0)
	{
		if (labelNum != 1)
		{
			printf("%d labels found.\n", labelNum);
		}
		else
		{
			printf("1 label found.\n");
		}
	}

	
	sLabel* labelList = malloc(labelNum * sizeof(sLabel));
	int labelCount = 0;

	TempFile = fopen("temp.txt", "r");

	int lineNumber = -1;
	while (1)										//records labels and associated line number
	{
		lineNumber++;
		if (fgets(lineBuffer, 300, TempFile) == NULL)
		{
			break;
		}

		for (int i = 0; i < 300; i++)			
		{
			if (lineBuffer[i] == ':')
			{
				for (int j = 0; j < i; j++)
				{
					(labelList[labelCount]).name[j] = lineBuffer[j];
					(labelList[labelCount]).name[j+1] = '\0';
				}
				(labelList[labelCount]).lineNumber = lineNumber;

				if (display != 0)
				{
					printf("Label \"%s\" was found on line %d.\n", (labelList[labelCount]).name, (labelList[labelCount]).lineNumber);
				}

				labelCount++;
			}
		}

	}
	fclose(TempFile);
	if (display != 0)
	{
		printf("\n");
	}

	//prints bumf at top of output file
	fprintf(OutFile, "-- Produced using the Assembler created by osholt\n");
	fprintf(OutFile, "--\n");
	fprintf(OutFile, "DEPTH = 16384;           --The size of memory in words\n");
	fprintf(OutFile, "WIDTH = 16;             --The size of data in bits\n");
	fprintf(OutFile, "ADDRESS_RADIX = HEX;    --The radix for address values\n");
	fprintf(OutFile, "DATA_RADIX = HEX;       --The radix for data values\n");
	fprintf(OutFile, "CONTENT                 -- start of(address : data pairs)\n");
	fprintf(OutFile, "BEGIN\n");


	TempFile = fopen("temp.txt", "r");
	lineNumber = -1;
	sInstruction currentInstruction;
	char instruction[300];
	while (1)										//reads in instructions
	{
		lineNumber++;
		if (fgets(lineBuffer, 300, TempFile) == NULL)
		{
			break;
		}
		for (int i = 0; i < 300; i++)
		{
			instruction[i] = lineBuffer[i];
		}

		const char s[2] = " ";
		char *token;
		char *temp;

		/* get the first token */
		token = strtok(lineBuffer, s);

		/* walk through other tokens */

		for (int i = 0; token[i] != NULL; i++)	//checks whether first word in a line is a label and ignores it if it is
		{
			if (token[i] == ':')
			{
				token = strtok(NULL, s);
				break;
			}
		}
		currentInstruction.instruction = -1;
		for (int i = 0; i < 16; i++)			//cfinds correct opcode for instruction
		{
			if (strcmp(token, instructions[i]) == 0)
			{
				currentInstruction.instruction = i;
				break;
			}
		}
		if (currentInstruction.instruction == -1)
		{
			if (display != 0)
			{
				printf("Invalid instruction found on line %d. Press enter to exit.\n", lineNumber);
				while (getchar() != '\n');
			}
			exit(-1);
		}
		token = strtok(NULL, s);
		if (currentInstruction.instruction != 0)
		{
			temp = token;
			removePunctuation(temp);
			currentInstruction.dest = strtol(temp, NULL, 0);		//records dest
			token = strtok(NULL, s);
			temp = token;
			removePunctuation(temp);
			currentInstruction.src = strtol(token, NULL, 0);		//records src

			currentInstruction.hex = (currentInstruction.src & 63) + ((currentInstruction.dest & 63) << 6) + ((currentInstruction.instruction & 15) << 12);
			if (display != 0)
			{
				printf("Line %d: %s", lineNumber, instruction);
				printf("Opcode:%d\tDest:%d\tSrc:%d\t", currentInstruction.instruction, currentInstruction.dest, currentInstruction.src);
				printf("Hex: %04x\n\n", currentInstruction.hex);
			}

		}
		else
		{
			currentInstruction.condition = -1;
			for (int i = 0; i < 8; i++)
			{
				if (strcmp((conditions[i]), token) == 0)
				{
					currentInstruction.condition = i;			//records condition
					break;
				}
			}
			if (currentInstruction.condition == -1)
			{
				if (display != 0)
				{
					printf("Invalid condition found on line %d. Press enter to exit.\n", lineNumber);
					while (getchar() != '\n');
				}
				exit(-1);
			}

			token = strtok(NULL, s);

			int ofsSet = 0;
			for (int i = 0; i < labelNum; i++)
			{
				if (strcmp((labelList[i]).name, token))			//attempts to store offset by resolving label
				{
					currentInstruction.offset = (labelList[i]).lineNumber - (lineNumber + 1);
					ofsSet = 1;
					break;
				}
			}
			if (ofsSet == 0)									//otherwise just stores the value
			{
				currentInstruction.offset = strtol(token, NULL, 0);
			}
			currentInstruction.hex = (currentInstruction.offset & 511 ) + ((currentInstruction.condition & 7) << 9) + ((currentInstruction.instruction & 15) << 12);


			if (display != 0)
			{
				printf("Line %d: %s", lineNumber, instruction);
				printf("Opcode:%d\tCnd:%d\tOfs:%d\t", currentInstruction.instruction, currentInstruction.condition, currentInstruction.offset);
				printf("Hex: %04x\n\n", currentInstruction.hex);
			}
		}
		
		fprintf(OutFile, "%04x : %04x; -- %s", lineNumber, currentInstruction.hex, instruction);
	}
	fclose(TempFile);
	fprintf(OutFile, "\n\n");
	fprintf(OutFile, "%04x : 8fc0; -- LIL PC, R0 0\n", lineNumber);
	lineNumber++;
	fprintf(OutFile, "%04x : 8fc0; -- LIL PC, R0 0\n", lineNumber);
	while (lineNumber < 16383)
	{
		lineNumber++;
		fprintf(OutFile, "%04x : 0e01; -- JR A, 1\n", lineNumber);
	}
	fprintf(OutFile, "END;\n", lineNumber);

	fclose(OutFile);

	if (display != 0)
	{
		printf("Code appears to have assembled correctly. Note this assembler does not check whether registers exist or if numbers are too long etc.\nDone. Press enter to exit.");
		while (getchar() != '\n');
	}

}


void removePunctuation(char *p)		//function to strip formatting from strings
{
	char *src = p, *dst = p;

	while (*src != NULL)
	{
		if ((ispunct((unsigned char)*src)) && (*src != '-'))
		{
			/* Skip this character */
			src++;
		}
		else if (!((isxdigit((unsigned char)*src)) || (*src == 'x') || (*src == 'X') || (*src == '-')))
		{
			/* Skip this character */
			src++;
		}
		else if (src == dst)
		{
			/* Increment both pointers without copying */
			src++;
			dst++;
		}
		else
		{
			/* Copy character */
			*dst++ = *src++;
		}
	}

	*dst = 0;
}