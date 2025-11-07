#include "change_filter.h"
#include <gtest/gtest.h>

using std::cerr;
using std::cout;
using std::endl;
using std::string;
using std::vector;

class HDF5CompressionModifierTest {
public:
    bool openFile(const string& source_file, const string& dest_file) {
        HDF5CompressionModifier modifier(source_file, dest_file, true);
        return modifier.openFile();
    }

    bool closeFile(HDF5CompressionModifier &modifier) {
        modifier.closeFile();
        return modifier.inputFileId == H5I_INVALID_HID && modifier.outputFileId == H5I_INVALID_HID;
    }

    bool exploreFileStructure(HDF5CompressionModifier &modifier, bool construct_error) {
        if (construct_error) {
            modifier.inputFileId = H5I_INVALID_HID;
        }
        vector<string> objectList;
        return modifier.exploreFileStructure(objectList);
    }

    bool readHdf5Data(HDF5CompressionModifier &modifier, bool construct_error, const string& datasetPath) {
        if (construct_error) {
            modifier.inputFileId = H5I_INVALID_HID;
        }
        vector<int> data;
        hsize_t dataSize;
        int dataRank;
        return modifier.readHdf5Data(datasetPath, data, dataSize, dataRank);
    }
};

TEST(HDF5CompressionModifierTest, openFile) {
    string source_file;
    string dest_file;
    HDF5CompressionModifierTest t;
    // 文件名为空
    EXPECT_FALSE(t.openFile(source_file, dest_file));
    // source_file文件名为空
    dest_file = "dest_file.hdf5";
    EXPECT_FALSE(t.openFile(source_file, dest_file));
    // source_file文件不存在
    source_file = "no_exist_file";
    dest_file = "dest_file.hdf5";
    EXPECT_FALSE(t.openFile(source_file, dest_file));
    // source_file文件不是hdf5文件
    source_file = "not_hdf5_file";
    dest_file = "dest_file.hdf5";
    EXPECT_FALSE(t.openFile(source_file, dest_file));
    source_file = "hdf5_file.hdf5";
    dest_file = "dest_file.hdf5";
    EXPECT_TRUE(t.openFile(source_file, dest_file));
}

TEST(HDF5CompressionModifierTest, closeFile) {
    string source_file;
    string dest_file;
    source_file = "hdf5_file.hdf5";
    dest_file = "dest_file.hdf5";
    HDF5CompressionModifierTest t;
    HDF5CompressionModifier modifier(source_file, dest_file, true);
    EXPECT_TRUE(t.closeFile(modifier));
}

TEST(HDF5CompressionModifierTest, exploreFileStructure) {
    string source_file;
    string dest_file;
    source_file = "hdf5_file.hdf5";
    dest_file = "dest_file.hdf5";
    HDF5CompressionModifierTest t;
    HDF5CompressionModifier modifier(source_file, dest_file, true);
    EXPECT_TRUE(t.exploreFileStructure(modifier, false));
    EXPECT_FALSE(t.exploreFileStructure(modifier, true));
}

TEST(HDF5CompressionModifierTest, visitCallback) {
    // 暂时不需要校验
}
TEST(HDF5CompressionModifierTest, getDatasets) {
    // 暂时不需要校验
}

TEST(HDF5CompressionModifierTest, readHdf5Data) {
    string source_file;
    string dest_file;
    source_file = "hdf5_file.hdf5";
    dest_file = "dest_file.hdf5";
    string datasetPath;
    HDF5CompressionModifierTest t;
    HDF5CompressionModifier modifier(source_file, dest_file, true);
    datasetPath = "right_datasetpath";
    EXPECT_TRUE(t.readHdf5Data(modifier, false, datasetPath));
    datasetPath = "error_datasetpath";
    EXPECT_FALSE(t.readHdf5Data(modifier, true, datasetPath));
    EXPECT_FALSE(t.readHdf5Data(modifier, true, datasetPath));
}

// 其他内容也需要尝试构造各种异常校验，应该会发现一些错误

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
