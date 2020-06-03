# Node percolation on networks

This is a C implementation with a Python wrapper for [Newman-Ziff's algorithm for node percolation on networks](https://journals.aps.org/pre/abstract/10.1103/PhysRevE.64.016706). It is meant to be fast, complete and readable (at the expense of memory optimization -- the original code is better at that).

Compile it as:

`gcc -Ofast -march=native -o perc perc.c -lm`

Run it as:

`python3 perc.py iceland.lnk iceland.pdf`

The output is an (standard error) errorbar plot of the size of the largest connected component as a function of the percolation probability. It is printed to a PDF file whose name should be given on the command line. If you are interested in the response to random deletion of nodes, you have to mirror the x-axis (the fraction of removed nodes is one minus the percolation probability).

There is an equivalent example how to run the C program from R. That one could be run as a R script as:

`Rscript perc.R iceland.lnk iceland.pdf`

The example network comes from:
[Haraldsdottir S, Gupta S, Anderson RM, Preliminary studies of sexual networks in a male homosexual community in Iceland, J Acquir Immune Defic Syndr. 1992;5(4):374-81.](https://journals.lww.com/jaids/abstract/1992/04000/preliminary_studies_of_sexual_networks_in_a_male.8.aspx)
