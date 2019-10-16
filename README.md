# stereo-client
stereo client



## 1 编译

### 1.1 编译工具  
1.1.1 测试过的编译器  
vs2017  
vs2019  
gcc version 5.4.0  
gcc version 7.3.1

1.1.2 cmake   
cmake version 3.14.5   
cmake version 3.15.2   
cmake-gui    
  

### 1.2 依赖

1.2.1 QT5   
测试过的版本：   
QT5.12.2   

在官网上下载QT安装包进行安装后，需要添加环境变量。   


1.2.1.1 ubuntu环境变量   
QTDIR=/home/wy/dpu/Qt5.12.2/   
PATH=$QTDIR/5.12.2/gcc_64/bin:$QTDIR/Tools/QtCreator/bin:$PATH   
MANPATH=$QTDIR/man:$MANPATH   
LD_LIBRARY_PATH=$QTDIR/5.12.2/gcc_64/lib:$LD_LIBRARY_PATH   


1.2.1.2 windows环境变量   
Path->D:\Qt\Qt5.12.2\5.12.2\msvc2017_64\bin;   

备注：   
屏蔽CMakeLists内的如下语句，那么不编译客户端，可以不依赖QT5。   
#ADD_SUBDIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/app/zscam_client)    


### 1.3 编译过程

1.3.1 windows下编译     
打开cmake-gui，选择编译器，选择源码路径和工程输出路径，参考下图进行配置。    
![cmake](docs/res/cmake_gui.png)     

依次点击“Configure”, "Generate", "Open Project"。    
打开vs后，选择ALL_BUILD右键重新生成。    
编译成功后，选择zscam_client，右键设为启动项目，点击运行。    


1.3.2 ubuntu下编译

mkdir build   
cd build   
cmake ..   
make -j8   
./bin/zscam_client    


1.3.3 交叉编译     

仅供参考，需要修改*.cmake文件。   

1.3.3.1 32位ARM   
mkdir build-zynq    
cd build-zynq     
cmake -DCMAKE_TOOLCHAIN_FILE=../mpsoc.cmake -DCMAKE_INSTALL_PREFIX=../../out ..     
make -j4    
make install   

1.3.3.2 64位ARM     
mkdir build-mpsoc    
cd build-mpsoc    
cmake -DCMAKE_TOOLCHAIN_FILE=../zscam.cmake -DCMAKE_INSTALL_PREFIX=../../out ..    
make -j4   
make install   


1.3.3.4 3531编译
export PATH=/opt/hisi-linux/x86-arm/arm-hisiv300-linux/bin:$PATH
cmake -DCMAKE_TOOLCHAIN_FILE=../3531.cmake -DCMAKE_INSTALL_PREFIX=../../out ..    
make -j4   
make install  

注意：    
出现报错：    
error while loading shared libraries: libz.so.1: cannot open shared object     

解决方法：
sudo apt-get install lib32ncurses5 lib32z1








 






