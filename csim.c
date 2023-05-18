// Submitted by Niket Gautam

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "lab3.h"

struct cacheline {
	unsigned long long accessedAt, tag;
	short valid;
};

int main(int argc, char*argv[]) {
	if (argc != 9 && argc != 10) {
		printf("Usage: ./csim [-hv] -s <num> -E <num> -b <num> -t <file>\nOptions:\n  -h         Print this help message.\n  -v         Optional verbose flag.\n  -s <num>   Number of set index bits.\n  -E <num>   Number of lines per set.\n  -b <num>   Number of block offset bits.\n  -t <file>  Trace file.\n\nExamples:\n  linux>  ./csim -s 4 -E 1 -b 4 -t traces/yi.trace\n  linux>  ./csim -v -s 8 -E 2 -b 4 -t traces/yi.trace\n");
		return 1;
	}
	unsigned long long cachehits = 0, cachemisses = 0, evictions = 0, associativity, verbose = 0, i = 1, setnobits, offsetbits, totalsets;
	// [-hv]
	if(strcmp(argv[i], "-v") == 0){
		verbose = 1;
		i++;
	}
	else if(strcmp(argv[i], "-s") != 0){
		printf("Usage: ./csim [-hv] -s <num> -E <num> -b <num> -t <file>\nOptions:\n  -h         Print this help message.\n  -v         Optional verbose flag.\n  -s <num>   Number of set index bits.\n  -E <num>   Number of lines per set.\n  -b <num>   Number of block offset bits.\n  -t <file>  Trace file.\n\nExamples:\n  linux>  ./csim -s 4 -E 1 -b 4 -t traces/yi.trace\n  linux>  ./csim -v -s 8 -E 2 -b 4 -t traces/yi.trace\n");
		return 1;
	}
	// -s
	if(strcmp(argv[i], "-s") != 0){
		printf("Usage: ./csim [-hv] -s <num> -E <num> -b <num> -t <file>\nOptions:\n  -h         Print this help message.\n  -v         Optional verbose flag.\n  -s <num>   Number of set index bits.\n  -E <num>   Number of lines per set.\n  -b <num>   Number of block offset bits.\n  -t <file>  Trace file.\n\nExamples:\n  linux>  ./csim -s 4 -E 1 -b 4 -t traces/yi.trace\n  linux>  ./csim -v -s 8 -E 2 -b 4 -t traces/yi.trace\n");
		return 1;
	}
	else{
			setnobits = atoi(argv[i+1]);
			i = i+2;
	}
	// -E
	if(strcmp(argv[i], "-E") != 0){
		printf("Usage: ./csim [-hv] -s <num> -E <num> -b <num> -t <file>\nOptions:\n  -h         Print this help message.\n  -v         Optional verbose flag.\n  -s <num>   Number of set index bits.\n  -E <num>   Number of lines per set.\n  -b <num>   Number of block offset bits.\n  -t <file>  Trace file.\n\nExamples:\n  linux>  ./csim -s 4 -E 1 -b 4 -t traces/yi.trace\n  linux>  ./csim -v -s 8 -E 2 -b 4 -t traces/yi.trace\n");
		return 1;
	}
	else{
			associativity = atoi(argv[i+1]);
			i = i+2;
	}
	// -b
	if(strcmp(argv[i], "-b") != 0){
		printf("Usage: ./csim [-hv] -s <num> -E <num> -b <num> -t <file>\nOptions:\n  -h         Print this help message.\n  -v         Optional verbose flag.\n  -s <num>   Number of set index bits.\n  -E <num>   Number of lines per set.\n  -b <num>   Number of block offset bits.\n  -t <file>  Trace file.\n\nExamples:\n  linux>  ./csim -s 4 -E 1 -b 4 -t traces/yi.trace\n  linux>  ./csim -v -s 8 -E 2 -b 4 -t traces/yi.trace\n");
		return 1;
	}
	else{
			offsetbits = atoi(argv[i+1]);
			i = i+2;
	}
	// -t
	if(strcmp(argv[i], "-t") != 0){
		printf("Usage: ./csim [-hv] -s <num> -E <num> -b <num> -t <file>\nOptions:\n  -h         Print this help message.\n  -v         Optional verbose flag.\n  -s <num>   Number of set index bits.\n  -E <num>   Number of lines per set.\n  -b <num>   Number of block offset bits.\n  -t <file>  Trace file.\n\nExamples:\n  linux>  ./csim -s 4 -E 1 -b 4 -t traces/yi.trace\n  linux>  ./csim -v -s 8 -E 2 -b 4 -t traces/yi.trace\n");
		return 1;
	}
	FILE* filedescriptor = fopen(argv[i+1], "r");
	if (filedescriptor == NULL) {
		printf("Error: Input trace file not found.\n");
		return 1;
	}
	totalsets =  (1 << setnobits);
	unsigned long long j, k, size, address, tag, setno, tagbits, totallines = totalsets*associativity;
	char accesstype[5];
	struct cacheline * cachelines;
	cachelines = malloc((totallines)*(sizeof(struct cacheline)));
	for (i = 0; i < (totallines); i++)
		cachelines[i].accessedAt =  cachelines[i].valid = 0;
	while (fscanf(filedescriptor, "%s", accesstype) == 1) {
		fscanf(filedescriptor, "%llx,", &address);
		fscanf(filedescriptor, "%llx", &size);
		if (accesstype[0] == 'I')
			continue;
		if (verbose==1)
			printf("%s %llx, %lld", accesstype, address, size);
		tagbits = 64 - offsetbits - setnobits;
		setno = ((address << tagbits) >> (tagbits + offsetbits));
		tag = (address >> (setnobits + offsetbits));
		for (i = setno*associativity; i < (setno + 1)*associativity; i++) {
			if (cachelines[i].valid == 1 && cachelines[i].tag == tag) {
				if (verbose==1)
					printf(" hit");
				cachehits++;
				j = i;
				break;
			}
		}
		if (i == (setno + 1)*associativity) {
			for (i = setno*associativity; i < (setno + 1)*associativity; i++) {
				if (cachelines[i].valid == 0) {
					cachelines[i].valid = 1;
					cachelines[i].tag = tag;
					if (verbose==1)
						printf(" miss");
					cachemisses++;
					j = i;
					break;
				}
			}
		}
		if (i == (setno + 1)*associativity) {
			j = setno*associativity;
			for (i = setno*associativity; i < (setno + 1)*associativity; i++) {
				if (cachelines[i].accessedAt > cachelines[j].accessedAt)
					j = i;
			}
			cachelines[j].tag = tag;
			if (verbose==1)
				printf(" miss eviction");
			evictions++;
			cachemisses++;
		}
		for (k = 0; k < totallines; k++) {
			if (cachelines[k].valid == 1)
				cachelines[k].accessedAt++;
		}
		cachelines[j].accessedAt = 0;
		if (accesstype[0] == 'M'){
			if (verbose==1)
				printf(" hit");
			cachehits++;
		}
		if (verbose==1)
			printf(" \n");
	}
	free(cachelines);
	printSummary(cachehits, cachemisses, evictions);
	return 0;
}
