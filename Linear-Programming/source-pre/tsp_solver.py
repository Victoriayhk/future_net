# coding:utf-8

from gurobipy import *
from node_edge import NODE, EDGE
import csv
import re
import time


def tsp_solver(case):
    start_time = time.time()

    # create edges(假设e.id为edges中的位置)
    nodes_num = 0
    edges = []
    with open(case + '/topo.csv', 'rb') as topofile:
        reader = csv.reader(topofile)
        for row in reader:
            edges.append(EDGE(row))
            nodes_num = max(nodes_num, max(int(row[1]), int(row[2])))
    edges.sort(key=lambda edge: edge.id, reverse=False)
    nodes_num += 1

    # create nodes
    nodes = []
    for id in range(0, nodes_num):
        nodes.append(NODE(id))

    #################################### read deamand.csv ##########################################
    with open(case + '/demand.csv', 'rb') as demandfile:
        line = demandfile.readline()
        demand = re.split(",| |\|", line)

    # must nodes
    s = nodes[int(demand[0])]
    t = nodes[int(demand[1])]
    for i in demand:
        nodes[int(i)].isMust = True

    # create matrix : cost[][] & id[][]
    cost = [[sys.maxint for x in range(len(nodes))] for y in range(len(nodes))]
    id = [[-1 for x in range(len(nodes))] for y in range(len(nodes))]
    subcost = [[sys.maxint for x in range(len(nodes))] for y in range(len(nodes))]
    subid = [[-1 for x in range(len(nodes))] for y in range(len(nodes))]
    for e in edges:
        if cost[e.src][e.dst] >= e.cost:
            subcost[e.src][e.dst] = cost[e.src][e.dst]
            subid[e.src][e.dst] = id[e.src][e.dst]
            cost[e.src][e.dst] = e.cost
            id[e.src][e.dst] = e.id
        elif subcost[e.src][e.dst] > e.cost:
            subcost[e.src][e.dst] = e.cost
            subid[e.src][e.dst] = e.id

    # add edge to node
    for i in range(0, len(nodes)):
        for j in range(0, len(nodes)):
            if i == t.id or j == s.id: continue
            if cost[i][j] != sys.maxint:
                e = edges[id[i][j]]
                e.order = "1st"
                nodes[e.src].outEdges.append(e)
                nodes[e.dst].inEdges.append(e)
            if subcost[i][j] != sys.maxint:
                e = edges[subid[i][j]]
                e.order = "2nd"
                nodes[e.src].outEdges.append(e)
                nodes[e.dst].inEdges.append(e)
    #################################### gurobi model ##########################################
    # create model
    m = Model("TSP")
    m.params.LogToConsole = 0

    # create variables & add variables to model
    flow = {}
    obj = LinExpr()
    for n in nodes:
        if n.id == t.id: continue
        for e in n.outEdges:
            flow[e.id] = m.addVar(vtype=GRB.BINARY)
            obj += e.cost * flow[e.id]
    u = {}
    for n in nodes:
        u[n.id] = m.addVar(vtype=GRB.INTEGER, lb=1, ub=len(nodes))
    print "create flow{} & u[]"

    # integrate new variables & add objective to model
    m.update()
    m.setObjective(obj, GRB.MINIMIZE)
    print "update & setObjective"

    # Add constraint 1 : must node(inDegree==outDegree==1), non-must node(inDegree==outDegree<=1)
    m.addConstr(s.getOutDegree(flow) == 1)
    m.addConstr(t.getInDegree(flow) == 1)

    for n in nodes:
        if n.id == s.id or n.id == t.id: continue
        if len(n.inEdges) == 0 or len(n.outEdges) == 0: continue
        m.addConstr(n.getOutDegree(flow) == n.getInDegree(flow))
        if n.isMust is True:
            m.addConstr(n.getOutDegree(flow) == 1)
        else:
            m.addConstr(n.getOutDegree(flow) <= 1)
    print "add constraint 1"

    # Add constraint 2 : only one loop in graph
    m.addConstr(u[s.id] == 1)
    m.addConstr(u[t.id] == len(nodes))
    for n in nodes:
        if n.id == t.id: continue
        for e in n.outEdges:
            m.addConstr(u[e.src] - u[e.dst] + flow[e.id] * len(nodes) <= len(nodes) - 1)
    print "add constraint 2"

    # Compute optimal solution
    print "start optimize()"
    m.optimize()
    print "obj  = ", int(m.objVal)
    print("time =  %d ms\n" % (1000 * (time.time() - start_time)))
