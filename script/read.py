import h5py

file_name = '../../hdf5-compression-bench/PBG08621_pass_6c7986d6_167483a9_0.hdf5'
file_name = 'hdf5_file.hdf5'

def print_items(file, obj):
    if isinstance(obj, h5py.Group):
        print(f"{obj} is a group")
        for k in obj.keys():
            obj1=obj[k]
            print_items(obj,obj1)
    elif isinstance(obj, h5py.Dataset):
        print(f"{obj} is a dataset")
        # 如果需要，可以进一步检查dataset的属性或数据
        #print(f"  Shape: {obj.shape}")
        #print(f"  Type: {obj.dtype}")
    else:
        print(f"{obj} is neither a group nor a dataset")
	
def show_file():
    # 打开文件
    file = h5py.File(file_name, 'r')  # 'r' 表示以只读模式打开

    for key in file.keys():
        obj=file[key]
        print_items(file, obj)

    file.close()

show_file()
