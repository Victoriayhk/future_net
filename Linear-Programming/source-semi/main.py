# coding:utf-8

from tsp_solver import tsp_solver

dir = "case-semi/"

cases = [
"case00",				# 0_6_10ms(gurobi:17ms_4ms)
# "case25-self",			# 0_509_15ms(gurobi:45ms_62ms)
# "case-10by100",			# 0_56_8ms(gurobi:16ms_6ms)(无增量-:0_236_6ms)
# "case0",				# 0_756_90ms(gurobi:11s_4s)
# "from_chen/case32",		# 0_69_180ms(gurobi:1.6s_14s)(无增量-:70)
# "_case294",				# 0_2425_650ms(gurobi:283ms_146ms)
# "Spirits/case2", 		# 0_69866_980ms(gurobi:14s_11s)
# "34256e_391948862_41",	# (同Spirits/case2)
# "Manual_case/case1", 	# 1_1616_1600ms(gurobi:15s_5s)    ##### http://bbs.csdn.net/topics/391943287
# "Manual_case/case2", 	# 1_43020_110ms(gurobi:3s_3s)
# "Manual_case/case3", 	# 1_43326_110ms(gurobi:5min_5s)
# "Manual_case/case4", 	# 1_43326_110ms(gurobi:3s_3s)
# "_case192",				# 6_9031_300ms(gurobi:1s_4s)
# "topoSimple",			# (gurobi:2_25200_1300ms101_21035_3.2s_3.3s) http://bbs.csdn.net/topics/391941776
# "NEST/reX", 			# (gurobi:2_20594_3s_3ms)(已知NEST:2_20594)
# "NEST/re0", 			# 0_20646_420ms(NEST:20646)(gurobi:3s_3ms)
# "666_4700e_391948409", 	# 0_1580_130ms(gurobi:1min_3s)
# "from_tian/case2", 		# bad34(gurobi:0_95266_100s_17s)(已知tian:7_94039_1min)
# "from_tian/case2pro",  	# bad66(gurobi:0_95581_128s_36s)(已知tian:7_94377_1min)
# "case_391945014",      	# bad23(gurobi:0_95581_128s_36s)(同from_tian/case2pro是一个)
# "Spirits/case4", 		# 0_1689_120ms(gurobi:3min_3s)
# "mycase_sprits",          #(gurobi:0_1689_3min_5s)
# "other/zz",				# 0_17407_1368ms(gurobi:0_16693_30s/90s/8s_3s)(无增量+:17214)
# "mycase/case2",		# 0_6005_9000ms(gurobi:1.5min_7min)(无增量-:6028)	(topo1800)(6021)(3.7s routeCn = 0,iteration = 3)

# "from_chen/case4000",	# 0_4695_9079ms(gurobi:no_8min)(wen:0_4705_6455ms)(无增量-:4720)
# "west_north/self2",		# 0_3951_950ms(gurobi:no_7s)
# "other/hppppp",			# 0_1539_285ms(gurobi:no_10ms)
# "west_north/self1",		# 0_1829_850ms(gurobi:no_6s)
#
# "case100_391947660", 	# 50_26854_500ms(gurobi:2min_?ms)
# "DZ/up3", 				# 0_5367_9200ms(gurobi:no_?ms)(已知DZ:5354)(无增量-:5403)
# "from_tian/case1", 		# 0_21222_5900ms(gurobi:no_?ms)(无增量+:21192)
# "from_tian/case1pro", 	# 0_21923_5500ms(gurobi:no_?ms)
# "case800_391947651",	# 0_15682_9148ms(gurobi:no_?ms)(已知:15354)(无增量+:15383)
# "from_chen/case0",		# 0_1752_30ms(gurobi:no_?ms)(已知wen:0_1742_53ms)(无增量-:1785)
#  "mycase/case0",		# 0_4750_9000ms(gurobi:no_?ms)(已知:4748)(无增量-:4768)				(case2000-self)
# "mycase/case1",		# 0_4890_9000ms(gurobi:no_?ms)(已 知:4882:spfa_aug_no.srand)							(large_391948290)
# "mycase/case",		# 0_11684_9000ms(gurobi:no_?ms)(已知:11683spfa_aug_no.srand)(无增量-:11747)			(topo1888)
# "mycase/case4",		# 0_5730_9000ms(gurobi:no_?ms)(无增量-:5742)						(topo2000_391938984)
# "mycase/case5",		# 0_4802_9286ms(gurobi:no_?ms)(已知:4768)(DZ:4775)(无增量+:4773)		(up2)
# "mycase/case6",		# 0_15599_9000ms(gurobi:no_?ms)(已知:15596)(无增量-:15809)			(west_north_case0)(6s routeCn = 1,iteration = 6)
# "from_chen/case1",		# 4_2524_130ms(gurobi:no_?ms)(已知wen:4_2467_71ms)
# "from_chen/case2",		# 2_6302_1500ms(gurobi:no_?ms)
# "from_chen/case3",		# 14_17817_9023ms(gurobi:no_?ms)
# "from_chen/case4",		# 134_18047_9076ms(gurobi:no_?ms)(chen:14_18543_5352ms)
]

for case in cases:
    for i in range(1):
        tsp_solver(dir + case)
