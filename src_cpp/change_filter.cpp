#include "change_filter.h"
#include <iostream>

using std::cerr;
using std::cout;
using std::endl;
using std::string;
using std::vector;

HDF5CompressionModifier::HDF5CompressionModifier(const string &source_file, const string &dest_file, bool is_verbose) : inputFile(source_file.c_str(),H5F_ACC_RDONLY), outputFile(dest_file.c_str(), H5F_ACC_TRUNC), verbose(is_verbose) {}
void HDF5CompressionModifier::run(const string& filter, int compress_level)
{
    auto m_start = std::chrono::steady_clock::now();
    batchModify(filter, compress_level);
    auto m_end = std::chrono::steady_clock::now();
    cost_ms = std::chrono::duration_cast<std::chrono::milliseconds>(m_end - m_start);
}

// 批量修改多个数据集的压缩算法
bool HDF5CompressionModifier::batchModify(const string& filterType, int compressionLevel)
{
    getDatasets();

    for (size_t i = 0; i < datasets.size(); ++i)
    {
        if (verbose)
            cout << "\n处理数据集 " << (i + 1) << "/" << datasets.size() << endl;

        bool success = modifyCompression(datasets[i],
            filterType,
            compressionLevel);

        if (success)
        {
            if (verbose)
                cout << "✓ 数据集 " << datasets[i] << " 修改成功" << endl;
        }
        else
        {
            cout << "✗ 数据集 " << datasets[i] << " 修改失败" << endl;
            return false;
        }
    }

    return true;
}

// 通过dataset ID获取完整路径名
string HDF5CompressionModifier::getDatasetName(hid_t datasetId) {
    // 获取名称长度
    ssize_t nameLength = H5Iget_name(datasetId, NULL, 0);
    if (nameLength < 0) {
        cerr << "无法获取名称长度" << endl;
        return "";
    }
    
    // 分配缓冲区并获取名称
    vector<char> buffer(nameLength + 1);
    ssize_t result = H5Iget_name(datasetId, buffer.data(), buffer.size());
    if (result < 0) {
        cerr << "无法获取数据集名称" << endl;
        return "";
    }
    
    return string(buffer.data());
}

void HDF5CompressionModifier::getDatasets()
{
    //inputfile.openGroup("/");
    cout << "1 id:" << inputFile.getId() << " H5I_INVALID_HID " << H5I_INVALID_HID << endl;
    ssize_t count = inputFile.getObjCount(H5F_OBJ_DATASET);
    cout << "2 id:" << inputFile.getId() << endl;
    cout << "count:" << count << endl;
    datasets.resize(count);
    inputFile.getObjIDs(H5F_OBJ_DATASET, count, datasets.data());

    if (datasets.size() >= datasetLimit)
        datasets.resize(datasetLimit);
}

// 修改压缩算法的主函数
bool HDF5CompressionModifier::modifyCompression(const hid_t datasetId,
    const string& filterType,
    int new_compressionLevel)
{
    vector<int> data;
    hsize_t dataSize;
    int dataRank;
    string datasetPath = getDatasetName(datasetId);

    if (verbose)
        cout << "数据集路径: " << datasetPath << endl;

    // 读取现有数据
    // if (!readHdf5Data(datasetId, data, dataSize, dataRank))
    // {
    //     return false;
    // }
    H5::DataSet dataset(datasetId);

    // 使用新压缩算法保存
    if (!saveWithNewCompression(datasetPath, dataset, filterType, new_compressionLevel))
    {
        return false;
    }

    return true;
}

// 读取HDF5文件数据
bool HDF5CompressionModifier::readHdf5Data(const hid_t datasetId, vector<int>& data,
    hsize_t& dataSize, int& dataRank)
{
    hid_t dataspaceId = H5I_INVALID_HID;

    try
    {
        // 获取数据空间
        dataspaceId = H5Dget_space(datasetId);
        dataRank = H5Sget_simple_extent_ndims(dataspaceId);

        // 获取数据维度
        vector<hsize_t> dims(dataRank);
        H5Sget_simple_extent_dims(dataspaceId, dims.data(), NULL);

        // 计算总数据大小
        dataSize = 1;
        for (int i = 0; i < dataRank; ++i)
        {
            dataSize *= dims[i];
        }

        // 读取数据
        data.resize(dataSize);
        herr_t status = H5Dread(datasetId, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, data.data());
        if (status < 0)
        {
            cerr << "读取数据失败" << endl;
            return false;
        }

        // 获取原始压缩信息
        hid_t origPlist = H5Dget_create_plist(datasetId);
        int numFilters = H5Pget_nfilters(origPlist);

        if (verbose)
        {
            cout << "成功读取数据集: " << getDatasetName(datasetId) << endl;
            cout << "数据维度: " << dataRank << "D, 大小: " << dataSize << " 个元素" << endl;
            cout << "原始压缩过滤器数量: " << numFilters << endl;
        }
        // 清理资源
        H5Pclose(origPlist);
        H5Sclose(dataspaceId);
        H5Dclose(datasetId);

        return true;
    }
    catch (const std::exception& e)
    {
        cerr << "读取数据时发生错误: " << e.what() << endl;
        if (dataspaceId >= 0)
            H5Sclose(dataspaceId);
        if (datasetId >= 0)
            H5Dclose(datasetId);
        return false;
    }
}

// 使用新的压缩算法保存数据
bool HDF5CompressionModifier::saveWithNewCompression(const string& datasetPath,
    H5::DataSet &dataset,
    const string& compressionType,
    unsigned int compressionLevel)
{
    hid_t datasetId = H5I_INVALID_HID;
    hid_t plistId = H5I_INVALID_HID;

    try
    {
        // // 创建数据空间
        // int dataRank = dims.size();
        // DataSpace dataspace = DataSpace(dataRank, dims.data(), NULL);

        // // 创建数据集创建属性列表
        // plistId = H5Pcreate(H5P_DATASET_CREATE);
        // if (plistId < 0)
        // {
        //     throw std::runtime_error("无法创建属性列表");
        // }

        // // 设置分块  //这里的1000 代表什么，理解后可能需要调整
        // vector<hsize_t> chunkDims(dataRank);
        // for (int i = 0; i < dataRank; ++i)
        // {
        //     chunkDims[i] = std::min(dims[i], static_cast<hsize_t>(1000));
        // }
        // H5Pset_chunk(plistId, dataRank, chunkDims.data());
        auto plistId = dataset.getCreatePlist().getId();
        auto dataspace = dataset.getSpace();

        SetNewFilter(plistId, compressionType, compressionLevel);

        // 创建数据集
        if (verbose)
            cout << "create dataset:" << datasetPath << endl;
        string groupPath;
        string datasetFile;
        createPath(datasetPath, groupPath, datasetFile);
        auto groupId = openGroup(outputFileId, groupPath);

        datasetId = createDatasetInGroup(groupId, datasetFile, H5T_NATIVE_INT, dataspace, plistId);
        if (datasetId < 0)
        {
            throw std::runtime_error("无法创建数据集");
        }

        vector<int> data(dataset.getInMemDataSize());
        // 写入数据
        herr_t writeStatus = H5Dwrite(datasetId, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, data.data());
        if (writeStatus < 0)
        {
            throw std::runtime_error("写入数据失败");
        }

        // 获取压缩统计信息
        hsize_t storageSize = H5Dget_storage_size(datasetId);

        double originalSize = data.size() * sizeof(float);
        double compressionRatio = (1.0 - static_cast<double>(storageSize) / originalSize) * 100.0;

        if (verbose)
            cout << "压缩比率: " << std::fixed << std::setprecision(2) << compressionRatio << "%" << endl;

        // 清理资源
        H5Dclose(datasetId);
        H5Pclose(plistId);
        H5Gclose(groupId);

        return true;
    }
    catch (const std::exception& e)
    {
        cerr << "保存文件时发生错误: " << e.what() << endl;
        if (datasetId >= 0)
            H5Dclose(datasetId);
        if (plistId >= 0)
            H5Pclose(plistId);
        return false;
    }
}

void HDF5CompressionModifier::SetNewFilter(hid_t plistId, const string& compressionType, unsigned int compressionLevel) {
    unsigned int filterInfo;
    // 设置新的压缩过滤器
    if (compressionType == "GZIP")
    {
        if (compressionLevel < 0 || compressionLevel > 9)
        {
            cerr << "警告: GZIP压缩级别调整到有效范围: 6" << endl;
            compressionLevel = 6;
        }
        H5Pset_deflate(plistId, compressionLevel);
    }
    else if (compressionType == "LZ4")
    {
        if (H5Zfilter_avail(H5Z_FILTER_LZ4))
        {
            H5Zget_filter_info(H5Z_FILTER_LZ4, &filterInfo);
            if (filterInfo & H5Z_FILTER_CONFIG_ENCODE_ENABLED)
            {
                H5Pset_filter(plistId, H5Z_FILTER_LZ4, H5Z_FLAG_OPTIONAL, 0, NULL);
            }
            else
            {
                // 和 H5Zfilter_avail 返回false时两种情况，暂时保留，后续 可以忽略这个else或者输出不同信息
                cerr << "警告: LZ4过滤器不可用，使用GZIP替代" << endl;
                H5Pset_deflate(plistId, compressionLevel);
            }
        }
        else
        {
            cerr << "警告: LZ4过滤器不可用，使用GZIP替代" << endl;
            H5Pset_deflate(plistId, 6);
        }
    }
    else if (compressionType == "ZSTD")
    {
        if (H5Zfilter_avail(H5Z_FILTER_ZSTD))
        {
            H5Zget_filter_info(H5Z_FILTER_ZSTD, &filterInfo);
            if (filterInfo & H5Z_FILTER_CONFIG_ENCODE_ENABLED)
            {
                H5Pset_filter(plistId, H5Z_FILTER_ZSTD, H5Z_FLAG_OPTIONAL, 1, &compressionLevel);
            }
            else
            {
                cerr << "警告: ZSTD过滤器不可用，使用GZIP替代" << endl;
                H5Pset_deflate(plistId, 6);
            }
        }
        else
        {
            cerr << "警告: ZSTD过滤器不可用，使用GZIP替代" << endl;
            H5Pset_deflate(plistId, 6);
        }
    }
    else if (compressionType == "BLOSC")
    {
        if (H5Zfilter_avail(H5Z_FILTER_BLOSC))
        {
            H5Zget_filter_info(H5Z_FILTER_BLOSC, &filterInfo);
            if (filterInfo & H5Z_FILTER_CONFIG_ENCODE_ENABLED)
            {
                unsigned int cd_values[7] = { 0 };
                cd_values[0] = 1; // BloscLZ压缩器
                cd_values[1] = compressionLevel;
                cd_values[2] = 1;             // shuffle过滤器
                cd_values[3] = sizeof(float); // 数据类型大小
                H5Pset_filter(plistId, H5Z_FILTER_BLOSC, H5Z_FLAG_OPTIONAL, 7, cd_values);
            }
            else
            {
                cerr << "警告: Blosc过滤器不可用，使用GZIP替代" << endl;
                H5Pset_deflate(plistId, 6);
            }
        }
        else
        {
            cerr << "警告: Blosc过滤器不可用，使用GZIP替代" << endl;
            H5Pset_deflate(plistId, 6);
        }
    }
    else if (compressionType == "VBZ")
    {
        if (H5Zfilter_avail(H5Z_FILTER_VBZ))
        {
            H5Zget_filter_info(H5Z_FILTER_VBZ, &filterInfo);
            if (filterInfo & H5Z_FILTER_CONFIG_ENCODE_ENABLED)
            {
                // 启用VBZ压缩（压缩级别可根据需求调整1-9）
                if (H5Pset_deflate(plistId, compressionLevel) < 0)
                { // 注意：VBZ插件复用了deflate的接口参数
                    throw std::runtime_error("无法设置VBZ压缩: " + compressionType);
                }

                // 强制指定压缩过滤器为VBZ（关键步骤）
                // if (H5Pset_filter(plistId, H5Z_FILTER_VBZ, H5Z_FLAG_MANDATORY, 0, nullptr) < 0) {
                if (H5Pset_filter(plistId, H5Z_FILTER_VBZ, H5Z_FLAG_OPTIONAL, 1, &compressionLevel) < 0)
                {
                    cerr << "无法设置VBZ过滤器" << endl;
                }
            }
        }
        else
        {
            cerr << "警告: VBZ过滤器不可用，使用GZIP替代" << endl;
            H5Pset_deflate(plistId, compressionLevel);
        }
    }
    else if (compressionType == "NO")
    {
        if (verbose)
            cout << "不压缩: " << compressionType << endl;
    }
    else
    {
        throw std::runtime_error("不支持的压缩类型: " + compressionType);
    }
}

// 函数：通过文件句柄创建组（支持嵌套路径）
hid_t HDF5CompressionModifier::createGroup(const hid_t fileId, const string& groupPath)
{
    // 创建组：使用默认属性列表
    hid_t groupId = H5Gcreate(
        fileId,
        groupPath.c_str(),
        H5P_DEFAULT, // 链接创建属性列表（默认）
        H5P_DEFAULT, // 组创建属性列表（默认）
        H5P_DEFAULT  // 组访问属性列表（默认）
    );

    if (groupId < 0)
    {
        cerr << "创建组失败：" << groupPath << endl;
        exit(-1);
    }
    else
    {
        if (verbose)
            cout << "创建组成功：" << groupPath << endl;
    }
    return groupId;
}

hid_t HDF5CompressionModifier::openGroup(const hid_t locId, const string& groupPath)
{
    // 打开组：H5Gopen 参数为位置 ID、组路径、访问属性列表（默认）
    hid_t groupId = H5Gopen(locId, groupPath.c_str(), H5P_DEFAULT);

    if (groupId < 0)
    {
        cerr << "打开组失败：" << groupPath << endl;
    }
    else
    {
        if (verbose)
            cout << "打开组成功：" << groupPath << "组 ID: " << groupId << endl;
    }
    return groupId;
}

// 函数：在指定组中创建数据集
hid_t HDF5CompressionModifier::createDatasetInGroup(
    const hid_t groupId,   // 组 ID（数据集的父组）
    const string dsetName, // 数据集名称
    const hid_t dtypeId,   // 数据类型
    H5::DataSpace &space,      // 数据空间
    const hid_t plistId)
{
    // 创建数据集：使用默认属性列表
    hid_t dsetId = H5Dcreate(
        groupId,
        dsetName.c_str(),
        dtypeId,
        space.getId(),
        H5P_DEFAULT, // 链接创建属性列表
        plistId,    // H5P_DEFAULT,  // 数据集创建属性列表（可配置压缩等）
        H5P_DEFAULT  // 数据集访问属性列表
    );

    if (dsetId < 0)
    {
        cerr << "在组中创建数据集失败：" << dsetName << endl;
    }
    else
    {
        if (verbose)
            cout << "在组中创建数据集成功：" << dsetName << "数据集 ID:" << dsetId << endl;
    }
    return dsetId;
}

void HDF5CompressionModifier::createPath(const string& path_and_file, string& path, string& file)
{
    vector<string> parts;
    size_t pos = 0;
    while ((pos = path_and_file.find('/', pos + 1)) != string::npos)
    {
        parts.push_back(path_and_file.substr(0, pos));
    }

    for (const auto& part : parts)
    {
        if (!isGroupExists(outputFileId, part))
        {
            createGroup(outputFileId, part);
        }
    }
    path = parts.back();
    file = path_and_file.substr(path.size() + 1);
}
