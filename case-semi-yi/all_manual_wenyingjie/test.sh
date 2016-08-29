for (( i = 1 ; $i <= 11; i++ ))  ;
do
		echo "case$i"
        ./future_net ./case$i/topo.csv ./case$i/demand.csv ./case$i/result.csv
done
