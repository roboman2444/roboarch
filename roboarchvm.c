#include <stdio.h>
#include <stdlib.h>
#include <string.h> // for memcpy which is essential for copy

#define TRUE 1
#define FALSE 0


unsigned int memsize = 65536;
unsigned char * mem = 0;




int inputap = 0;
int inputbp = 0;
int outputp = 0;

//gets the pointers
void getp(const int op, const int a, const int b, const int c){
	if(op & 1) inputap = ((int *)mem)[a];
	else inputap = a;
	if(op & 2) inputbp = ((int *)mem)[b];
	else inputbp = b;
	if(op & 4) outputp = ((int *)mem)[c];
	else outputp = c;
}
void doop(const int op){
	switch((unsigned int) op >> 3){
		case 0://copy
			memcpy(((int*)mem) + outputp, ((int*)mem) + inputap, ((int *)mem)[inputbp]*4);
		break;
		case 1:	//add
			((int*)mem)[outputp] = ((int*)mem)[inputap] + ((int*)mem)[inputbp];
		break;
		case 2:	//sub
			((int*)mem)[outputp] = ((int*)mem)[inputap] - ((int*)mem)[inputbp];
		break;
		case 3:	//mult
			((int*)mem)[outputp] = ((int*)mem)[inputap] * ((int*)mem)[inputbp];
		break;
		case 4:	//div
			((int*)mem)[outputp] = ((int*)mem)[inputap] / ((int*)mem)[inputbp];
		break;
		case 5:	//mod
			((int*)mem)[outputp] = ((int*)mem)[inputap] % ((int*)mem)[inputbp];
		break;
		case 6:	//and
			((int*)mem)[outputp] = ((int*)mem)[inputap] & ((int*)mem)[inputbp];
		break;
		case 7:	//or
			((int*)mem)[outputp] = ((int*)mem)[inputap] | ((int*)mem)[inputbp];
		break;
		case 8:	//xor
			((int*)mem)[outputp] = ((int*)mem)[inputap] ^ ((int*)mem)[inputbp];
		break;
		case 9:	//not
			((int*)mem)[outputp] = ~((int*)mem)[inputap];
		break;
		case 10://bnot
			((int*)mem)[outputp] = !((int*)mem)[inputap];
		break;
		case 11://sl
			((int*)mem)[outputp] = ((int*)mem)[inputap] << ((int*)mem)[inputbp];
		break;
		case 12://sr
			((int*)mem)[outputp] = ((unsigned int*)mem)[inputap] >> ((int*)mem)[inputbp]; //the unsigned tricks c into logical shift?
		break;
		case 13://sa
			((int*)mem)[outputp] = ((int*)mem)[inputap] >> ((int*)mem)[inputbp];
		break;
		case 14://tg
			((int*)mem)[outputp] = (((int*)mem)[inputap] > ((int*)mem)[inputbp]);
		break;
		case 15://te
			((int*)mem)[outputp] = (((int*)mem)[inputap] == ((int*)mem)[inputbp]);
		break;
		default: break;//noop
	}
}

void executionloop(void){
	int ip;
	for(;(ip = ((int*)mem)[0])*4; ((int*)mem)[0]++){
		int op = ((int*)mem)[ip];
		int a = ((int*)mem)[ip+1];
		int b = ((int*)mem)[ip+2];
		int c = ((int*)mem)[ip+3];
		getp(op, a, b, c);
		doop(op);
	}
}
int main(int argc, char ** argv){
	//grab input file
	char * filename = argc > 1 ? argv[1] : "./program.bin";
	FILE *f = fopen(filename, "rb");
	if(!f){
		printf("File %s not found\n", filename);
		return TRUE;
	}
	fseek(f, 0, SEEK_END);
	int filesize = ftell(f);
	rewind(f);
	memsize = (filesize + memsize-1) & ~(memsize -1);
	if(argc > 2){
		int presize = atoi(argv[2]);
		if(presize < filesize) printf("Memsize %i too small for a %i file, setting to %i\n", presize, filesize, memsize);
		else memsize = presize;
	}
	mem = malloc(memsize);
	fread(mem, 1, filesize, f);
	if(f)fclose(f); f = 0;
	memset(mem + filesize, 0, memsize - filesize);
	executionloop();
	//dump output
	if(argc > 3) f = fopen(argv[3], "wb");
	if(f){
		//find end of mem
		int i;
		for(i = memsize -1; i >= 0 && !mem[i]; i--);
		i++;
		if(i){
			printf("dumping %i bytes to file %s\n", i, argv[3]);
			fwrite(mem, i, 1, f);
		} else printf("no bytes to write\n");
	}
	if(mem) free(mem); mem = 0;
	return FALSE;
}
