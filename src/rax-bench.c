
#include <stdio.h>
#include "rax.h"
#include <stdlib.h>
// #include "server.h"
long long ustime(void);
long long mstime(void);

#define CYCLES (6*10*1000*1000)
#define KEYLEN 32
#define LOG_CENTILES_START 2

void bench() {
	rax *tree = raxNew();

	long long starttime;
	long long endtime;
	unsigned long long log_centiles[100] = {0};
	unsigned long long log_centiles_avg[100] = {0};
	int log_centiles_index = 0;
	unsigned long long log_centiles_modulus = LOG_CENTILES_START;
	long long prev_index = 0;

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
	long long read_time_ns = (endtime - starttime) * 1000;
	long long read_time_avg_ns = read_time_ns / CYCLES / 2;
	
	
	
	starttime = ustime();
	for (long long i=0; i<CYCLES; i++) {
		if (i % log_centiles_modulus == 0) {
			endtime = ustime();
			log_centiles[log_centiles_index] = (endtime - starttime) * 1000;

			log_centiles_avg[log_centiles_index] = (i - prev_index);
			prev_index = i;
			
			starttime = endtime;
			log_centiles_index++;
			log_centiles_modulus = log_centiles_modulus * 2;
		}
		raxInsert(tree, keys + (i * KEYLEN), KEYLEN, NULL, NULL);
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
	printf("avg time*1000 us%lld\r\n", (endtime - starttime) * 1000 / CYCLES);
	long long run1n = (endtime - starttime);
	long long rax_insert_1n_ns = (endtime - starttime) * 1000;
	long long rax_insert_1n_avg_ns = rax_insert_1n_ns / CYCLES;

	printf("log_centiles\r\n");
	log_centiles_index = 0;
	log_centiles_modulus = LOG_CENTILES_START;
	for (int i = 0; i<100; i++) {
		printf("%llu,", (i + 1) * log_centiles_modulus);
		log_centiles_modulus = log_centiles_modulus * 2;
	}
	printf("\r\n");
	for (int i = 0; i<100; i++) {
		printf("%llu,", log_centiles[i]);
	}
	printf("\r\n");
	for (int i = 0; i<100; i++) {
		printf("%llu,", log_centiles_avg[i]);
	}

	printf("\r\nlog_centiles_done\r\n");




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
	long long rax_insert_2n_ns = (endtime - starttime) * 1000;
	long long rax_insert_2n_avg_ns = rax_insert_2n_ns / CYCLES / 2;

	printf("normalized run time us 2N - N %lld\r\n", run2n - run1n);
	printf("normalized avg run time*1000 us 2N - N / N %lld\r\n", (run2n - run1n)*1000/CYCLES);
	long long rax_insert_delta_ns = rax_insert_2n_ns - rax_insert_1n_ns;
	long long rax_insert_delta_avg_ns = rax_insert_delta_ns / CYCLES;
	
	
	
	starttime = ustime();
	for (long long i=0; i<CYCLES; i++) {
		raxFind(tree, keys + (i * KEYLEN), KEYLEN);
	}
	endtime = ustime();
	printf("\r\n1N rax tree lookup run\r\n");
	printf("rax tree nodes %llu\r\n", tree->numnodes);
	printf("rax size %llu\r\n", raxSize(tree));
	printf("start time %lld\r\n", starttime);
	printf("end time %lld\r\n", endtime);
	printf("delta time us %lld\r\n", endtime - starttime);
	printf("delta time ms %lld\r\n", (endtime - starttime)/1000);
	printf("avg time*1000 us%lld\r\n", (endtime - starttime) * 1000 / CYCLES);
	run1n = (endtime - starttime);
	long long rax_lookup_1n_ns = (endtime - starttime) * 1000;
	long long rax_lookup_1n_avg_ns = rax_lookup_1n_ns / CYCLES;




	starttime = ustime();
	for (long long i=0; i<CYCLES; i++) {
		raxFind(tree2, keys + (i * KEYLEN), KEYLEN);
	}
	
	for (long long i=0; i<CYCLES; i++) {
		raxFind(tree3, keys + (CYCLES + i * KEYLEN), KEYLEN);
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
	run2n = (endtime - starttime);
	long long rax_lookup_2n_ns = (endtime - starttime) * 1000;
	long long rax_lookup_2n_avg_ns = rax_lookup_2n_ns / CYCLES / 2;

	printf("normalized run time us 2N - N %lld\r\n", run2n - run1n);
	printf("normalized avg run time*1000 us 2N - N / N %lld\r\n", (run2n - run1n)*1000/CYCLES);
	long long rax_lookup_delta_ns = rax_lookup_2n_ns - rax_lookup_1n_ns;
	long long rax_lookup_delta_avg_ns = rax_lookup_delta_ns / CYCLES;
	
	
	
	starttime = ustime();
	for (long long i=0; i<CYCLES; i++) {
		raxRemove(tree, keys + (i * KEYLEN), KEYLEN, NULL);
	}
	endtime = ustime();
	printf("\r\n1N rax tree delete run\r\n");
	printf("rax tree nodes %llu\r\n", tree->numnodes);
	printf("rax size %llu\r\n", raxSize(tree));
	printf("start time %lld\r\n", starttime);
	printf("end time %lld\r\n", endtime);
	printf("delta time us %lld\r\n", endtime - starttime);
	printf("delta time ms %lld\r\n", (endtime - starttime)/1000);
	printf("avg time*1000 us%lld\r\n", (endtime - starttime) * 1000 / CYCLES);
	run1n = (endtime - starttime);
	long long rax_delete_1n_ns = (endtime - starttime) * 1000;
	long long rax_delete_1n_avg_ns = rax_delete_1n_ns / CYCLES;




	starttime = ustime();
	for (long long i=0; i<CYCLES; i++) {
		raxRemove(tree2, keys + (i * KEYLEN), KEYLEN, NULL);
	}
	
	for (long long i=0; i<CYCLES; i++) {
		raxRemove(tree3, keys + (CYCLES + i * KEYLEN), KEYLEN, NULL);
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
	run2n = (endtime - starttime);
	long long rax_delete_2n_ns = (endtime - starttime) * 1000;
	long long rax_delete_2n_avg_ns = rax_delete_2n_ns / CYCLES / 2;

	printf("normalized run time us 2N - N %lld\r\n", run2n - run1n);
	printf("normalized avg run time*1000 us 2N - N / N %lld\r\n", (run2n - run1n)*1000/CYCLES);
	long long rax_delete_delta_ns = rax_delete_2n_ns - rax_delete_1n_ns;
	long long rax_delete_delta_avg_ns = rax_delete_delta_ns / CYCLES;

	printf("type,keylen,cycles,key read time ns,key read time avg ns,rax tree op ns,rax tree op avg ns,2N rax tree op ns,2N rax tree op avg ns,2N - N ns,(2N - N)/N ns\r\n");
	printf("%s,%d,%d,", "raxInsert", KEYLEN, CYCLES);
	printf("%lld,%lld,", read_time_ns, read_time_avg_ns);
	printf("%lld,%lld,", rax_insert_1n_ns, rax_insert_1n_avg_ns);
	printf("%lld,%lld,", rax_insert_2n_ns, rax_insert_2n_avg_ns);
	printf("%lld,%lld,", rax_insert_delta_ns, rax_insert_delta_avg_ns);
	printf("\r\n");	
	printf("%s,%d,%d,", "raxFind", KEYLEN, CYCLES);
	printf("%lld,%lld,", read_time_ns, read_time_avg_ns);
	printf("%lld,%lld,", rax_lookup_1n_ns, rax_lookup_1n_avg_ns);
	printf("%lld,%lld,", rax_lookup_2n_ns, rax_lookup_2n_avg_ns);
	printf("%lld,%lld,", rax_lookup_delta_ns, rax_lookup_delta_avg_ns);
	printf("\r\n");
	printf("%s,%d,%d,", "raxRemove", KEYLEN, CYCLES);
	printf("%lld,%lld,", read_time_ns, read_time_avg_ns);
	printf("%lld,%lld,", rax_delete_1n_ns, rax_delete_1n_avg_ns);
	printf("%lld,%lld,", rax_delete_2n_ns, rax_delete_2n_avg_ns);
	printf("%lld,%lld,", rax_delete_delta_ns, rax_delete_delta_avg_ns);
	printf("\r\n");
}

int main() {
	srand(123145151);
	printf("benchmark start\r\n");
	bench();
	printf("benchmark end\r\n");
}
