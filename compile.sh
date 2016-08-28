#编译,compile选项自己的还是SDK	0：开源代码	1：自己的代码 2：东南的代码
if [ $1 = "1" ] ; then
	./SDK-gcc-2.0/batch.sh
	cp ./SDK-gcc-2.0/bin/future_net ./
	echo "done  tar & code"
elif [ $1 = "0" ] ; then
	cd SDK-PRE-SRC/
	make
	cp future_net ./..
	cd ..
else
	cd SDK-eastSouth/
	./batch.sh
	cp ./bin/future_net ./..
	cd ..
fi
> runout.txt

#打包
if [ $1 = "0" ] ; then
	cd SDK-PRE-SRC/
	cd tar_gz/
	rm -rf *
	cp ../future_net ./
	#是否打包代码
	mkdir code
	cp ../readme.txt ../Makefile code/
	cp -r ../SRC code/
	rm code/SRC/OBJ/*
	#打包tar
	tar -zcPf future_net.tar.gz *
	rm -f ../future_net.tar.gz
	mv future_net.tar.gz ../
	echo "done  tar & code"
	cd ..
fi