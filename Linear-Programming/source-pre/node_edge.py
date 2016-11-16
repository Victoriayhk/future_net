# coding:utf-8


class NODE:
    def __init__(self, id):
        self.isMust = False
        self.id = id
        self.inEdges = []
        self.outEdges = []

    def getInDegree(self, flow):
        degree = 0
        for e in self.inEdges:
            degree += flow[e.id]
        return degree

    def getOutDegree(self, flow):
        degree = 0
        for e in self.outEdges:
            degree += flow[e.id]
        return degree


class EDGE:
    def __init__(self, row):
        self.id = int(row[0])
        self.src = int(row[1])
        self.dst = int(row[2])
        self.cost = int(row[3])
        self.order = ""

    def printInfo(self):
        print self.id, self.src, self.dst, self.cost
