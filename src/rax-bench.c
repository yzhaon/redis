
#include <stdio.h>
#include "rax.h"
#include <stdlib.h>
// #include "server.h"
#include <string.h>

long long ustime(void);
long long mstime(void);

#define CYCLES (6*10*1000*1000)
#define KEYLEN 32
#define LOG_CENTILES_START 2

typedef struct benchmark_data {
	long long starttime;
	long long endtime;
	unsigned long long log_centiles[100];
	unsigned long long log_centiles_avg[100];
	int log_centiles_index;
	unsigned long long log_centiles_modulus;
	long long prev_index;
} benchmark_data;

unsigned long long time_delta_ns(benchmark_data *data) {
	return (data->endtime - data->starttime) * 1000;
}
void print_run_data(benchmark_data *data) {
	// printf("start time %lld\r\n", data->starttime);
	// printf("end time %lld\r\n", data->endtime);
	printf("delta time us %lld\r\n", time_delta_ns(data) / 1000);
	printf("delta time ms %lld\r\n", time_delta_ns(data) / 1000 / 1000);
	printf("avg time ns %lld\r\n", time_delta_ns(data) / CYCLES);
}

void log_centiles_init(benchmark_data *data) {
	memset(data, 0, sizeof(benchmark_data));
	data->log_centiles_modulus = LOG_CENTILES_START;
}

void log_centiles_print(benchmark_data *data) {
	printf("log_centiles\r\n");
	data->log_centiles_index = 0;
	data->log_centiles_modulus = LOG_CENTILES_START;
	for (int i = 0; i<100; i++) {
		printf("%llu,", data->log_centiles[i]);
	}
	printf("\r\n");
	for (int i = 0; i<100; i++) {
		printf("%llu,", data->log_centiles_avg[i]);
	}

	printf("\r\nlog_centiles_done\r\n");
}

void log_centiles_update(benchmark_data *data, long long index) {
	data->endtime = ustime();
	data->log_centiles[data->log_centiles_index] = time_delta_ns(data);

	data->log_centiles_avg[data->log_centiles_index] = (index - data->prev_index);
	data->prev_index = index;
	
	data->starttime = data->endtime;
	data->log_centiles_index++;
	data->log_centiles_modulus = data->log_centiles_modulus * 2;
}

void print_tree_data(rax *tree, char *prefix) {
	printf("%s rax tree nodes %llu\r\n", prefix, tree->numnodes);
	printf("%s rax size %llu\r\n", prefix, raxSize(tree));
}



void bench() {
	rax *tree = raxNew();

	long long starttime;
	long long endtime;
	unsigned long long log_centiles[100] = {0};
	unsigned long long log_centiles_avg[100] = {0};
	int log_centiles_index = 0;
	unsigned long long log_centiles_modulus = LOG_CENTILES_START;
	long long prev_index = 0;

	benchmark_data data = {0};

	unsigned char *keys = malloc(sizeof(unsigned char) * KEYLEN * CYCLES * 2 );
	data.starttime = ustime();
	for (long long i=0; i<CYCLES * 2; i++) {
		for (long long j=0; j<KEYLEN; j++) {
			unsigned char randc = rand() % 256;
			keys[i * KEYLEN + j] = randc;
		}
	}
	data.endtime = ustime();
	printf("\r\nrandom key gen time \r\n");
	print_run_data(&data);


	data.starttime = ustime();
	int counter1 = 0;
	int counter2 = 0;
	for (long long i=0; i<CYCLES; i++) {
		if (keys[i * KEYLEN + 0] == ' ') counter1++;
		if (keys[i * KEYLEN + 0] != ' ') counter2++;
	}
	data.endtime = ustime();
	printf("\r\nrandom key read/load time \r\n");
	print_run_data(&data);
	long long read_time_ns = time_delta_ns(&data);
	long long read_time_avg_ns = read_time_ns / CYCLES;
	
	
	data.starttime = ustime();
	counter1 = 0;
	counter2 = 0;
	long long measure_time = data.starttime;
	for (long long i=0; i<CYCLES; i++) {
		long long delta = ustime() - measure_time;
		if (delta % 2 == 0) counter1++;
		if (delta % 2 == 1) counter2++;
	}
	data.endtime = ustime();
	printf("\r\ntimestamping time\r\n");
	print_run_data(&data);
	long long ts_time_ns = time_delta_ns(&data);
	long long ts_time_avg_ns = ts_time_ns / CYCLES;



	log_centiles_init(&data);
	data.starttime = ustime();
	for (long long i=0; i<CYCLES; i++) {
		if (i % data.log_centiles_modulus == 0) {
			log_centiles_update(&data, i);
		}
		raxInsert(tree, keys + (i * KEYLEN), KEYLEN, NULL, NULL);
	}
	data.endtime = ustime();
	printf("\r\n1N rax tree insert run\r\n");
	printf("rax tree nodes %llu\r\n", tree->numnodes);
	print_tree_data(tree, "insert");
	print_run_data(&data);
	log_centiles_print(&data);
	long long run1n = time_delta_ns(&data);
	long long rax_insert_1n_ns = time_delta_ns(&data);
	long long rax_insert_1n_avg_ns = rax_insert_1n_ns / CYCLES;




	
	log_centiles_init(&data);
	data.starttime = ustime();
	for (long long i=0; i<CYCLES; i++) {
		if (i % data.log_centiles_modulus == 0) {
			log_centiles_update(&data, i);
		}
		raxFind(tree, keys + (i * KEYLEN), KEYLEN);
	}
	data.endtime = ustime();
	printf("\r\n1N rax tree lookup run\r\n");
	print_tree_data(tree, "find");
	print_run_data(&data);
	log_centiles_print(&data);
	run1n = time_delta_ns(&data);
	long long rax_lookup_1n_ns = time_delta_ns(&data);
	long long rax_lookup_1n_avg_ns = rax_lookup_1n_ns / CYCLES;



	
	
	
	log_centiles_init(&data);
	data.starttime = ustime();
	for (long long i=0; i<CYCLES; i++) {
		if (i % data.log_centiles_modulus == 0) {
			log_centiles_update(&data, i);
		}
		raxRemove(tree, keys + (i * KEYLEN), KEYLEN, NULL);
	}
	data.endtime = ustime();
	printf("\r\n1N rax tree delete run\r\n");
	print_tree_data(tree, "remove");
	print_run_data(&data);
	log_centiles_print(&data);
	run1n = time_delta_ns(&data);
	long long rax_delete_1n_ns = time_delta_ns(&data);
	long long rax_delete_1n_avg_ns = rax_delete_1n_ns / CYCLES;



	printf("keylen,cycles,key read time ns,key read time avg ns,ts time ns, ts time avg ns,\r\n");
	printf("%d,%d,", KEYLEN, CYCLES);
	printf("%lld,%lld,", read_time_ns, read_time_avg_ns);
	printf("%lld,%lld,", ts_time_ns, ts_time_avg_ns);
	printf("\r\n");


	printf("type,rax tree op ns,rax tree op avg ns,\r\n");
	printf("%s,", "raxInsert");
	printf("%lld,%lld,", rax_insert_1n_ns, rax_insert_1n_avg_ns);
	printf("\r\n");	
	printf("%s,", "raxFind");
	printf("%lld,%lld,", rax_lookup_1n_ns, rax_lookup_1n_avg_ns);
	printf("\r\n");
	printf("%s,", "raxRemove");
	printf("%lld,%lld,", rax_delete_1n_ns, rax_delete_1n_avg_ns);
	printf("\r\n");
}

int main() {
	srand(123145151);
	printf("benchmark start\r\n");
	bench();
	printf("benchmark end\r\n");
}
