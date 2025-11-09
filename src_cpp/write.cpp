
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

DEFINE_string(dest_file, "dest_file.hdf5", "dest文件");
DEFINE_string(filter, "GZIP", "GZIP LZ4 NO等");
DEFINE_int32(compress_level, 6, "压缩等级");
DEFINE_bool(verbose, false, "是否输出详细信息");

using std::cout;
using std::endl;

int main(int argc, char* argv[]) {
    try {
		gflags::ParseCommandLineFlags(&argc, &argv, true);
		std::cout << "dest文件 " << FLAGS_dest_file << std::endl;

		hid_t file_id;       // 文件标识符
		hid_t dset_id;       // 数据集标识符
		hsize_t dims[1] = {100}; // 数据集的维度
		int data[100];       // 数据缓冲区
		int i;

		// 初始化数据
		for (i = 0; i < 100; i++) {
			data[i] = i;
		}

		// 创建文件
		file_id = H5Fcreate(FLAGS_dest_file.c_str(), H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
		if (file_id < 0) {
			printf("Error creating file\n");
			return -1;
		}

		// 创建数据集
        H5::DataSpace dataspace = H5::DataSpace(1, dims, NULL);
		dset_id = H5Dcreate2(file_id, "/right_datasetpath", H5T_STD_I32BE, dataspace.getId(), H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
		if (dset_id < 0) {
			printf("Error creating dataset\n");
			H5Fclose(file_id);
			return -1;
		}

		// 写入数据到数据集
		if (H5Dwrite(dset_id, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, data) < 0) {
			printf("Error writing to dataset\n");
			H5Dclose(dset_id);
			H5Fclose(file_id);
			return -1;
		}

		// 关闭数据集和文件
		H5Dclose(dset_id);
		H5Fclose(file_id);
        
    } catch (const std::exception& e) {
        std::cerr << "程序执行失败: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
