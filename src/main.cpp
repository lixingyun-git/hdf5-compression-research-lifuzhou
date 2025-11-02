
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
#include <H5Cpp.h>

class HDF5CompressionModifier {
private:
    std::string input_filename;
    std::string output_filename;
    hid_t input_file_id = H5I_INVALID_HID;
    hid_t output_file_id = H5I_INVALID_HID;
    //H5::H5File output_file;

    int is_group_exists(hid_t file_id, const std::string &group_path) {
        // 调用 H5Lexists 检查链接是否存在
        herr_t status = H5Lexists(file_id, group_path.c_str(), H5P_DEFAULT);
        return (status >= 0);  // 存在返回 1，不存在返回 0
    }

    // 函数：通过文件句柄创建组（支持嵌套路径）
    hid_t create_group(hid_t file_id, const std::string &group_path) {
        // 创建组：使用默认属性列表
        hid_t group_id = H5Gcreate(
            file_id,
            group_path.c_str(),
            H5P_DEFAULT,  // 链接创建属性列表（默认）
            H5P_DEFAULT,  // 组创建属性列表（默认）
            H5P_DEFAULT   // 组访问属性列表（默认）
        );

        if (group_id < 0) {
            fprintf(stderr, "创建组失败：%s\n", group_path.c_str());
        } else {
            printf("创建组成功：%s\n", group_path.c_str());
        }
        return group_id;
    }

    hid_t open_group(hid_t loc_id, const std::string &group_path) {
        // 打开组：H5Gopen 参数为位置 ID、组路径、访问属性列表（默认）
        hid_t group_id = H5Gopen(loc_id, group_path.c_str(), H5P_DEFAULT);

        if (group_id < 0) {
            fprintf(stderr, "打开组失败：%s\n", group_path.c_str());
        } else {
            printf("打开组成功：%s（组 ID：%d）\n", group_path.c_str(), (int)group_id);
        }
        return group_id;
    }

    // 函数：在指定组中创建数据集
    hid_t create_dataset_in_group(
        hid_t group_id,         // 组 ID（数据集的父组）
        const char* dset_name,  // 数据集名称
        hid_t dtype_id,         // 数据类型
        hid_t space_id,         // 数据空间
        hid_t plist_id
    ) {
        // 创建数据集：使用默认属性列表
        hid_t dset_id = H5Dcreate(
            group_id,
            dset_name,
            dtype_id,
            space_id,
            H5P_DEFAULT,  // 链接创建属性列表
            plist_id,     // H5P_DEFAULT,  // 数据集创建属性列表（可配置压缩等）
            H5P_DEFAULT   // 数据集访问属性列表
        );

        if (dset_id < 0) {
            fprintf(stderr, "在组中创建数据集失败：%s\n", dset_name);
        } else {
            printf("在组中创建数据集成功：%s（数据集 ID：%d）\n", dset_name, (int)dset_id);
        }
        return dset_id;
    }
    
public:
    HDF5CompressionModifier(const std::string& in_filename, const std::string& out_filename) : input_filename(in_filename), output_filename(out_filename) {}

    bool openFile() {
        input_file_id = H5Fopen(input_filename.c_str(), H5F_ACC_RDONLY, H5P_DEFAULT);
        if (input_file_id < 0) {
            std::cerr << "无法打开HDF5文件: " << input_filename << std::endl;
            return false;
        }

        output_file_id = H5Fcreate(output_filename.c_str(), H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
        // output_file = H5::H5File(output_filename, H5F_ACC_TRUNC);
        if (output_file_id < 0) {
            std::cerr << "无法打开HDF5文件: " << output_filename << std::endl;
            return false;
        }

        return true;
    }

    void closeFile() {
        if (input_file_id >= 0) {
            H5Fclose(input_file_id);
            input_file_id = -1;
        }
    }

    static herr_t visitCallback(hid_t loc_id, const char* name, 
                          const H5O_info_t* info, void* operator_data) {
        std::vector<std::string>* objectList = 
            static_cast<std::vector<std::string>*>(operator_data);
        
        std::string fullPath = name;
        std::string typeStr;
        
        switch (info->type) {
            case H5O_TYPE_GROUP:
                typeStr = "Group";
                break;
            case H5O_TYPE_DATASET:
                typeStr = "Dataset";
                break;
            case H5O_TYPE_NAMED_DATATYPE:
                typeStr = "Datatype";
                break;
            default:
                typeStr = "Unknown";
        }
        
        std::string objectInfo = fullPath + " (" + typeStr + ")";
        objectList->push_back(objectInfo);
        
        return 0;
    }
    
    std::vector<std::string> exploreFileStructure() {
        std::vector<std::string> objectList;
        
        herr_t status = H5Ovisit(input_file_id, H5_INDEX_NAME, H5_ITER_NATIVE, 
                          visitCallback, &objectList, H5O_INFO_ALL);
        
        if (status < 0) {
            std::cerr << "遍历HDF5文件结构失败" << std::endl;
        }
        
        return objectList;
    }
    
    // 读取HDF5文件数据
    bool read_hdf5_data(const std::string& dataset_path, std::vector<int>& data, 
                         hsize_t& data_size, int& data_rank) {
        hid_t dataset_id = H5I_INVALID_HID;
        hid_t dataspace_id = H5I_INVALID_HID;
        
        try {
            // 打开数据集
            dataset_id = H5Dopen(input_file_id, dataset_path.c_str(), H5P_DEFAULT);
            if (dataset_id < 0) {
                throw std::runtime_error("read_hdf5_data 无法打开数据集: " + dataset_path);
            }
            
            std::cout << "step 1" << std::endl;
            // 获取数据空间
            dataspace_id = H5Dget_space(dataset_id);
            data_rank = H5Sget_simple_extent_ndims(dataspace_id);
            
            std::cout << "step 2" << std::endl;
            // 获取数据维度
            std::vector<hsize_t> dims(data_rank);
            H5Sget_simple_extent_dims(dataspace_id, dims.data(), NULL);
            
            std::cout << "step 3" << std::endl;
            // 计算总数据大小
            data_size = 1;
            for (int i = 0; i < data_rank; ++i) {
                data_size *= dims[i];
            }
            
            std::cout << "step 4" << std::endl;
            // 读取数据
            data.resize(data_size);
            herr_t status = H5Dread(dataset_id, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, data.data());
            if (status < 0) {
                throw std::runtime_error("读取数据失败");
            }
            
            std::cout << "step 5" << std::endl;
            // 获取原始压缩信息
            hid_t orig_plist = H5Dget_create_plist(dataset_id);
            int num_filters = H5Pget_nfilters(orig_plist);
            
            std::cout << "成功读取数据集: " << dataset_path << std::endl;
            std::cout << "数据维度: " << data_rank << "D, 大小: " << data_size << " 个元素" << std::endl;
            std::cout << "原始压缩过滤器数量: " << num_filters << std::endl;
            
            // 清理资源
            H5Pclose(orig_plist);
            H5Sclose(dataspace_id);
            H5Dclose(dataset_id);
            
            std::cout << "step 6" << std::endl;
            return true;
            
        } catch (const std::exception& e) {
            std::cerr << "读取数据时发生错误: " << e.what() << std::endl;
            if (dataspace_id >= 0) H5Sclose(dataspace_id);
            if (dataset_id >= 0) H5Dclose(dataset_id);
            return false;
        }
    }

    void create_path(const std::string& path_and_file, std::string& path, std::string& file) {
        std::vector<std::string> parts;
        size_t pos = 0;
        while ((pos = path_and_file.find('/', pos + 1)) != std::string::npos) {
            parts.push_back(path_and_file.substr(0, pos));
        }

        for (const auto& part : parts) {
            //if (!output_file_id.exists(part)) {
            if (is_group_exists(output_file_id, part)) {
                //output_file_id.createGroup(part);
                create_group(output_file_id, part);
            }
        }
        path = parts.back();
        file = path_and_file.substr(path.size()+1);
    }

    
    // 使用新的压缩算法保存数据
    bool save_with_new_compression(const std::string &dataset_path,
                                   const std::vector<int>& data,
                                   const std::vector<hsize_t>& dims,
                                   const std::string& compression_type,
                                   int compression_level = 6) {
        hid_t dataset_id = H5I_INVALID_HID;
        hid_t dataspace_id = H5I_INVALID_HID;
        hid_t plist_id = H5I_INVALID_HID;
        
        try {
            // 创建数据空间
            int data_rank = dims.size();
            dataspace_id = H5Screate_simple(data_rank, dims.data(), NULL);
            if (dataspace_id < 0) {
                throw std::runtime_error("无法创建数据空间");
            }
            
            // 创建数据集创建属性列表
            plist_id = H5Pcreate(H5P_DATASET_CREATE);
            if (plist_id < 0) {
                throw std::runtime_error("无法创建属性列表");
            }
            
            // 设置分块
            std::vector<hsize_t> chunk_dims(data_rank);
            for (int i = 0; i < data_rank; ++i) {
                chunk_dims[i] = std::min(dims[i], static_cast<hsize_t>(1000));
            }
            H5Pset_chunk(plist_id, data_rank, chunk_dims.data());
            
            // 设置新的压缩过滤器
            if (compression_type == "GZIP") {
                if (compression_level < 0 || compression_level > 9) {
                    std::cerr << "警告: GZIP压缩级别调整到有效范围: 6" << std::endl;
                    compression_level = 6;
                }
                H5Pset_deflate(plist_id, compression_level);
                
            } /*else if (compression_type == "LZ4") {
                unsigned int filter_info;
                if (H5Zfilter_avail(H5Z_FILTER_LZ4)) {
                    H5Zget_filter_info(H5Z_FILTER_LZ4, &filter_info);
                    if (filter_info & H5Z_FILTER_CONFIG_ENCODE_ENABLED) {
                        H5Pset_filter(plist_id, H5Z_FILTER_LZ4, H5Z_FLAG_OPTIONAL, 0, NULL);
                    } else {
                        std::cerr << "警告: LZ4过滤器不可用，使用GZIP替代" << std::endl;
                        H5Pset_deflate(plist_id, 6);
                    }
                } else {
                    std::cerr << "警告: LZ4过滤器不可用，使用GZIP替代" << std::endl;
                    H5Pset_deflate(plist_id, 6);
                }
                
            } /*else if (compression_type == "ZSTD") {
                unsigned int filter_info;
                if (H5Zfilter_avail(H5Z_FILTER_ZSTD)) {
                    H5Zget_filter_info(H5Z_FILTER_ZSTD, &filter_info);
                    if (filter_info & H5Z_FILTER_CONFIG_ENCODE_ENABLED) {
                        H5Pset_filter(plist_id, H5Z_FILTER_ZSTD, H5Z_FLAG_OPTIONAL, 1, &compression_level);
                    } else {
                        std::cerr << "警告: ZSTD过滤器不可用，使用GZIP替代" << std::endl;
                        H5Pset_deflate(plist_id, 6);
                    }
                } else {
                    std::cerr << "警告: ZSTD过滤器不可用，使用GZIP替代" << std::endl;
                    H5Pset_deflate(plist_id, 6);
                }
                
            } else if (compression_type == "BLOSC") {
                unsigned int filter_info;
                if (H5Zfilter_avail(H5Z_FILTER_BLOSC)) {
                    H5Zget_filter_info(H5Z_FILTER_BLOSC, &filter_info);
                    if (filter_info & H5Z_FILTER_CONFIG_ENCODE_ENABLED) {
                    unsigned int cd_values[7] = {0};
                    cd_values[0] = 1;  // BloscLZ压缩器
                    cd_values[1] = compression_level;
                    cd_values[2] = 1;  // shuffle过滤器
                    cd_values[3] = sizeof(float); // 数据类型大小
                    H5Pset_filter(plist_id, H5Z_FILTER_BLOSC, H5Z_FLAG_OPTIONAL, 7, cd_values);
                } else {
                    std::cerr << "警告: Blosc过滤器不可用，使用GZIP替代" << std::endl;
                    H5Pset_deflate(plist_id, 6);
                }
                } else {
                    std::cerr << "警告: Blosc过滤器不可用，使用GZIP替代" << std::endl;
                    H5Pset_deflate(plist_id, 6);
                }
                
            } else {
                throw std::runtime_error("不支持的压缩类型: " + compression_type);
            }*/
            
            // 创建数据集
            std::string dataset_name = "modified_data";
            dataset_name = dataset_path;
            std::cout << "create dataset:" << dataset_name << std::endl;
            std::string group_path;
            std::string dataset_file;
            create_path(dataset_path, group_path, dataset_file);
            dataset_name = dataset_file;
            //H5::Group group = output_file.openGroup(group_path);
            auto group_id = open_group(output_file_id, group_path);


            //dataset_id = H5Dcreate2(output_file_id, dataset_name.c_str(), H5T_NATIVE_FLOAT,
            //dataset_id = H5Dcreate2(group_id, dataset_name.c_str(), H5T_NATIVE_INT,
            //                          dataspace_id, H5P_DEFAULT, plist_id, H5P_DEFAULT);
            dataset_id = create_dataset_in_group(group_id, dataset_name.c_str(), H5T_NATIVE_INT, dataspace_id, plist_id);
            if (dataset_id < 0) {
                throw std::runtime_error("无法创建数据集");
            }
        
            // 写入数据
            herr_t write_status = H5Dwrite(dataset_id, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL, H5P_DEFAULT, data.data());
            if (write_status < 0) {
                throw std::runtime_error("写入数据失败");
            }
            
            // 获取压缩统计信息
            hsize_t storage_size = H5Dget_storage_size(dataset_id);

            double original_size = data.size() * sizeof(float);
            double compression_ratio = (1.0 - static_cast<double>(storage_size) / original_size) * 100.0;
            
            std::cout << "新压缩算法: " << compression_type << std::endl;
            std::cout << "压缩级别: " << compression_level << std::endl;
            std::cout << "压缩比率: " << std::fixed << std::setprecision(2) 
                      << compression_ratio << "%" << std::endl;
            
            // 清理资源
            //H5Dclose(dataset_id);
            H5Sclose(dataspace_id);
            H5Pclose(plist_id);
            
            return true;
            
        } catch (const std::exception& e) {
            std::cerr << "保存文件时发生错误: " << e.what() << std::endl;
            if (dataset_id >= 0) H5Dclose(dataset_id);
            if (dataspace_id >= 0) H5Sclose(dataspace_id);
            if (plist_id >= 0) H5Pclose(plist_id);
            return false;
        }
    }
    
    // 修改压缩算法的主函数
    bool modify_compression_algorithm(const std::string& dataset_path,
                                       const std::string& new_compression_type,
                                       int new_compression_level = 6) {
        std::vector<int> data;
        hsize_t data_size;
        int data_rank;
        
        std::cout << "开始处理HDF5文件压缩算法修改..." << std::endl;
        std::cout << "输入文件: " << input_filename << std::endl;
        std::cout << "数据集路径: " << dataset_path << std::endl;
        std::cout << "新压缩算法: " << new_compression_type << std::endl;
        
        // 读取现有数据
        if (!read_hdf5_data(dataset_path, data, data_size, data_rank)) {
            return false;
        }
        
        // 使用新压缩算法保存
        if (!save_with_new_compression(dataset_path, data, {data_size}, new_compression_type, new_compression_level)) {
            return false;
        }
        
        return true;
    }
    
    // 批量修改多个数据集的压缩算法
    void batch_modify_compression(const std::vector<std::string>& dataset_paths,
                                const std::string& new_compression_type,
                                int new_compression_level = 6) {
        std::cout << "=== HDF5批量压缩算法修改 ===" << std::endl;
        
        for (size_t i = 0; i < dataset_paths.size(); ++i) {
            std::cout << "\n处理数据集 " << (i+1) << "/" << dataset_paths.size() << std::endl;
            
            bool success = modify_compression_algorithm(dataset_paths[i],
                                                        new_compression_type,
                                                        new_compression_level);
            
            if (success) {
                std::cout << "✓ 数据集 " << dataset_paths[i] << " 修改成功" << std::endl;
            } else {
                std::cout << "✗ 数据集 " << dataset_paths[i] << " 修改失败" << std::endl;
            }
        }
    }
};

int main(int argc, char* argv[]) {
    try {
        std::cout << "HDF5压缩算法修改工具启动..." << std::endl;
        
        // 默认输入文件
        std::string input_file = "test_compression.h5";
        std::string output_file = "test_compression_output.h5";
        
        // 如果命令行参数指定了输入文件
        if (argc > 1) {
            input_file = argv[1];
        }
        if (argc > 2) {
            output_file = argv[2];
        }
        
        // 创建修改器实例
        HDF5CompressionModifier modifier(input_file, output_file);

        modifier.openFile();
        
        // 批量修改多个数据集的压缩算法
        std::cout << "\n--- 批量压缩算法修改 ---" << std::endl;
        
        std::vector<std::string> datasets;
        auto objects = modifier.exploreFileStructure();
        datasets.reserve(objects.size());

        int limit = 5;
        for (const auto& obj : objects) {
            //if (datasets.size() >= limit) break;
            if (obj.find("(Dataset)") != std::string::npos) {
                size_t pos = obj.find(" (Dataset)");
                std::string datasetName = obj.substr(0, pos);
                datasets.push_back(datasetName);
            }
        }
        
        modifier.batch_modify_compression(
            datasets,
            "GZIP",
            6
        );

        modifier.closeFile();
        
        std::cout << "\n=== 所有压缩算法修改任务完成 ===" << std::endl;
        std::cout << "输出文件已保存到当前目录" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "程序执行失败: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
