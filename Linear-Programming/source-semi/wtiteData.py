# coding:utf-8


def recoredNext(nodes, t, flow):
    for n in nodes:
        if n.id == t.id: continue
        for e in n.outEdges:
            if round(flow[0][e.id].x) == 1:
                n.next[0] = e
            if round(flow[1][e.id].x) == 1:
                n.next[1] = e


def writeResult(case, nodes, s, t):
    with open(case + '/result_gurobi.csv', 'wb') as resultFile:
        for i in range(2):
            n = s
            while True:
                resultFile.write("%d" % n.next[i].id)
                n = nodes[n.next[i].dst]
                if n.id == t.id:
                    break
                resultFile.write("|")
            if i == 0: resultFile.write("\n")
        resultFile.close()
