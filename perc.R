# R wrapper for the percolation C code
# this example takes an input file -- an edge list representation
# of a network, then calculates the expected size of the largest connected
# component, given the percolation probability, then prints this to a PDF
# file

library(igraph)
library(numbers)
library(ggplot2)

# reading file name
argv = commandArgs(trailingOnly = TRUE)

if (length(argv) != 2) {
  stop("usage: Rscript perc.R [edge list file] [output fig file]", call. = FALSE)
}

# assembling the network in the format of the C code
g <- read_graph(argv[1], format = c("edgelist"))

nwk <- vector()
nwk <- c(nwk, paste(gorder(g), ecount(g)))
for (v in V(g)) {
	nwk <- c(nwk,degree(g, toString(v)))
}
for (e in E(g)) {
	v = ends(g, e)
	nwk <- c(nwk, paste(v[1] - 1, v[2] - 1))
}
nwk <- c(nwk, "") # needed to get a final newline
txtnwk <- paste(nwk, collapse = "\n")

# getting a string representation of a relatively random 64-bit uint
seed <- sample(0:99999, 4, replace = TRUE)
txtseed <- sprintf("%d%05d%05d%05d", mod(seed[1], 9223), seed[2], seed[3], seed[4])

# running the C code
o <- system2("./perc", args = txtseed, input = txtnwk, stdout = TRUE)

# parsing and plotting
d <- read.table(text = paste(o, collapse = "\n"), header = FALSE)
colnames(d) <- c("x", "y", "yerr")
pdf(argv[2])
outplot <- ggplot(d, aes(x = x, y = y)) + geom_errorbar(aes(ymin = y - yerr, ymax = y + yerr)) + geom_line() + labs(x = "Percolation probability", y = "Size of largest cluster")
print(outplot)
dev.off()
