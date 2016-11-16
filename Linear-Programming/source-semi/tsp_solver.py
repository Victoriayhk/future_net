# coding:utf-8

import time
from readData import *
from gurobi_model import *
from wtiteData import *
import subprocess


def tsp_solver(case):
    start_time = time.time()

    edges, nodes_num = create_edges(case)
    nodes = create_nodes(nodes_num)
    demand = create_demand(case)
    s, t = set_belongto(nodes, demand)
    cost, id, subcost, subid = get_cost_and_id(nodes, edges)
    add_edge2node(nodes, edges, s, t, cost, subcost, id, subid)

    #################################### gurobi model ##########################################
    # create model
    m = Model("TSP")
    # m.params.LogToConsole = 0       # whether print gurobi log
    # m.setParam('TimeLimit', 60*13)  # set limited time
    flow, useEdges = get_var_flow(m, nodes, t)

    # obj_and_sum, obj_cost_sum = get_objective_Expr(m, flow, nodes, t)
    obj_and_sum, obj_cost_sum = get_objective_addVar(m, flow, useEdges)

    set_single_obj(m, obj_and_sum, obj_cost_sum)
    # set_multi_obj(m, obj_and_sum, obj_cost_sum)

    u = add_constraints(m, flow, nodes, s, t)
    m.update()
    m.write('tsp.lp')
    m.optimize()
    if m.Status != GRB.Status.OPTIMAL: return

    #################################### print result ##########################################
    recoredNext(nodes, t, flow)
    writeResult(case, nodes, s, t)
    print "exam result(road0,road1,overlap,road sum):"
    subprocess.call(["./future_net_exam %s/topo.csv %s/demand.csv %s/result_gurobi.csv" % (case, case, case)], shell=True)

    overlap_edge = 0
    cost_sum = 0
    for n in nodes:
        if n.id == t.id: continue
        for e in n.outEdges:
            overlap_edge += int(round(flow[0][e.id].x) * round(flow[1][e.id].x))
            cost_sum += e.cost * (round(flow[0][e.id].x) + round(flow[1][e.id].x))
    if obj_and_sum.__class__ == QuadExpr:
        print case, "\nobj_and_sum", obj_and_sum.getValue()
    elif obj_and_sum.__class__ == Var:
        print case, "\nobj_and_sum", obj_and_sum.x
    print "overlap_edge = %d" % overlap_edge
    print "cost_sum     = %d" % cost_sum
    print("time         = %d ms\n" % (1000 * (time.time() - start_time)))
