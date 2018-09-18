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
		L"Adventure Commander Help: \n"
		L"[--load] Load a file from ~/.rlg327/dungeon\n"
		L"[--save] Write a dungeon to ~/.rlg327/dungeon\n"
	);
}

FILE* get(char* mode) {
	char path[256];
	char* home = getenv("HOME");
	snprintf(path, 255, "%s/.rlg327/dungeon", home);

	//Load dungeon from file
	FILE* file = fopen(path, mode);
	if (file == NULL) {
		wprintf(L"Failed to open file '%s'\n", path);
		exit(FILE_READ_GONE);
	}

	return file;
}

int main(int argc, char** argv) {
	setlocale(LC_ALL, "");
	srand(time(NULL));
	
	int save = 0;
	int load = 0;
	
	//Parse arguments
	for (int arg = 1; arg < argc; arg++) {
		if (strlen(argv[arg]) < 3 || argv[arg][0] != '-' || argv[arg][1] != '-') {
			//Check length
			wprintf(L"Bad argument '%s'\n", argv[arg]);
			help();
			exit(ARGUMENT_NO_DASH);
		} else {
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
		}
	}

	Dungeon dungeon;

	//Load dungeon from file
	if (load) {
		FILE* file = get("rb");
		dungeon = dungeonLoad(file);
		fclose(file);
	} else {
		dungeon = dungeonGenerate(DUNGEON_DIM);
	}
	
	dungeonPrint(dungeon);
	
	//Save dungeon to file.
	if (save) {
		FILE* file = get("wb");
		dungeonSave(dungeon, file);
		fclose(file);
	}
	
	dungeonDestroy(&dungeon);
}
