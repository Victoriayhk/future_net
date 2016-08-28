1、整体指引：
  1)本工程使用了开源代码库LKH-2.0.7，下载路径为：http://www.akira.ruc.dk/~keld/research/LKH/LKH-2.0.7.tgz
  2)作者对于版权的声明：The code is distributed for research use. The author reserves all rights to the code.
  3)我们自己实现的代码：.h文件（路径code/INCLUDE/）：future_net.h read_info.h route.h write_result.h
                     .c文件（路径code/SRC/）：future_net.c read_info.c route.c exam_repeated.c write_result.c
  4)除了3)中提到的.c和.h，其余的代码均为LKH库代码。具体请看下面的"2、code的目录结构"
  4)编译方法：本工程没有使用华为官方sdk，自己编写了Makefile。操作步骤为，终端依次输入"cd code/"和"make"，会在code目录下编译产生名为future_net的可执行文件

2、code的目录结构：
code/
├── SRC/
│     ├── OBJ/		用于存放编译过程中产生的.o文件
│     ├── INCLUDE/
│     │     ├──  自己实现的.h：future_net.h read_info.h route.h write_result.h
│     │     └──  LKH库的.h：Delaunay.h GainType.h Genetic.h GeoConversion.h Hashing.h Heap.h LKH.h Segment.h Sequence.h
│     ├── Makefile
│     ├── 自己实现的.c：future_net.c read_info.c route.c exam_repeated.c write_result.c
│     └── LKH库的.c：其余.c文件均为LKH库中文件(由于文件过多就不显示了)
├── Makefile
└── readme.txt
