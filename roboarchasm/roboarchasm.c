#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "stringlib.h"

#define TRUE 1
#define FALSE 0

int curplace = 0;
int *data = 0;
int datasize = 0;
int maxput = 0;


void putdata(int in, int place){
	if(place >= datasize){
		datasize = place +16;
		data = realloc(data, datasize * sizeof(int));
	}
	if(place > maxput) maxput = place;
	data[place] = in;
}

const char *ops[] = {"copy", "add", "sub", "mult", "div", "mod", "and", "or", "xor", "not", "bnot", "sl", "sr", "sa", "tg", "te", 0};
//if it isnt a valid op, returns false
int writeop(char * word){
	int opcode;
	for(opcode = 0; ops[opcode]; opcode++) if(string_testEqualWFCI(ops[opcode], word)) break;
	if(!ops[opcode]) return FALSE;
	opcode = opcode << 3;
	int bmp = strlen(ops[opcode]);
	int i;
	for(i = 0; i < 3 && !ISWHITESPACE(word[bmp + i]); i++){
		if(ISUPPER(word[bmp+i])) opcode = opcode | 1<<i;		//will change later
	}
	putdata(opcode, curplace);
	curplace++;
	return bmp+i;
}
char * tword = 0;
unsigned int twordlen = 0;
int parseword(char * word){
	int sz = string_wordLength(word);
	if(sz >= twordlen){
		twordlen = sz+1;
		if(tword) free(tword);
		tword = malloc(twordlen);
	}
	int i;
	for(i = 0; i < sz; i++)tword[i] = word[i];
	tword[sz] = 0;
	int writeres = writeop(word);
	if(writeres){
		printf("parsed %s as the op %i or 0x%08x, loc %i\n", tword, data[curplace-1], data[curplace-1], curplace-1);
		return writeres;
	}
	errno = 1;
	if(string_testEqualN(word,"0x", 2)){ //hex value
		writeres = strtol(word, NULL, 0);
	}
/*	if(errno){
		errno = 0;
		writeres = strtof(word, NULL) + 1.0;
	}*/
	if(errno){ //int?
		errno = 0;
		writeres = strtol(word, NULL, 10);
	}

	if(errno){
		printf("error parsing %s\n", tword);
		writeres = 0;
	}
	else printf("parsed %s as %i or 0x%08x, loc %i\n", tword, writeres, writeres, curplace);
	putdata(writeres, curplace);
	curplace++;

	return sz;
}

char * indata = 0;
size_t insize = 0;
int main(int argc, char ** argv){
	FILE *f = fopen(argv[1], "r");
	fseek(f, 0, SEEK_END);
	insize = ftell(f);
	rewind(f);
	indata = malloc(insize);
	fread(indata, 1, insize, f);
	fclose(f);
	char * curln = indata;
	while(*curln){
		curln += parseword(curln);
		//advance to next word
		for(;!ISWHITESPACE(*curln); curln++);
		for(;*curln && ISWHITESPACE(*curln); curln++);
	}

	f = fopen(argv[2], "wb");
	fwrite(data, (maxput+1) * sizeof(int), 1, f);




	if(indata) free(indata);
	if(data)free(data);
	if(tword)free(tword);
	return FALSE;
}
