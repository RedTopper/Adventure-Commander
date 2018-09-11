#include <wchar.h>
#include <locale.h>
#include <time.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "types.h"
#include "dungeon.h"


const char* SAVE = "--save";
const char* LOAD = "--load";

void help() {
	wprintf(
		L"TODO: Help\n"
		L"Second line\n"
	);
}

int main(int argc, char** argv) {
	setlocale(LC_ALL, "");
	srand(time(NULL));
	
	int save = 0;
	int load = 0;
	
	//Parse arguments
	for (int arg = 1; arg < argc; arg++) {
		if (strlen(argv[arg]) < 3) {
			//Check length
			wprintf(L"Unknown argument '%s'\n");
			help();
			exit(ARGUMENT_BAD);
		} else if (argv[arg][0] == '-' && argv[arg][1] == '-') {
			//Check if correct argument
			if (strcmp(SAVE, argv[arg]) == 0) {
				save = 1;
			} else if (strcmp(LOAD, argv[arg]) == 0) {
				load = 1;
			} else {
				wprintf(L"Unknown argument '%s'\n", argv[arg]);
				help();
				exit(ARGUMENT_NO_UNKNOWN);
			}
		} else {
			//Broken prefix
			wprintf(L"Arguments must start with --\n");
			help();
			exit(ARGUMENT_NO_DASH);
		}
	}
	
	char path[256];
	char* home = getenv("HOME");
	snprintf(path, 255, "%s/.rlg327/dungeon", home);
	Dungeon dungeon;
	
	//Load dungeon from file
	if (load) {
		FILE* file = fopen(path, "rb");
		
		if (file == NULL) {
			wprintf(L"Failed to open file '%s'\n", path);
			exit(FILE_READ_GONE);
		}
		
		dungeon = dungeonLoad(file);
		fclose(file);
	} else {
		dungeon = dungeonGenerate(DUNGEON_DIM);
	}
	
	dungeonPrint(dungeon);
	
	//Save dungeon to file.
	if (save) {
		FILE* file = fopen(path, "wb");
		
		if (file == NULL) {
			wprintf(L"Failed to write file '%s'\n", path);
			exit(FILE_READ_GONE);
		}
		
		dungeonSave(dungeon, file);
		fclose(file);
	}
	
	dungeonDestroy(dungeon);
}