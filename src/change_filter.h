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
#include <filesystem>
#include <sys/stat.h>
#include <climits>

// 最好不要在头文件添加这样的用法，在cpp文件中使用
using std::cerr;
using std::cout;
using std::endl;
using std::string;
using std::vector;

class HDF5CompressionModifierTest;

namespace
{
    // 通过stat结构体获得文件大小，单位字节
    size_t getFileSize(const string& file_name)
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
    HDF5CompressionModifier(const string &source_file, const string &dest_file, bool is_verbose);
    ~HDF5CompressionModifier();
    void run(const string& filter, int compress_level);

private:
    friend HDF5CompressionModifierTest;
    bool openFile();
    void closeFile();

    // 批量修改多个数据集的压缩算法
    bool batchModify(const string& filterType, int compressionLevel = 6);
    bool getDatasets();
    // 修改压缩算法的主函数
    bool modifyCompression(const string& datasetPath, const string& filterType, int new_compressionLevel = 6);

    // 读取HDF5文件数据
    bool readHdf5Data(const string& datasetPath, vector<int>& data, hsize_t& dataSize, int& dataRank);

    // 使用新的压缩算法保存数据
    bool saveWithNewCompression(const string& datasetPath,
        const vector<int>& data,
        const vector<hsize_t>& dims,
        const string& compressionType,
        unsigned int compressionLevel = 6);
    void SetNewFilter(hid_t plistId, const string& compressionType, unsigned int compressionLevel);

    inline int isGroupExists(const hid_t fileId, const string& groupPath)
    {
        // 调用 H5Lexists 检查链接是否存在
        herr_t status = H5Lexists(fileId, groupPath.c_str(), H5P_DEFAULT);
        cout << "isGroupExists status: " << status << endl;
        return (status == 1); // 存在返回 1，不存在返回 0
    }
    // 函数：通过文件句柄创建组（支持嵌套路径）
    hid_t createGroup(const hid_t fileId, const string& groupPath);
    hid_t openGroup(const hid_t locId, const string& groupPath);
    // 函数：在指定组中创建数据集
    hid_t createDatasetInGroup(
        const hid_t groupId,   // 组 ID（数据集的父组）
        const string dsetName, // 数据集名称
        const hid_t dtypeId,   // 数据类型
        const hid_t spaceId,   // 数据空间
        const hid_t plistId);

    //herr_t visitCallback(hid_t locId, const char* name, const H5O_info_t* info, void* operatorData);
    bool exploreFileStructure(vector<string> &objectList);
    void createPath(const string& path_and_file, string& path, string& file);

    string inputFile;
    string outputFile;
    hid_t inputFileId = H5I_INVALID_HID;
    hid_t outputFileId = H5I_INVALID_HID;
    vector<string> datasets;
    bool verbose = false;

    const int H5Z_FILTER_LZ4 = 32004;
    const int H5Z_FILTER_ZSTD = 32015;
    const int H5Z_FILTER_BLOSC = 32001;
    const int H5Z_FILTER_VBZ = 32020;

public:
    int datasetLimit = INT_MAX;       // 用于测试，限制处理的dataset数量
    std::chrono::milliseconds cost_ms; // 压缩耗时 ms
};
