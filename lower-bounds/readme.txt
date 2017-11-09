
This is a program for proving lower bounds on the number of connected sets in graphs of bounded vertex degree, by analyzing graphs obtained by linking together gadgets of bounded size.

The file 'gadgets.txt' contains the gadgets, which are certificates for the claimed lower bounds.

To verify the bounds, first compile the C99 program 'count', e.g. on Linux using GCC:

  gcc count.c -o count --std=c99 -O3

Then run the Python3 script 'gadgets.py' (requires the 'numpy' package):

  python3 gadgets.py

The script will read the gadgets from 'gadgets.txt' and verify the implied lower bounds. By default this computation will use the precomputed counts of connected sets. By using

  python3 gadgets.py --count

the script will instead invoke the program 'count' to recompute the number of connected sets. Depending on your system you may first need to edit line 8 in 'gadgets.py' to match the name of the counting program (e.g. 'count.exe').

It's also possible to specify only a single gadget to verify, e.g.

  python3 gadgets.py L-3-10

with or without --count.
