#compile选项自己的还是SDK	0：开源代码	1：自己的代码	2:东南代码
SDK_use="1"

#NAME[0]="case-semi-yi/PPT_Manual_basic/"
#NAME[0]="case-semi-yi/all_manual_wenyingjie/"
#NAME[0]="case-semi-yi/PPT_MANUAL_CHEN/"
#NAME[0]="case-semi-yi/night/"
#NAME[0]="case-semi-yi/nanhang_case/"
#NAME[0]="case-semi-yi/dongnan/"
#NAME[0]="case-semi-yi/chengyu/"
#NAME[0]="case-semi-yi/88/"
#NAME[0]="case-semi-yi/80/"
#NAME[0]="case-semi-yi/caseyy/"
#NAME[0]="case-semi-yi/dag/"
#NAME[0]="case-semi-yi/other2/"
#NAME[0]="case-semi-yi/huaweiData/"
NAME[0]="case-semi-yi/chen/"

length=${#NAME[@]}

./compile.sh $SDK_use
for ((i=0; i<$length; i++))
do
	dir=${NAME[$i]}
	filelist=`ls $dir`
	for file in $filelist #for ((k=5; k<24; k++)) 
	do
		casedir=$dir$file
		echo ${NAME[$i]}$file" : start" >> runout.txt

		for ((j=0; j<1; j++))
		do  
			#输出答案
			topo=$casedir"/topo.csv" demand=$casedir"/demand.csv" result=$casedir"/result.csv"
			./future_net $topo  $demand $result >> runout.txt

			#检测答案合法性
			echo "********************** exam result ***********************" >> runout.txt
			exam_result="exam_result1"
			echo "  Road0   Road1  repeat  sumCost" >> runout.txt
			./exam_result1  $topo  $demand $result >> runout.txt
			echo -e "\n\n" >> runout.txt
		done
	done
done
echo  "all run over!"