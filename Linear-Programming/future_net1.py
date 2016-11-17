# coding:utf-8
# future_net1.py

# 专门针对http://codecraft.huawei.com/home/detail中初赛的例子
# 有向图中信息：
# 0,0,1,1
# 1,0,2,2
# 2,0,3,1
# 3,2,1,3
# 4,3,1,1
# 5,2,3,1
# 6,3,2,1
# 起点为0，终点为1，必经点为2、3：
# 0,1,2|3

from gurobipy import *

m = Model("TSP")

# cost/distance of node
c01 = 1
c02 = 2
c03 = 1
c21 = 3
c23 = 1
c31 = 1
c32 = 1

# create variables
x01 = m.addVar(vtype=GRB.BINARY)
x02 = m.addVar(vtype=GRB.BINARY)
x03 = m.addVar(vtype=GRB.BINARY)
x21 = m.addVar(vtype=GRB.BINARY)
x23 = m.addVar(vtype=GRB.BINARY)
x31 = m.addVar(vtype=GRB.BINARY)
x32 = m.addVar(vtype=GRB.BINARY)

# set objective
obj = c01*x01 + c02*x02 + c03*x03 + c21*x21 + c23*x23 + c31*x31 + c32*x32
m.setObjective(obj, GRB.MINIMIZE)

# degree constraint
m.addConstr(x01 + x02 + x03 == 1)	# out degree of start node s
m.addConstr(x21 + x31 == 1)			# in degree of end node t
m.addConstr(x02 + x32 == x21 + x23)	# must node 2: out degree == in degree
m.addConstr(x21 + x23 == 1)			# must node 2: out degree == 1
m.addConstr(x03 + x23 == x31 + x32)	# must node 3: out degree == in degree
m.addConstr(x31 + x32 == 1)			# must node 3: out degree == 1

# variable u constraint
n = 4
u0 = m.addVar(vtype=GRB.INTEGER, lb=1, ub=n)
u1 = m.addVar(vtype=GRB.INTEGER, lb=1, ub=n)
u2 = m.addVar(vtype=GRB.INTEGER, lb=1, ub=n)
u3 = m.addVar(vtype=GRB.INTEGER, lb=1, ub=n)
m.addConstr(u0 == 1)
m.addConstr(u1 == n)
m.addConstr(u0 - u1 + x01 * n <= n - 1)
m.addConstr(u0 - u2 + x02 * n <= n - 1)
m.addConstr(u0 - u3 + x03 * n <= n - 1)
m.addConstr(u2 - u1 + x21 * n <= n - 1)
m.addConstr(u2 - u3 + x23 * n <= n - 1)
m.addConstr(u3 - u1 + x31 * n <= n - 1)
m.addConstr(u3 - u2 + x32 * n <= n - 1)


# Compute optimal solution
m.optimize()

#print result
print "cost sum  =", int(m.objVal), "\n"

print "x01 =", x01.x
print "x02 =", x02.x
print "x03 =", x03.x
print "x21 =", x21.x
print "x23 =", x23.x
print "x31 =", x31.x
print "x32 =", x32.x, "\n"

print "u0 =", u0.x
print "x1 =", u1.x
print "x2 =", u2.x
print "x3 =", u3.x