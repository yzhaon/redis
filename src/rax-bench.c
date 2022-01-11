
#include <stdio.h>
#include "rax.h"
#include <stdlib.h>
// #include "server.h"
long long ustime(void);
long long mstime(void);

#define KEYLEN 32

void bench() {
	rax *tree = raxNew();

	int cycles = 6*10*1000*1000;
	long long starttime;
	long long endtime;

	char **keys = malloc(sizeof(char *) * cycles * 2 );
	starttime = ustime();
	for (int i=0; i<cycles * 2; i++) {
		keys[i] = malloc(sizeof(char) * KEYLEN);
		for (int j=0; j<KEYLEN; j++) {
			char randc = rand() % 256;
			keys[i][j] = randc;
		}
	}
	endtime = ustime();
	printf("\r\nrandom key gen time \r\n");
	printf("start time %lld\r\n", starttime);
	printf("end time %lld\r\n", endtime);
	printf("delta time us %lld\r\n", endtime - starttime);
	printf("delta time ms %lld\r\n", (endtime - starttime)/1000);
	printf("avg time*1000 us%llu\r\n", (endtime - starttime)*1000 / cycles);


	starttime = ustime();
	int counter1 = 0;
	int counter2 = 0;
	for (int i=0; i<cycles * 2; i++) {
		if (keys[i][0] == ' ') counter1++;
		if (keys[i][0] != ' ') counter2++;
	}
	endtime = ustime();
	printf("\r\nrandom key read/load time \r\n");
	printf("start time %lld\r\n", starttime);
	printf("end time %lld\r\n", endtime);
	printf("delta time us %lld\r\n", endtime - starttime);
	printf("delta time ms %lld\r\n", (endtime - starttime)/1000);
	printf("avg time*1000 us%llu\r\n", (endtime - starttime)*1000 / cycles);
	printf("counters %d %d \r\n", counter1, counter2);
	
	
	
	starttime = ustime();
	for (int i=0; i<cycles; i++) {
		raxInsert(tree, keys[i], KEYLEN, NULL, NULL);
	}
	endtime = ustime();
	printf("\r\n1N rax tree run\r\n");
	printf("rax tree nodes %llu\r\n", tree->numnodes);
	printf("rax size %llu\r\n", raxSize(tree));
	printf("rax alloc %llu\r\n", raxAllocSize(tree));
	printf("start time %lld\r\n", starttime);
	printf("end time %lld\r\n", endtime);
	printf("delta time us %lld\r\n", endtime - starttime);
	printf("delta time ms %lld\r\n", (endtime - starttime)/1000);
	printf("avg time*1000 us%llu\r\n", (endtime - starttime) * 1000 / cycles);
	long long run1n = (endtime - starttime);




	rax *tree2 = raxNew();
	rax *tree3 = raxNew();
	starttime = ustime();
	for (int i=0; i<cycles; i++) {
		raxInsert(tree2, keys[i], KEYLEN, NULL, NULL);
	}
	
	for (int i=0; i<cycles; i++) {
		raxInsert(tree3, keys[i + cycles], KEYLEN, NULL, NULL);
	}
	
	endtime = ustime();
	printf("\r\n2N rax tree run\r\n");
	printf("rax tree nodes %llu %llu\r\n", tree2->numnodes, tree3->numnodes);
	printf("rax size %llu %llu\r\n", raxSize(tree2), raxSize(tree3));
	printf("start time %lld\r\n", starttime);
	printf("end time %lld\r\n", endtime);
	printf("delta time us %lld\r\n", endtime - starttime);
	printf("delta time ms %lld\r\n", (endtime - starttime)/1000);
	printf("avg time*1000 us%llu\r\n", (endtime - starttime)*1000 / cycles);
	long long run2n = (endtime - starttime);

	printf("normalized run time us 2N - N %llu\r\n", run2n - run1n);
	printf("normalized avg run time*1000 us 2N - N / N %llu\r\n", (run2n - run1n)*1000/cycles);
}

int main() {
	srand(123145151);
	printf("benchmark start\r\n");
	bench();
	printf("benchmark end\r\n");
}
