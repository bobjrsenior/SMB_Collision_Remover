#include <stdio.h>
#include <stdint.h>

uint32_t readBigInt(FILE *file) {
	uint32_t c1 = getc(file) << 24;
	uint32_t c2 = getc(file) << 16;
	uint32_t c3 = getc(file) << 8;
	uint32_t c4 = getc(file);
	return (c1 | c2 | c3 | c4);
}

void writeBigInt(FILE *file, uint32_t value) {
	putc((value >> 24), file);
	putc((value >> 16), file);
	putc((value >> 8), file);
	putc((value), file);
}

int removeCollision(char *filename) {
	FILE *rawFile = fopen(filename, "rb+");
	if (rawFile == NULL) {
		puts("File not found");
		return -1;
	}

	// Seek to collision field values
	fseek(rawFile, 8, SEEK_SET);
	uint32_t numCollisionFields = readBigInt(rawFile);
	uint32_t collisionFieldOffset = readBigInt(rawFile);

	// Seek to first collision field
	fseek(rawFile, collisionFieldOffset, SEEK_SET);
	for (uint32_t i = 0; i < numCollisionFields; i++) {
		uint32_t collisionFieldStart = ftell(rawFile);

		// Seek to triangle data and collision grid pointers
		fseek(rawFile, 32, SEEK_CUR);
		uint32_t collisionGridOffset = readBigInt(rawFile);
		fseek(rawFile, 16, SEEK_CUR); // Skip unneeded data
		uint32_t gridXStepCount = readBigInt(rawFile);
		uint32_t gridZStepCount = readBigInt(rawFile);
		uint32_t numGrids = gridXStepCount * gridZStepCount;


		// Seek to the grids and zero them out
		fseek(rawFile, collisionGridOffset, SEEK_SET);
		for (uint32_t grid = 0; grid < numGrids; grid++) {
			writeBigInt(rawFile, 0); // Trignle list pointer
		}

		// Seek to next collision grid
		fseek(rawFile, collisionFieldStart + 196, SEEK_SET);
	}

	fclose(rawFile);

	return 0;
}

int main(int argc, char *argv[]) {
	if (argc <= 1) {
		printf("Add lz paths as command line params");
	}


	// Go through every command line arg
	for (int i = 1; i < argc; ++i) {
		if (!removeCollision(argv[i])) {
			printf("Failed to remove collision for: %s\n", argv[i]);
		}
	}
	return 0;
}