#compile选项自己的还是SDK	0：开源代码	1：自己的代码 2：东南的代码
SDK_use="1"
testdir="case-semi/"

N[${#N[@]}]="case00"				# 0_6_10ms 			
N[${#N[@]}]="case25-self" 			# 0_509_15ms
N[${#N[@]}]="case-10by100"			# 0_56_8ms(无增量-:0_236_6ms)

N[${#N[@]}]="from_chen/case0"		# 0_1752_30ms(已知wen:0_1742_53ms)(无增量-:1785)
N[${#N[@]}]="case0"					# 0_756_90ms
N[${#N[@]}]="Spirits/case4" 		# 0_1689_120ms	
N[${#N[@]}]="666_4700e_391948409" 	# 0_1580_130ms
N[${#N[@]}]="from_chen/case1"		# 4_2524_130ms(已知wen:4_2467_71ms) 
N[${#N[@]}]="from_chen/case32"		# 0_69_180ms(无增量-:70)

N[${#N[@]}]="other/hppppp"			# 0_1539_285ms
N[${#N[@]}]="_case192"				# 6_9031_300ms

N[${#N[@]}]="NEST/re0" 				# 0_20646_420ms(NEST:20646)
N[${#N[@]}]="case100_391947660" 	# 50_26854_500ms
N[${#N[@]}]="_case294"				# 0_2425_650ms	
N[${#N[@]}]="Spirits/case2" 		# 0_69866_980ms
N[${#N[@]}]="34256e_391948862_41"	# 0_69866_965ms
N[${#N[@]}]="west_north/self1"		# 0_1829_850ms
N[${#N[@]}]="west_north/self2"		# 0_3951_950ms

N[${#N[@]}]="NEST/reX" 				# 2_25200_1300ms(已知NEST:2_20594)
N[${#N[@]}]="from_chen/case2"		# 2_6302_1500ms
N[${#N[@]}]="other/zz"				# 0_17407_1368ms(无增量+:17214)
N[${#N[@]}]="Manual_case/case1" 	# 1_1616_1600ms    ##### http://bbs.csdn.net/topics/391943287
N[${#N[@]}]="Manual_case/case2" 	# 1_43020_110ms
N[${#N[@]}]="Manual_case/case3" 	# 1_43326_110ms
N[${#N[@]}]="Manual_case/case4" 	# 1_43326_110ms

N[${#N[@]}]="from_tian/case1pro" 	# 0_21923_5500ms
N[${#N[@]}]="from_tian/case1" 		# 0_21222_5900ms(无增量+:21192)
	
N[${#N[@]}]="DZ/up3" 				# 0_5367_9200ms(已知DZ:5354)(无增量-:5403)
N[${#N[@]}]="from_chen/case4000"	# 0_4695_9079ms(wen:0_4705_6455ms)(无增量-:4720)
N[${#N[@]}]="case800_391947651"		# 0_15682_9148ms(已知:15354)(无增量+:15383)

N[${#N[@]}]="from_tian/case2" 		# bad34(已知tian:7_94039_1min)
N[${#N[@]}]="from_tian/case2pro"  	# bad66(已知tian:7_94377_1min)
N[${#N[@]}]="case_391945014"      	# bad23(已知:8_93125_9142ms)
N[${#N[@]}]="from_chen/case3"		# 14_17817_9023ms
N[${#N[@]}]="from_chen/case4"		# 14_18047_9076ms(chen:14_18543_5352ms)
# ########### N[${#N[@]}]="topoSimple"			# hehe...http://bbs.csdn.net/topics/391941776

N[${#N[@]}]="mycase_sprits"
N[${#N[@]}]="mycase/case0"		# 0_4750_9000ms(已知:4748)(无增量-:4768)				(case2000-self)
N[${#N[@]}]="mycase/case1"		# 0_4890_9000ms(已知:4882:spfa_aug_no.srand)							(large_391948290)
N[${#N[@]}]="mycase/case2"		# 0_6005_9000ms(无增量-:6028)						(topo1800)(6021)(3.7s routeCn = 0,iteration = 3)
N[${#N[@]}]="mycase/case3"		# 0_11684_9000ms(已知:11683spfa_aug_no.srand)(无增量-:11747)			(topo1888)
N[${#N[@]}]="mycase/case4"		# 0_5730_9000ms(无增量-:5742)						(topo2000_391938984)
N[${#N[@]}]="mycase/case5"		# 0_4802_9286ms(已知:4768)(DZ:4775)(无增量+:4773)		(up2)
N[${#N[@]}]="mycase/case6"		# 0_15599_9000ms(已知:15596)(无增量-:15809)			(west_north_case0)(6s routeCn = 1,iteration = 6)


length=${#N[@]}	#length="1"

#编译执行
./compile.sh $SDK_use
for ((i=0; i<$length; i++))
do
	casename=${N[$i]}
	echo "start :"$casename >> runout.txt
	casedir=$testdir$casename

	for ((j=0; j<1; j++))
	do  
		#输出答案
		topo=$casedir"/topo.csv" demand=$casedir"/demand.csv" result=$casedir"/result.csv"
		./future_net $topo  $demand $result >> runout.txt

		#检测答案合法性
		echo "********************** exam result ***********************" >> runout.txt
		echo "  Road0   Road1  repeat  sumCost" >> runout.txt
		./exam_result1  $topo  $demand $result  >> runout.txt #check.exam_result1
		echo "" >> runout.txt
	done

	echo "done  "$casename
	echo -e "\n\n " >> runout.txt
done