# coding:utf-8

from tsp_solver import tsp_solver

dir  = "../../case-pre/"
cases = [
# "case0", "case1", "case2", "case3", "case4",
# "ofeiguanfang_zhongji",
# "o600_4800_50",
# "case_7s340ms_weight554",
# "case-anti/case1-fix6",
# "case-anti/case2-fix6",
# "case-anti/case2-fix8",
# "case-anti/case2-fix8-2",
# "case2000-100&100-e(40000)-ultimate-20160415T202336-1951",
# "32_undirected_1",
# "pre_case4", "pre_case11", "case1000_100",
# "topoSimple"
]

for case in cases:
    tsp_solver(dir + case)
