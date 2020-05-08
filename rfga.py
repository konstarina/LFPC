import networkx as nx
import matplotlib.pyplot as plt

rules = [
	'S-aB',
	'B-bS',
	'B-aC',
	'B-c',
	'C-bD',
	'D-c',
	'D-aC'
]


def drawGraph():
	G = nx.MultiDiGraph()
	G.add_edges_from(
	[('S', 'B'), ('B', 'S'), ('B', 'C'), ('B', 'Z'), ('C', 'D'),
	('D', 'Z'), ('D', 'C')])

	# Need to create a layout when doing
	# separate calls to draw nodes and edges
	pos = nx.spring_layout(G)
	nx.draw_networkx_nodes(G, pos)
	nx.draw_networkx_labels(G, pos)
	nx.draw_networkx_edges(G, pos, arrows=True, )
	nx.draw_networkx_edge_labels(G, pos, edge_labels=
		dict([
			(('S', 'B'), 'a <---> b'),
			(('B', 'C'), 'a'),
			(('B', 'Z'), 'c'),
			(('C', 'D'), 'b <---> a'),
			(('D', 'Z'), 'c'),
		])
	)
	plt.show()

def getRules(rules):
	fa = {}
	for rule in rules:
		uInput = rule.split('-')

		x = {
			0: list(uInput[0])[0],
			1: list(uInput[1])
		}

		if not x[0] in fa:
			fa[x[0]] = {}
		if len(x[1]) == 1:
			x[1].append('Z')
		
		fa[ x[0] ][ x[1][0] ] = x[1][1]
	return fa

def accepts(fa, initial, final, string):
	state = initial
	for x in string:
		if state in fa and x in fa[state]:
			state = fa[state][x]
		else:
			return False

	return state in final

drawGraph()

fa = getRules(rules)
print(fa)
while True:
	uInput = input('> ')

	if uInput == '':
		break

	print(accepts(fa, 'S', {'Z'}, uInput)) 
