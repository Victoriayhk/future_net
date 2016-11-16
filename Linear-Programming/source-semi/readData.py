# coding:utf-8

import csv
import sys
from node_edge import NODE, EDGE
import re


def create_edges(case):
    nodes_num = 0
    edges = []
    with open(case + '/topo.csv', 'rb') as topofile:
        reader = csv.reader(topofile)
        for row in reader:
            edges.append(EDGE(row))
            nodes_num = max(nodes_num, max(int(row[1]), int(row[2])))
    nodes_num += 1
    return edges, nodes_num


def create_nodes(nodes_num):
    nodes = []
    for i in range(0, nodes_num):
        nodes.append(NODE(i))
    return nodes


def create_demand(case):
    demand = []
    with open(case + '/demand.csv', 'rb') as demandfile:
        for i in range(2):
            line = demandfile.readline()
            demand.append(re.split(",| |\|", line))
    return demand


def set_belongto(nodes, demand):
    s = nodes[int(demand[0][1])]
    t = nodes[int(demand[0][2])]
    for i in range(2):
        if demand[i][3] not in 'NA\r\n':
            for j in demand[i][1:]:
                nodes[int(j)].belongto = i
    return s, t

def get_cost_and_id(nodes, edges):
    cost = [[sys.maxint for x in range(len(nodes))] for y in range(len(nodes))]
    id = [[-1 for x in range(len(nodes))] for y in range(len(nodes))]
    subcost = [[sys.maxint for x in range(len(nodes))] for y in range(len(nodes))]
    subid = [[-1 for x in range(len(nodes))] for y in range(len(nodes))]
    for i, e in enumerate(edges):
        if cost[e.src][e.dst] >= e.cost:
            subcost[e.src][e.dst] = cost[e.src][e.dst]
            subid[e.src][e.dst] = id[e.src][e.dst]
            cost[e.src][e.dst] = e.cost
            id[e.src][e.dst] = i
        elif subcost[e.src][e.dst] > e.cost:
            subcost[e.src][e.dst] = e.cost
            subid[e.src][e.dst] = i
    return cost, id, subcost, subid


def add_edge2node(nodes, edges, s, t, cost, subcost, id, subid):
    for i in range(0, len(nodes)):
        for j in range(0, len(nodes)):
            if i == t.id or j == s.id: continue
            if cost[i][j] != sys.maxint:
                e = edges[id[i][j]]
                nodes[e.src].outEdges.append(e)
                nodes[e.dst].inEdges.append(e)
            if subcost[i][j] != sys.maxint:
                e = edges[subid[i][j]]
                nodes[e.src].outEdges.append(e)
                nodes[e.dst].inEdges.append(e)
