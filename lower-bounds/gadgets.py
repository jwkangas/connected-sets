
import subprocess
import sys
import numpy as np
from numpy import linalg as LA

# path of the program for counting connected sets
PATH_COUNT = "./count"


# parses a string into an edge list
def parse_edgelist(edgelist):
	return [] if edgelist == "" else [tuple(map(int, e.split("-"))) for e in edgelist.split(",")]

# converts a neighbor list representation to an edge set
def neighbors_to_edges(neighbors):
	edges = set()
	for i in range(len(neighbors)):
		for u in neighbors[i]:
			if u > i:
				edges.add((i,u))
	return edges


threshold = 1e-12

# a class to hold gadget data
class Gadget:
	def count_connected_sets_c_program(self):
		print("Counting connected sets...")
		gt = {"C" : "I", "L" : "II"}[self.gtype]
		adjacencies = [str(v) for e in self.edges for v in e]
		args = [PATH_COUNT, gt, str(self.n)] + adjacencies
		proc = subprocess.Popen(args, stdout=subprocess.PIPE)
		return proc.stdout.read()
	
	def type_i_bound(self, recount):
		if recount:
			base = int(self.count_connected_sets_c_program().strip())
		else:
			base = self.n_sets
		
		print("Connected sets:", base)
		
		# the exponential growth by number of vertices
		bound = base ** (1 / self.n)
		
		print("Lower bound:", ("%.20f" % bound)[:14])
		print(self.bound)
		
		assert(self.bound - bound < threshold)
		return (bound, base)
	
	def type_ii_bound(self, recount):
		if recount:
			# the ways to produce B, U, and D type sets from previous B, U, and D type sets
			counts = [int(c) for c in self.count_connected_sets_c_program().strip().split()]
			bb, ub, db, bu, uu, du, bd, ud, dd = counts
			matrix = ((bb,ub,db), (bu,uu,du), (bd,ud,dd))
		else:
			matrix = self.matrix
		
		print("Matrix:", matrix)
		
		A = np.array(matrix)
		
		# eigendecomposition of A and coefficients for eigenvalues
		eigenvalues, P = LA.eig(A)
		
		# the base of exponential growth when chaining gadgets
		base = max(abs(e) for e in eigenvalues)
		print("Largest eigenvalue:", base)
		
		# the exponential growth by number of vertices
		bound = base ** (1 / (self.n - 2))
		
		print("Lower bound:", ("%.20f" % bound)[:14])
		
		assert(self.bound - bound < threshold)
		return (bound, matrix)
	
	def recompute_bound(self, recount):
		print("Computing a lower bound for a type %s gadget of degree %i and %i vertices." % ({"C" : "I", "L" : "II"}[self.gtype], self.degree, self.n))
		print("Edges:", " ".join("%i-%i" % e for e in self.edges))
		
		if self.gtype == "C":
			bound, counts = self.type_i_bound(recount)
			self.bound = bound
			self.n_sets = counts
		elif self.gtype == "L":
			bound, counts = self.type_ii_bound(recount)
			self.bound = bound
			self.matrix = counts
		else:
			raise Exception
	
	def get_line(self):
		edges = neighbors_to_edges(self.neighbors)
		
		key = "%s-%s-%s" % (self.gtype, self.degree, self.n)
		
		if self.gtype == "C":
			counts = "c:%i" % self.n_sets
		elif self.gtype == "L":
			counts = "m:" + "|".join(",".join(str(a) for a in l) for l in self.matrix)
		else:
			raise Exception
		
		line = "%-7s  %s  %-6s  %-80s   :%s" % (key, ("%.20f" % self.bound)[:14], " ".join(self.flags), counts, ",".join("%i-%i" % (u,v) for u,v in sorted(edges)))
		
		return line


# parses a gadget string representation into a Gadget object
def parse_gadget(gadgetline):
	gadget = Gadget()
	gadget.optimal = False
	gadget.flags = []
	
	parts = gadgetline.split()
	
	gadget.key = parts[0]
	keyparts = gadget.key.split("-")
	gadget.gtype = keyparts[0]
	gadget.degree = int(keyparts[1])
	gadget.n = int(keyparts[2])
	gadget.bound = float(parts[1])
	
	for part in parts[2:]:
		if ":" not in part:
			gadget.flags.append(part)
			continue
		
		attr, value = part.split(":", 1)
		if attr == "c":
			gadget.n_sets = int(value)
		elif attr == "m":
			gadget.matrix = [[int(c) for c in r.split(",")] for r in value.split("|")]
		elif attr == "":
			gadget.edges = parse_edgelist(value)
			n = gadget.n
			gadget.neighbors = [[] for i in range(n)]
			for i,j in sorted(gadget.edges):
				gadget.neighbors[i].append(j)
				gadget.neighbors[j].append(i)
		else:
			raise Exception(attr)
	
	return gadget


# reads all gadget from given file
def read_gadgets(path):
	gadgets = {}
	
	with open(path, "r") as f:
		while True:
			line = f.readline()
			if not line:
				break
			
			# ignore all non-gadget lines
			if not (line.startswith("L-") or line.startswith("C-")):
				continue
			
			parts = line.split()
			name = parts[0]
			t, d, n = name.split("-")
			
			gadgets[(t, int(d), int(n))] = parse_gadget(line)
	
	return gadgets



recount = False
key = None
for arg in sys.argv[1:]:
	if arg == "--count":
		recount = True
	else:
		key = arg

gadgets = read_gadgets("gadgets.txt")

if key is not None:
	gt = key.split("-")
	gt = (gt[0], int(gt[1]), int(gt[2]))
	gadgets[gt].recompute_bound(recount)
	exit()

for gadget in sorted(gadgets):
	g = gadgets[gadget]
	g.recompute_bound(recount)
	print()




