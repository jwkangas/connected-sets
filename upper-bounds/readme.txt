
This is a program for verifying the upper bounds on the number of connected sets in graphs of bounded vertex degree, as obtained by application of Shearer's inequality to augmented vertex neighborhoods.

The programs are written in C99. Specific instructions for compiling and running under Linux using GCC:

  gcc enumerate.c graph.c -o enumerate --std=c99 -O3
  gcc count.c graph.c -o count -lm --std=c99 -O3

This produces two programs, 'enumerate' and 'count'. The former enumerates all essential neighborhoods of radius 2 and given maximum degree and writes them into the standard output. To use, run

  ./enumerate MAX_DEGREE [SIZE] > neighborhoods.txt

The SIZE parameter is optional. If omitted, the program will enumerate neighborhoods of all sizes. The program will also print additional information into the error output.

The program 'count' reads a list of neighborhoods from the standard input. For each neighborhood, it counts the number of boundary-connected sets, the ratio between the number of boundary-connected sets and the number of all subsets, and the implied upper bound on the number of connected sets in a graph of bounded degree (see paper). To use, run

  ./count MAX_DEGREE < neighborhoods.txt

The MAX_DEGREE parameter must be the same as used with 'enumerate'; it is not inferred automatically from neighborhood.txt.

A full example for degree 4:

  ./enumerate 4 > neighborhoods_of_degree_4.txt
  ./count 4 < neighborhoods_of_degree_4.txt

You may also use e.g.

  ./enumerate 4 | ./count 4

However, the secondary information written to the error output by the two programs may appear in indeterminate order.

The files 'neighborhoods3' and 'neighborhoods4' are included. You may use these to verify that 'enumerate' gives the correct output. Note that starting at degree 5 the output will be several megabytes.
