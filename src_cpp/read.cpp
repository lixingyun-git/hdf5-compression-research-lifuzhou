
#include <hdf5.h>
#include <hdf5_hl.h>
#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <iomanip>
#include <gflags/gflags.h>
#include <filesystem>
#include <sys/stat.h>
#include <H5Cpp.h>

namespace fs = std::filesystem;

DEFINE_string(source_file, "source_file.h5", "源文件");
DEFINE_string(filter, "GZIP", "GZIP LZ4 NO等");
DEFINE_int32(compress_level, 6, "压缩等级");
DEFINE_bool(verbose, false, "是否输出详细信息");

using std::cout;
using std::endl;

int main(int argc, char* argv[]) {
    try {
	gflags::ParseCommandLineFlags(&argc, &argv, true);
	std::cout << "源文件 " << FLAGS_source_file << std::endl;

        auto h5file = H5::H5File(FLAGS_source_file.c_str(), H5F_ACC_RDONLY);
	
    	auto group = h5file.openGroup("/");
    	cout << "fileid:" << h5file.getId() << " H5I_INVALID_HID " << H5I_INVALID_HID << endl;
    	ssize_t datasetcount = h5file.getObjCount(H5F_OBJ_DATASET);
		ssize_t group_obj_count = group.getNumObjs();
    	cout << "group_obj_count:" << group_obj_count << endl;
    	cout << "datasetcount:" << datasetcount << endl;
    	ssize_t allcount = h5file.getObjCount(H5F_OBJ_ALL);
    	cout << "allcount:" << allcount << endl;
    	cout << "fileid:" << h5file.getId() << endl;
        
    } catch (const std::exception& e) {
        std::cerr << "程序执行失败: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
