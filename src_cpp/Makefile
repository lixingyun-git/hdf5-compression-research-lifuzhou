
ROOT_PATH=/root

INCLUDE_PATH=-I/usr/include/hdf5/serial
LIB_PATH=-L/usr/lib/x86_64-linux-gnu/hdf5/serial

INCLUDE_PATH=-I/root/hdf5-compression-research-lifuzhou/third_lib_and_plugin/include
LIB_PATH=-L/root/hdf5-compression-research-lifuzhou/third_lib_and_plugin/lib

COMMON_PARAM=-Wfatal-errors -fpermissive -lhdf5 -lgflags -lhdf5_cpp ${INCLUDE_PATH} ${LIB_PATH}
COMMON_PARAM=-Wfatal-errors -fpermissive -lhdf5_debug -lgflags -lhdf5_cpp_debug ${INCLUDE_PATH} ${LIB_PATH}
#export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:${LIB_PATH}

all:
	#g++ main.cpp -o main ${COMMON_PARAM}
	#g++ new_main.cpp change_filter.cpp -o new_main ${COMMON_PARAM}
	#g++ change_filter_test.cpp change_filter.cpp -o googletest ${COMMON_PARAM} -lgtest
	g++ read.cpp -o read ${COMMON_PARAM}
	g++ write.cpp -o write ${COMMON_PARAM}
