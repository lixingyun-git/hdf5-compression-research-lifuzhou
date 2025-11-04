
ROOT_PATH=/root

INCLUDE_PATH=-I/usr/include/hdf5/serial
LIB_PATH=-L/usr/lib/x86_64-linux-gnu/hdf5/serial

COMMON_PARAM=-Wfatal-errors -fpermissive -lhdf5 -lgflags ${INCLUDE_PATH} ${LIB_PATH}
#export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:${LIB_PATH}

all:
	g++ main.cpp -o main ${COMMON_PARAM}
