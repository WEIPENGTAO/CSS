# Linpack性能测试

注：本次实验使用了三台centos7虚拟机，每台虚拟机都分配了一个实际物理核心、两个虚拟逻辑核心。

## 1、实验目的

学习安装和运行**Linpack**性能测试

## 2、实验过程

### 2.1、安装和运行HPL基准测试

首先下载```OpenBLAS-0.3.23.tar.gz```和```hpl-2.3.tar.gz```两个压缩包，```openmpi```已在之前的实验安装完成。

#### 2.1.1、安装**OpenBLAS**

```bash
tar -xf OpenBLAS-0.3.23.tar.gz  # 解压缩OpenBLAS源代码包
cd OpenBLAS-0.3.23  # 进入解压后的OpenBLAS目录
make    # 执行配置命令
make PREFIX=/usr/local install  # 执行安装命令
```

安装完成后，**OpenBLAS**库文件将被安装到指定的目标路径，即```/usr/local```

#### 2.2.2、安装**HPL**并进行基准测试

```bash
tar -xf hpl-2.3.tar.gz  # 解压缩HPL源代码包
cd hpl-2.3  # 进入解压后的HPL目录
cp ./setup/Make.Linux_PII_FBLAS Make.Linux_PII_FBLAS    # 选择模板文件，将其复制为Make.Linux_PII_FBLAS
vim Make.Linux_PII_FBLAS  # 使用文本编辑器打开Make.Linux_PII_FBLAS文件，进行配置
make arch=Linux_PII_FBLAS   # 运行make命令进行编译，使用Linux_PII_FBLAS作为架构选项
cd ./bin/Linux_PII_FBLAS/   # 进入生成的可执行文件所在目录Linux_PII_FBLAS
mpirun --allow-run-as-root -np 4 ./xhpl > 1.txt  # 使用mpirun命令运行HPL程序，使用4个进程，并将输出重定向到1.txt文件
```

```Make.Linux_PII_FBLAS```文件需要修改的地方如下：

```vim
 70 TOPdir       = $(HOME)/hpl-2.3        # HPL顶级目录，存放HPL文件的位置
 84 MPdir        = /usr/lib64/openmpi     # OpenMPI安装目录
 86 MPlib        = $(MPdir)/lib/libmpi.so  # OpenMPI库文件路径
 95 LAdir        = /usr/local/lib         # OpenBLAS安装目录
 97 LAlib        = $(LAdir)/libopenblas.a  # OpenBLAS库文件路径
169 CC           = /usr/lib64/openmpi/bin/mpicc  # C编译器路径
171 CCFLAGS      = $(HPL_DEFS) -fomit-frame-pointer -O3 -funroll-loops -W -Wall -pthread  # C编译器标志
176 LINKER       = /usr/lib64/openmpi/bin/mpif77  # 链接器路径
```

得到的```1.txt```文件部分内容如下：

```txt
================================================================================
HPLinpack 2.3  --  High-Performance Linpack benchmark  --   December 2, 2018
Written by A. Petitet and R. Clint Whaley,  Innovative Computing Laboratory, UTK
Modified by Piotr Luszczek, Innovative Computing Laboratory, UTK
Modified by Julien Langou, University of Colorado Denver
================================================================================

An explanation of the input/output parameters follows:
T/V    : Wall time / encoded variant.
N      : The order of the coefficient matrix A.
NB     : The partitioning blocking factor.
P      : The number of process rows.
Q      : The number of process columns.
Time   : Time in seconds to solve the linear system.
Gflops : Rate of execution for solving the linear system.

The following parameter values will be used:

N      :      29       30       34       35
NB     :       1        2        3        4
PMAP   : Row-major process mapping
P      :       2        1        4
Q      :       2        4        1
PFACT  :    Left    Crout    Right
NBMIN  :       2        4
NDIV   :       2
RFACT  :    Left    Crout    Right
BCAST  :   1ring
DEPTH  :       0
SWAP   : Mix (threshold = 64)
L1     : transposed form
U      : transposed form
EQUIL  : yes
ALIGN  : 8 double precision words

--------------------------------------------------------------------------------
```

### 2.2、单机/集群系统性能调优及分析

以下实验为在集群上进行性能调优，若要在单机上，去掉```--machinefile nodes --mca btl_tcp_if_include ens33```参数即可。

```bash
cd /root/hpl-2.3/bin/Linux_PII_FBLAS
vim nodes
vim HPL.dat
mpirun --allow-run-as-root -np 4 --machinefile nodes --mca btl_tcp_if_include ens33 ./xhpl > output.txt
```

在HPL测试中，使用的参数选择与测试的结果有很大的关系。HPL中参数的设定是通过从一个配置文件HPL.dat中读取的，所以在测试前要改写HPL.dat文件，设置需要使用的各种参数，然后再开始运行测试程序。配置文件```HPL.dat```内容的结构如下：

```bash
HPLinpack benchmark input file
Innovative Computing Laboratory, University of Tennessee
HPL.out      output file name (if any)        # 输出文件名（如果有）
6            device out (6=stdout,7=stderr,file)
4            # 问题规模(N)
29 30 34 35  Ns                        # 问题大小的具体数值（Ns）
4            # NB的数量（块大小）
1 2 3 4      NBs                       # NB的具体数值（NBs）
0            PMAP process mapping (0=Row-,1=Column-major)    # PMAP进程映射（0=行主序，1=列主序）
3            # 进程网格的数量（P x Q）
2 1 4        Ps                        # 进程网格的行号（Ps）
2 4 1        Qs                        # 进程网格的列号（Qs）
16.0         threshold                 # 阈值
3            # 面板因子化的数量
0 1 2        PFACTs (0=left, 1=Crout, 2=Right)    # 面板因子化的具体数值（PFACTs）
2            # 递归停止准则的数量
2 4          NBMINs (>= 1)             # 递归停止准则的具体数值（NBMINs）
1            # 递归中面板的数量
2            NDIVs                     # NDIV的具体数值
3            # 递归面板因子化的数量
0 1 2        RFACTs (0=left, 1=Crout, 2=Right)    # 递归面板因子化的具体数值（RFACTs）
1            # 广播的数量
0            BCASTs (0=1rg,1=1rM,2=2rg,3=2rM,4=Lng,5=LnM)     # 广播的具体数值（BCASTs）
1            # 向前看的深度的数量
0            DEPTHs (>=0)              # 向前看的深度的具体数值（DEPTHs）
2            SWAP (0=bin-exch,1=long,2=mix)       # SWAP类型（0=bin-exch,1=long,2=mix）
64           swapping threshold        # 交换阈值
0            L1 in (0=transposed,1=no-transposed) form    # L1是否以转置形式存储（0=转置，1=非转置）
0            U  in (0=transposed,1=no-transposed) form    # U是否以转
```

要得到调试出高的性能，必须考虑内存大小，网络类型以及拓扑结构，调试上面的参数，直到得出最高性能。本次实验需要对以下三组参数进行设置：

```bash
2            # of problems sizes (N)
1960 2048    Ns     # 指出要计算的矩阵规格有2种，规格是1960，2048
2            # of NBs
60 80        NBs    # 指出使用2种不同的分块大小，大小为60，80
2            # of process grids (P x Q)
2 4          Ps     # 指出用2种进程组合方式
2 1          Qs     # 分别为（p＝2，q=2） 和（p＝4，q=1）
```

注：p＝2，q=2时需要的进程数是p×q＝2×2＝4，运行时```mpirun```命令行中指定的进程数必须大于等于4。

以上3组每组有两种情况，组合后一共有8种情况，将得到8个性能测试值，经过不断的调试将会得出一个最大的性能值，这就是得到的最高性能值。

以下是将会测试的参数：

```bash
The following parameter values will be used:

N      :    1960     2048 
NB     :      60       80 
PMAP   : Row-major process mapping
P      :       2        4 
Q      :       2        1 
PFACT  :    Left    Crout    Right 
NBMIN  :       2        4 
NDIV   :       2 
RFACT  :    Left    Crout    Right 
BCAST  :   1ring 
DEPTH  :       0 
SWAP   : Mix (threshold = 64)
L1     : transposed form
U      : transposed form
EQUIL  : yes
ALIGN  : 8 double precision words
```

以下是其中一个性能测试值，规格为1960，分块是60，p＝2，q=2时，运行时间为：2.96，运算速度为1.6971Gflops。PASSED代表结果符合要求。

```bash
T/V                N    NB     P     Q               Time                 Gflops
--------------------------------------------------------------------------------
WR00L2L2        1960    60     2     2               2.96             1.6971e+00
HPL_pdgesv() start time Thu May 11 23:23:10 2023

HPL_pdgesv() end time   Thu May 11 23:23:13 2023

--------------------------------------------------------------------------------
||Ax-b||_oo/(eps*(||A||_oo*||x||_oo+||b||_oo)*N)=   2.60744810e-03 ...... PASSED
```

全部结果已整理成```data.xlsx```文件，8组参数，每组得到18个测试结果。原始输出文件见```output.txt```。

以下是各参数组合得到的平均测试结果，可以看出，在```N=2048,NB=80,P=2,Q=2```时有最优秀的性能，每秒钟可以执行的浮点运算次数最高。

![平均性能表格](.\result\avg_gflops.png)

本次实验主要分析的是参数调优，可以通过调整 ```HPL.dat``` 测试中的参数配置，如网格分块的维度（P）和每个分块内的处理器数量（Q），进行测试，从而找到最佳的参数组合。
