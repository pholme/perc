# Python wrapper for the percolation C code
# this example takes an input file -- an edge list representation
# of a network, then calculates the expected size of the largest connected
# component, given the percolation probability, then prints this to a PDF
# file

import networkx as nx
from sys import argv
from random import getrandbits
from subprocess import Popen, PIPE, STDOUT
import matplotlib.pyplot as plt

# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

if __name__ == "__main__":

	if len(argv) != 3:
		print('usage: python3 perc.py [edge list file] [output PDF file]')
		exit(1)

	G = nx.convert_node_labels_to_integers(nx.read_edgelist(argv[1]))

	# assembling the network in the format of the C code
	txt = str(G.number_of_nodes()) + ' ' + str(G.number_of_edges()) + '\n'
	for i in range(G.number_of_nodes()):
		txt += str(G.degree(i)) + '\n'
	for u,v in G.edges():
		txt += str(u) + ' ' + str(v) + '\n'

	# running the C code
	p = Popen(['./perc', str(getrandbits(64))], stdout=PIPE, stdin=PIPE, stderr=STDOUT)
	o = p.communicate(input=bytes(txt,encoding='utf-8'))[0]

	# interpret and plot the output
	a = o.decode().split('\n')
	
	x = []
	y = []
	yerr = []

	for l in a:
		b = l.split()
		if len(b) == 3:
			x.append(float(b[0]))
			y.append(float(b[1]))
			yerr.append(float(b[2]))

	plt.xlabel('Percolation probability')
	plt.ylabel('Size of largest cluster')
	plt.errorbar(x,y,yerr)
	plt.savefig(argv[2])

# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
