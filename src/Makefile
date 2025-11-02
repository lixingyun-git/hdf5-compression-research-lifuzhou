
ROOT_PATH=/root

INCLUDE_PATH=-I${ROOT_PATH}/hdf5-hdf5_1.14.6/hdf5/include -I${ROOT_PATH}/hdf5-hdf5_1.14.6/c++/src
LIB_PATH=-L${ROOT_PATH}/hdf5-hdf5_1.14.6/hdf5/lib

COMMON_PARAM=-Wfatal-errors -fpermissive -lhdf5 -lhdf5_hl -lpthread -g ${INCLUDE_PATH} ${LIB_PATH}
#export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/root/install_cmake/hdf5-hdf5_1.14.6/hdf5/lib

all:
	g++ main.cpp -o main ${COMMON_PARAM}
