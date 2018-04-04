//Do emulatora został dołączony przykładowy program (test.program), który powinien wypiwać "3" - sprawdza on wszystkie dostępne rozkazy procesora

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

int main(int argc, char *argv[])
{

	if (argc != 2)
	{
		printf("usage: %s <nazwa pliku z programem>\n", argv[0]);
		exit(1);
	}

	char *zero = {0};
	int lines[2];
	bool firstLine = true;
	FILE *sourceFile;
	FILE *resultFile;
	char *line = NULL;
	size_t len = 0;
	char *resultFileWithExtension;
	const char *extension = ".bin";

	char *fileName = argv[1];
	char *fileNameBackup = malloc(2048 * sizeof(char));
	strcpy(fileNameBackup, fileName);

	char *fileNameWithoutExtension;

	//Wczytujemy plik
	sourceFile = fopen(fileName, "r");
	if (sourceFile == NULL)
	{
		printf("Plik nie istnieje\n");
		exit(EXIT_FAILURE);
	}

	//Znajdujemy nazwę pliku bez rozszerzenia
	fileNameWithoutExtension = strtok(fileName, ".");

	resultFileWithExtension = malloc(strlen(fileNameWithoutExtension) + 1 + 4);
	strcpy(resultFileWithExtension, fileNameWithoutExtension);
	strcat(resultFileWithExtension, extension);

	fclose(fopen(resultFileWithExtension, "a"));

	resultFile = fopen(resultFileWithExtension, "wb");


	char **command = malloc(3 * sizeof(char *));

	putc(255, resultFile);
	putc(255, resultFile);

	//Szukanie pierwszego i ostatniego wiersza
	while (getline(&line, &len, sourceFile) != -1)
	{
		char *temp;

		int p = 0;

		temp = strtok(line, " ");
		while (temp != NULL)
		{
			command[p] = temp;
			p++;
			temp = strtok(NULL, " ");
		}

		if (strcmp(command[0], "\n") == 0)
		{
			continue;
		}

		if (firstLine)
		{
			lines[0] = (int) strtol(command[0], NULL, 16);
			firstLine = false;
		}
		lines[1] = (int) strtol(command[0], NULL, 16);
	}

	printf("%d\n%d", lines[0], lines[1]);

	int i = 0;

	putc(lines[i] % 256, resultFile);
	putc(lines[i] / 256, resultFile);

	i = 1;
	putc(lines[i] % 256, resultFile);
	putc(lines[i] / 256, resultFile);

	fclose(sourceFile);
	if (line)
	{
		free(line);
	}
	*line = 0;
	len = 0;
	sourceFile = fopen(fileNameBackup, "r");
	if (sourceFile == NULL)
	{
		printf("Plik nie istnieje\n");
		exit(EXIT_FAILURE);
	}

	int lastID = -1;


	while (getline(&line, &len, sourceFile) != -1)
	{

		char *temp;

		int p = 0;

		temp = strtok(line, " ");
		while (temp != NULL)
		{
			command[p] = temp;
			p++;
			temp = strtok(NULL, " ");
		}

		if (p == 2)
		{
			command[2] = 0;
		}

		//PUSTA LINIA
		if (strcmp(command[0], "\n") == 0)
		{
			continue;
		}


		int currentID = (int) strtol(command[0], NULL, 16);

		//Jeśli jest jakaś przerwa w programie
		while (lastID != currentID - 1 && lastID != -1)
		{
			putc(0, resultFile);
			putc(0, resultFile);
			lastID++;
		}

		lastID = currentID;

		//UCODE

		int opcode;

		if (strcmp(command[1], "INS") == 0)
		{
			opcode = 0;
		}
		else if (strcmp(command[1], "LOAD") == 0)
		{
			opcode = 1;
		}
		else if (strcmp(command[1], "STORE") == 0)
		{
			opcode = 2;
		}
		else if (strcmp(command[1], "ADD") == 0)
		{
			opcode = 3;
		}
		else if (strcmp(command[1], "SUBT") == 0)
		{
			opcode = 4;
		}
		else if (strcmp(command[1], "INPUT") == 0 || strcmp(command[1], "INPUT\n") == 0)
		{
			opcode = 5;
		}
		else if (strcmp(command[1], "OUTPUT") == 0 || strcmp(command[1], "OUTPUT\n") == 0)
		{
			opcode = 6;
		}
		else if (strcmp(command[1], "HALT") == 0 || strcmp(command[1], "HALT\n") == 0)
		{
			opcode = 7;
		}
		else if (strcmp(command[1], "SKIPCOND") == 0)
		{
			opcode = 8;
		}
		else if (strcmp(command[1], "JUMP") == 0)
		{
			opcode = 9;
		}
		else if (strcmp(command[1], "CLEAR") == 0 || strcmp(command[1], "CLEAR\n") == 0)
		{
			opcode = 10;
		}
		else if (strcmp(command[1], "ADDI") == 0)
		{
			opcode = 11;
		}
		else if (strcmp(command[1], "JUMPI") == 0)
		{
			opcode = 12;
		}
		else if (strcmp(command[1], "HEX") == 0)
		{
			// Trochę inaczej się to obsługuje, więc zrobimy to w tym ifie
			int number = (int) strtol(command[2], NULL, 16);

			int backup = number;

			putc(backup & 0xFF, resultFile);
			putc((number >> 8) & 0xFF, resultFile);

			continue;

		}
		else if (strcmp(command[1], "DEC") == 0)
		{
			// Trochę inaczej się to obsługuje, więc zrobimy to w tym ifie
			int number = (int) strtol(command[2], NULL, 10);

			int backup = number;

			putc(backup & 0xFF, resultFile);
			putc((number >> 8) & 0xFF, resultFile);

			continue;

		}
		else /* default: */
		{
			printf("UNKNOWN OPCODE: %s\n", command[1]);
			exit(1);
		}

		if (command[2] == '\0')
		{
			command[2] = zero;
		}

		int number = 0;

		if (command[2])
		{
			number = (int) strtol(command[2], NULL, 16);
		}

		putc(number % 256, resultFile);
		putc(opcode * 16 + number / 256, resultFile);

	}

	fclose(sourceFile);
	if (line)
	{
		free(line);
	}
	exit(0);
}
