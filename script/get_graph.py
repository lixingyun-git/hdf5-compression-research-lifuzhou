import matplotlib.pyplot as plt

def print_graph(x, y1, y2):
    ...
# 数据

    # 创建画布
    plt.figure(figsize=(10, 6))

    # 绘制多条折线
    plt.plot(x, y1, label='cost ms', color='red', marker='s', linestyle='-')
    plt.plot(x, y2, label='size K', color='green', marker='^', linestyle='--')

    # 设置标题和标签
    plt.title('comparse', fontsize=15)
    plt.xlabel('X', fontsize=12)
    plt.ylabel('Y', fontsize=12)


    # 添加图例和网格
    plt.legend()
    plt.grid(alpha=0.5)

    # 显示图形
    plt.show()



file_path = "result.txt"

def get_graph_data():
    x = []
    y1 = []
    y2 = []
# 打开文件（默认模式为 'r'，只读）
    with open(file_path, 'r', encoding='utf-8') as file:
        # 循环逐行读取
        for line in file:
            # 注意：每行末尾可能包含换行符 '\n'，可用 strip() 去除
            cleaned_line = line.strip()  # 去除首尾空白（包括换行符）
            result = cleaned_line.split()
            x.append(result[1])
            y1.append(int(result[3].split("ms")[0]))
            y2.append(int(result[4].split(":")[1].split("K")[0]))

    print(x)
    print(y1)
    print(y2)
    return x, y1, y2


if __name__ == '__main__':
    x, y1, y2 = get_graph_data()
    print_graph(x, y1, y2)

