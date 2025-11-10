import h5py
import numpy as np

# 创建HDF5文件
f = h5py.File("hdf5_file.hdf5", "w")

# 创建空数据集，指定形状和数据类型
d1 = f.create_dataset("right_datasetpath", (20,), 'i')

