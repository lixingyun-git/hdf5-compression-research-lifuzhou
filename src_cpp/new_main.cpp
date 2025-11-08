#include <gflags/gflags.h>
#include "change_filter.h"
#include <iostream>


namespace fs = std::filesystem;
using std::cerr;
using std::cout;
using std::endl;

DEFINE_string(source_file, "source_file.h5", "源文件");
DEFINE_string(dest_file, "dest_file.h5", "压缩后内容保存文件");
DEFINE_string(filter, "GZIP", "GZIP LZ4 NO等");
DEFINE_int32(compress_level, 6, "压缩等级");
DEFINE_bool(verbose, false, "是否输出详细信息");

int main(int argc, char* argv[])
{
    try
    {
        gflags::ParseCommandLineFlags(&argc, &argv, true);

        if (fs::exists(FLAGS_dest_file))
        {
            cerr << "\n压缩后内容保存文件不能是已存在的文件 " << FLAGS_dest_file << endl;
            return 0;
        }

        cout << "source_file " << FLAGS_source_file << endl
            << "dest_file " << FLAGS_dest_file << endl
            << "filter " << FLAGS_filter << endl;

        // 创建修改器实例
        HDF5CompressionModifier modifier(FLAGS_source_file, FLAGS_dest_file, FLAGS_verbose);
        modifier.datasetLimit = 5; // 测试代码，正式发布不使用，所以没有添加命令行参数

        modifier.run(FLAGS_filter, FLAGS_compress_level);

        cout << "filter " << FLAGS_filter
            << "compress_level_" << FLAGS_compress_level
            << " cost " << modifier.cost_ms.count() << "ms"
            << " file_size:" << getFileSize(FLAGS_dest_file) / 1024 << "K" << endl;
    }
    catch (const std::exception& e)
    {
        cerr << "程序执行失败: " << e.what() << endl;
        return 1;
    }

    return 0;
}
