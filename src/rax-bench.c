
#include <stdio.h>
#include "rax.h"
#include <stdlib.h>
// #include "server.h"
long long ustime(void);
long long mstime(void);

#define KEYLEN 32

void bench() {
	rax *tree = raxNew();

	int cycles = 10*1000*1000;

	long long starttime = ustime();
	for (int i=0; i<cycles; i++) {
		char key[KEYLEN];
		for (int j=0; j<KEYLEN; j++) {
			char randc = rand() % 256;
			key[j] = randc;
		}
		raxInsert(tree, key, KEYLEN, NULL, NULL);
	}
	long long endtime = ustime();
	printf("rax tree nodes %llu\r\n", tree->numnodes);
	printf("rax size %llu\r\n", raxSize(tree));
	printf("start time %lld\r\n", starttime);
	printf("end time %lld\r\n", endtime);
	printf("delta time us %lld\r\n", endtime - starttime);
	printf("delta time ms %lld\r\n", (endtime - starttime)/1000);



	starttime = ustime();
	for (int i=0; i<cycles; i++) {
		char key[KEYLEN];
		for (int j=0; j<KEYLEN; j++) {
			char randc = rand() % 256;
			key[j] = randc;
		}
	}
	endtime = ustime();
	printf("rax tree nodes %llu\r\n", tree->numnodes);
	printf("rax size %llu\r\n", raxSize(tree));
	printf("start time %lld\r\n", starttime);
	printf("end time %lld\r\n", endtime);
	printf("delta time us %lld\r\n", endtime - starttime);
	printf("delta time ms %lld\r\n", (endtime - starttime)/1000);
}

int main() {
	srand(123145151);
	printf("benchmark start\r\n");
	bench();
	printf("benchmark end\r\n");
}