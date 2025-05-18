#include <stdio.h>

#include "liym/liym.h"
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
	if (argc > 2) {
		printf("Too many args\n");
		printf("Try: ./liyatoarray anim.liya\n");
		return -1;
	}
 
	if (argc < 2) {
		printf("Not enough args\n");
		printf("Try: ./liyatoarray anim.liym\n");
		return -1;
	}

	
	FILE *infile;
	FILE *outfile;
	struct liyaParseResults aparse = initLiyaStreamer(argv[1], &infile);

	if (aparse.bonecount == 0) {
		printf("Parse failed or anim has no bones\n");
		return -1;
	} else {
		printf("Got %d prims back from liya parser\n", aparse.bonecount);
	}

	char *friendlyname = malloc(strlen(argv[1]) * sizeof(char));
	for(int i = 0; 1; i++) {
		friendlyname[i] = argv[1][i];
		if(friendlyname[i] == '\0') {
			break;
		}
		if(friendlyname[i] == '.') {
			friendlyname[i] = '\0';
			break;
		}
	}

	char quantname[99];
	strcpy(quantname, friendlyname);
	strcat(quantname, "_liya.h");
	outfile = fopen(quantname, "w"); //open as w and close to empty the file
	fclose(outfile);

	outfile = fopen(quantname, "a+");

	fprintf(outfile, "int ");
	fprintf(outfile, "%s", friendlyname);
	fprintf(outfile, "primcount = %d;\n", aparse.bonecount);

	fprintf(outfile, "int ");
	fprintf(outfile, "%s", friendlyname);
	fprintf(outfile, "framecount = %d;\n", aparse.framecount);

	fprintf(outfile, "f32 ");
	fprintf(outfile, "%s", friendlyname);
	fprintf(outfile, "Anim[] = {");

	for(int i = 0; i < aparse.framecount; i++) {
		aparse = liyaStreamframe(aparse, infile);
		for(int j = 0; j < aparse.bonecount; j++) {
			fprintf(outfile, "%ff, ", aparse.framedata[j]);
		}
	}

	fseeko(outfile, -2, SEEK_END);
	int trimpos = ftello(outfile);
	ftruncate(fileno(outfile), trimpos);
	fprintf(outfile, "};\n");
	fclose(outfile);

	freeLiya(aparse, infile);
}
