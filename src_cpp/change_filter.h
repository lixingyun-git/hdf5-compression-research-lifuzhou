#include <hdf5.h>
#include <hdf5_hl.h>
#include <vector>
#include <string>
#include <chrono>
#include <cstdlib>
#include <cmath>
#include <iomanip>
#include <filesystem>
#include <sys/stat.h>
#include <climits>
#include <H5Cpp.h>

class HDF5CompressionModifierTest;

namespace
{
    // 通过stat结构体获得文件大小，单位字节
    size_t getFileSize(const std::string& file_name)
    {
        if (file_name.empty())
        {
            return 0;
        }
        // 这是一个存储文件(夹)信息的结构体，其中有文件大小和创建时间、访问时间、修改时间等
        struct stat statbuf;
        // 提供文件名字符串，获得文件属性结构体
        stat(file_name.c_str(), &statbuf);
        // 获取文件大小
        size_t filesize = statbuf.st_size;
        return filesize;
    }
}

/*
调用关系图
    run
        openFile
        batchModify
            getDatasets
                exploreFileStructure
                    visitCallback
            modifyCompression
                readHdf5Data
                saveWithNewCompression
                    SetNewFilter
                    createPath
                        isGroupExists
                        createGroup
                    openGroup
                    createDatasetInGroup
        closeFile
*/

class HDF5CompressionModifier
{
public:
    HDF5CompressionModifier(const std::string &source_file, const std::string &dest_file, bool is_verbose);
    void run(const std::string& filter, int compress_level);

private:
    friend HDF5CompressionModifierTest;

    // 批量修改多个数据集的压缩算法
    bool batchModify(const std::string& filterType, int compressionLevel = 6);
    std::string getDatasetName(hid_t datasetId);
    void getDatasets();
    // 修改压缩算法的主函数
    bool modifyCompression(const hid_t datasetId, const std::string& filterType, int new_compressionLevel = 6);

    // 读取HDF5文件数据
    bool readHdf5Data(const hid_t datasetId, std::vector<int>& data, hsize_t& dataSize, int& dataRank);

    // 使用新的压缩算法保存数据
    bool saveWithNewCompression(const std::string& datasetPath,
        H5::DataSet &dataset,
        const std::string& compressionType,
        unsigned int compressionLevel = 6);
    void SetNewFilter(hid_t plistId, const std::string& compressionType, unsigned int compressionLevel);

    inline int isGroupExists(const hid_t fileId, const std::string& groupPath)
    {
        // 调用 H5Lexists 检查链接是否存在
        herr_t status = H5Lexists(fileId, groupPath.c_str(), H5P_DEFAULT);
        return (status == 1); // 存在返回 1，不存在返回 0
    }
    // 函数：通过文件句柄创建组（支持嵌套路径）
    hid_t createGroup(const hid_t fileId, const std::string& groupPath);
    hid_t openGroup(const hid_t locId, const std::string& groupPath);
    // 函数：在指定组中创建数据集
    hid_t createDatasetInGroup(
        const hid_t groupId,   // 组 ID（数据集的父组）
        const std::string dsetName, // 数据集名称
        const hid_t dtypeId,   // 数据类型
        H5::DataSpace  &space,   // 数据空间
        const hid_t plistId);

    void createPath(const std::string& path_and_file, std::string& path, std::string& file);

    H5::H5File inputFile;
    H5::H5File outputFile;
    hid_t inputFileId = H5I_INVALID_HID;
    hid_t outputFileId = H5I_INVALID_HID;
    std::vector<hid_t> datasets;
    bool verbose = false;

    const int H5Z_FILTER_LZ4 = 32004;
    const int H5Z_FILTER_ZSTD = 32015;
    const int H5Z_FILTER_BLOSC = 32001;
    const int H5Z_FILTER_VBZ = 32020;

public:
    int datasetLimit = INT_MAX;       // 用于测试，限制处理的dataset数量
    std::chrono::milliseconds cost_ms; // 压缩耗时 ms
};
