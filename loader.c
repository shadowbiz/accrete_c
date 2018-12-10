#include "loader.h"

char *LoadTextFile(const char *filename)
{
	char *text = 0;
	int file_length = 0;
	FILE *file = fopen(filename, "r");

	if (file)
	{
		fseek(file, 0, SEEK_END);
		file_length = ftell(file);
		text = (char *)malloc(sizeof(char) * (file_length + 1));
		memset(text, 0, file_length + 1);
		rewind(file);
		fread(text, sizeof(char), file_length, file);
		fclose(file);
	}
	else
	{
		printf("Error loading %s file\n", filename);
		free(text);
		exit(EXIT_FAILURE);
		return NULL;
	}
	return text;
}
