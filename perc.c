// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Code for node percolation on networks by Petter Holme (2020)
// Following the Newman-Ziff algorithm

// Compile by:
// gcc -Ofast -march=native-o perc perc.c -lm

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>

#define NAVG 10000 // number of runs for averages

// auxiliary macro
#define SQ(x) (((double) x) * (x))

// struct with all info about the nodes
typedef struct NODE {
	unsigned int deg, size; // degree and size of the cluster if the node is root (i.e. if parent = NULL)
	struct NODE **nb, *parent; // neighbors and parents in the tree
} NODE;

typedef struct GLOBALS { // collecting the global variables
	unsigned int n; // number of nodes
	struct NODE **order; // order of nodes being merged
	double *s1, *s2; // for stats
	uint64_t state; // rng state
} GLOBALS;


GLOBALS g;
NODE *n;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// this function contains the random number generator, derived from the PCG
// RNG v0.94 http://www.pcg-random.org under the Apache License 2.0
// http://www.apache.org/licenses/LICENSE-2.0

// 32-bit Output, 64-bit State: PCG-XSH-RS

uint32_t pcg_32 () {
	uint64_t state = g.state;
	uint32_t value, rot;

	g.state = g.state * 6364136223846793005ull + 1442695040888963407ull;
	value = ((state >> 18u) ^ state) >> 27u;
	rot = state >> 59u;
	return (value >> rot) | (value << ((- rot) & 31));
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// returning a 32-bit random number in the interval [0,bound-1]

uint32_t pcg_32_bounded (uint32_t bound) {
	uint32_t threshold = -bound % bound, r;

	do r = pcg_32();
	while (r < threshold);

	return r % bound;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// read the network
// The file format has to be as follows:
// The first row has the number of nodes N and edges M
// The next N rows are: [the degree of the node of line number - 1]
// The rest of the file lists the edges

void read_data () {
	unsigned int i, j, k, m;

	if (2 != scanf("%u %u\n", &g.n, &m)) {
		fprintf(stderr, "reading error (1)\n");
		exit(1);
	}

	n = calloc(g.n, sizeof(NODE));
	g.order = malloc(g.n * sizeof(NODE *));
	g.s1 = calloc(g.n, sizeof(double));
	g.s2 = calloc(g.n, sizeof(double));

	for (i = 0; i < g.n; i++) {
		if (1 != scanf("%u\n", &j)) {
			fprintf(stderr, "reading error (2)\n");
			exit(1);
		}
		n[i].nb = malloc(j * sizeof(NODE *));
		g.order[i] = n + i;
	}

	for (i = 0; i < m; i++) {
		if (2 != scanf("%u %u\n", &j, &k)) {
			fprintf(stderr, "reading error (1)\n");
			exit(1);
		}
		n[k].nb[n[k].deg++] = n + j;
		n[j].nb[n[j].deg++] = n + k;
	}
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// run the percolation process once

void perc () {
	unsigned int i, j, s = 0;
	NODE *me, *you, *root;

	// get a random ordering of the nodes
	for (i = g.n - 1; i > 0; i--) { // Fisher-Yates shuffle
		j = pcg_32_bounded(i);
		me = g.order[i];
		g.order[i] = g.order[j];
		g.order[j] = me;
	}

	// initialize
	for (i = 0; i < g.n; i++) {
		n[i].parent = NULL;
		n[i].size = 0;
	}

	for (i = 0; i < g.n; i++) {
		root = me = g.order[i];
		me->size = 1;
		
		for (j = 0; j < me->deg; j++) {
			you = me->nb[j];
			if (you->size) {
				// replace you by the root of its cluster
				while (you->parent) you = you->parent;

				// if you is in a different cluster, merge it
				if (you != root) {
					if (root->size < you->size) { // merge small to large
						you->size += root->size;
						root->parent = you;
						root = you;
					} else {
						root->size += you->size;
						you->parent = root;
					}
				}
			}
		}
		// update largest cluster size
		if (s < root->size) s = root->size;

		g.s1[i] += s; // save for stats
		g.s2[i] += SQ(s);
	}
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// main function handling input

int main (int argc, char *argv[]) {
	unsigned int i;

	// read rng seed/state (the only argument to the program, a 64-bit uint)
	g.state = (uint64_t) strtoull(argv[1], NULL, 10);
	 
	// read network data
	read_data();

	// run the simulations and summing for averages
	for (i = 0; i < NAVG; i++) perc();

	// print result
	for (i = 0; i < g.n; i++) {
		g.s1[i] /= NAVG;
		g.s2[i] /= NAVG;
		printf("%g %g %g\n", (i + 1) / (double) g.n, g.s1[i], sqrt((g.s2[i] - SQ(g.s1[i])) / (NAVG - 1.0)));
	}

	// cleaning up
	for (i = 0; i < g.n; i++) free(n[i].nb);
	free(n); free(g.order); free(g.s1); free(g.s2);

	return 0;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
