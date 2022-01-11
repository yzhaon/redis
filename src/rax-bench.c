
#include <stdio.h>
#include "rax.h"
#include <stdlib.h>
// #include "server.h"
long long ustime(void);
long long mstime(void);

#define CYCLES (6*10*1000*1000)
#define KEYLEN 32

void bench() {
	rax *tree = raxNew();

	long long starttime;
	long long endtime;

	unsigned char *keys = malloc(sizeof(unsigned char) * KEYLEN * CYCLES * 2 );
	starttime = ustime();
	for (long long i=0; i<CYCLES * 2; i++) {
		for (long long j=0; j<KEYLEN; j++) {
			unsigned char randc = rand() % 256;
			keys[i * KEYLEN + j] = randc;
		}
	}
	endtime = ustime();
	printf("\r\nrandom key gen time \r\n");
	printf("start time %lld\r\n", starttime);
	printf("end time %lld\r\n", endtime);
	printf("delta time us %lld\r\n", endtime - starttime);
	printf("delta time ms %lld\r\n", (endtime - starttime)/1000);
	printf("avg time*1000 us%lld\r\n", (endtime - starttime)*1000 / CYCLES);


	starttime = ustime();
	int counter1 = 0;
	int counter2 = 0;
	for (long long i=0; i<CYCLES * 2; i++) {
		if (keys[i * KEYLEN + 0] == ' ') counter1++;
		if (keys[i * KEYLEN + 0] != ' ') counter2++;
	}
	endtime = ustime();
	printf("\r\nrandom key read/load time \r\n");
	printf("start time %lld\r\n", starttime);
	printf("end time %lld\r\n", endtime);
	printf("delta time us %lld\r\n", endtime - starttime);
	printf("delta time ms %lld\r\n", (endtime - starttime)/1000);
	printf("avg time*1000 us%lld\r\n", (endtime - starttime)*1000 / CYCLES);
	printf("counters %d %d \r\n", counter1, counter2);
	
	
	
	starttime = ustime();
	for (long long i=0; i<CYCLES; i++) {
		raxInsert(tree, keys + (i * KEYLEN), KEYLEN, NULL, NULL);
	}
	endtime = ustime();
	printf("\r\n1N rax tree run\r\n");
	printf("rax tree nodes %llu\r\n", tree->numnodes);
	printf("rax size %llu\r\n", raxSize(tree));
	printf("start time %lld\r\n", starttime);
	printf("end time %lld\r\n", endtime);
	printf("delta time us %lld\r\n", endtime - starttime);
	printf("delta time ms %lld\r\n", (endtime - starttime)/1000);
	printf("avg time*1000 us%lld\r\n", (endtime - starttime) * 1000 / CYCLES);
	long long run1n = (endtime - starttime);




	rax *tree2 = raxNew();
	rax *tree3 = raxNew();
	starttime = ustime();
	for (long long i=0; i<CYCLES; i++) {
		raxInsert(tree2, keys + (i * KEYLEN), KEYLEN, NULL, NULL);
	}
	
	for (long long i=0; i<CYCLES; i++) {
		raxInsert(tree3, keys + (CYCLES + i * KEYLEN), KEYLEN, NULL, NULL);
	}
	
	endtime = ustime();
	printf("\r\n2N rax tree run\r\n");
	printf("rax tree nodes %llu %llu\r\n", tree2->numnodes, tree3->numnodes);
	printf("rax size %llu %llu\r\n", raxSize(tree2), raxSize(tree3));
	printf("start time %lld\r\n", starttime);
	printf("end time %lld\r\n", endtime);
	printf("delta time us %lld\r\n", endtime - starttime);
	printf("delta time ms %lld\r\n", (endtime - starttime)/1000);
	printf("avg time*1000 us%lld\r\n", (endtime - starttime)*1000 / CYCLES);
	long long run2n = (endtime - starttime);

	printf("normalized run time us 2N - N %lld\r\n", run2n - run1n);
	printf("normalized avg run time*1000 us 2N - N / N %lld\r\n", (run2n - run1n)*1000/CYCLES);
}

int main() {
	srand(123145151);
	printf("benchmark start\r\n");
	bench();
	printf("benchmark end\r\n");
}
