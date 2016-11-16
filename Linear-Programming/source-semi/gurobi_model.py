# coding:utf-8
from gurobipy import *


def get_var_flow(m, nodes, t):
    flow = [{}, {}]  # two dict()
    useEdges = []
    for n in nodes:
        if n.id == t.id: continue
        for e in n.outEdges:
            useEdges.append(e)
            flow[0][e.id] = m.addVar(vtype=GRB.BINARY)
            flow[1][e.id] = m.addVar(vtype=GRB.BINARY)
    return flow, useEdges


def get_objective_Expr(m, flow, nodes, t):
    obj_and_sum = QuadExpr()
    obj_cost_sum = LinExpr()
    for n in nodes:
        if n.id == t.id: continue
        for e in n.outEdges:
            obj_and_sum += flow[0][e.id] * flow[1][e.id]
            obj_cost_sum += e.cost * (flow[0][e.id] + flow[1][e.id])
    return obj_and_sum, obj_cost_sum


def get_objective_addVar(m, flow, useEdges):
    obj_and = m.addVars(len(useEdges), vtype=GRB.BINARY, name="obj_and")
    obj_cost = m.addVars(len(useEdges), vtype=GRB.INTEGER, name="obj_cost")
    for i, e in enumerate(useEdges):
        m.addGenConstrAnd(obj_and[i], [flow[0][e.id], flow[1][e.id]])
        # m.addConstr(obj_and[i] == flow[0][e.id] * flow[1][e.id])
        m.addConstr(obj_cost[i] == e.cost * (flow[0][e.id] + flow[1][e.id]))
    obj_and_sum = m.addVar(vtype=GRB.INTEGER, name="obj_and_sum")
    obj_cost_sum = m.addVar(vtype=GRB.INTEGER, name="obj_cost_sum")
    m.addConstr(obj_and_sum == obj_and.sum(), name="obj_and_sum")
    m.addConstr(obj_cost_sum == obj_cost.sum(), name="obj_cost_sum")
    return obj_and_sum, obj_cost_sum


def set_single_obj(m, obj_and_sum, obj_cost_sum):
    # m.addConstr(obj_and_sum >= 0) # for cplex
    # m.addConstr(obj_and_sum <= 0) # for cplex

    m.setObjective(obj_and_sum, GRB.MINIMIZE)

    # m.addConstr(obj_and_sum == 0)
    # m.setObjective(obj_cost_sum, GRB.MINIMIZE)


def set_multi_obj(m, obj_and_sum, obj_cost_sum):
    m.ModelSense = GRB.MINIMIZE
    m.NumObj = 2

    m.setParam(GRB.Param.ObjNumber, 0)
    m.ObjNPriority = 2
    obj_and_sum.ObjN = 1

    m.setParam(GRB.Param.ObjNumber, 1)
    m.ObjNPriority = 1
    obj_cost_sum.ObjN = 1


def add_constraints(m, flow, nodes, s, t):
    # Add constraint 1 : must node(inDegree==outDegree==1), non-must node(inDegree==outDegree<=1)
    for i in range(2):
        m.addConstr(s.getOutDegree(flow[i]) == 1)
        m.addConstr(t.getInDegree(flow[i]) == 1)
    for n in nodes:
        if n.id == s.id or n.id == t.id or len(n.inEdges) == 0 or len(n.outEdges) == 0: continue
        for i in range(2):
            m.addConstr(n.getOutDegree(flow[i]) == n.getInDegree(flow[i]))
            if n.belongto == i:
                m.addConstr(n.getOutDegree(flow[i]) == 1)
            else:
                m.addConstr(n.getOutDegree(flow[i]) <= 1)

    # Add constraint 2 : only one loop in graph
    u = [{}, {}]
    for n in nodes:
        u[0][n.id] = m.addVar(vtype=GRB.INTEGER, lb=1, ub=len(nodes))
        u[1][n.id] = m.addVar(vtype=GRB.INTEGER, lb=1, ub=len(nodes))
    for i in range(2):
        m.addConstr(u[i][s.id] == 1)
        m.addConstr(u[i][t.id] == len(nodes))
    for n in nodes:
        if n.id == t.id: continue
        for e in n.outEdges:
            for i in range(2):
                m.addConstr(u[i][e.src] - u[i][e.dst] + flow[i][e.id] * len(nodes) <= len(nodes) - 1)
    return u
