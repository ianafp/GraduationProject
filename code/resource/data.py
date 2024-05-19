import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.dates as mdates
import csv
from datetime import datetime, timedelta
# 读取CSV文件中的数据


# 定义CSV文件路径
resource_path = 'C:\\Users\\86173\\Documents\\code\\GraduationProject\\zjuthesis\\code\\resource\\'



for i in range(3):

# 读取CSV文件
    data = {}
    csv_file_path = resource_path + "data" + str(i) +".csv"
    with open(csv_file_path, newline='') as csvfile:
        reader = csv.reader(csvfile)
        for row in reader:
            task_name = row[0]
            clocks = row[1:]
            data[task_name] = [(int(start), int(end)) for start, end in zip(clocks[::2], clocks[1::2])]

    # 设置时间的开始点为2023年5月1日
    start_date = datetime(2023, 5, 1)

    # 创建图形和坐标轴
    fig, ax = plt.subplots()

    # 绘制每个任务的每个时期
    for task_name, periods in data.items():
        y_pos = task_name  # 使用任务名称作为Y轴的位置
        for start, end in periods:
            # 将小时转换为datetime对象
            # start_datetime = start_date + timedelta(hours=start)
            # end_datetime = start_date + timedelta(hours=end)
            ax.barh(y_pos, end - start, left=start, height=0.5, align='center', edgecolor='black')

    # 设置X轴的格式和标签
    ax.xaxis_date()
    clock = 200
    ax.set_xlim(0, clock)  # 设置X轴的范围为一天

    # 设置X轴的刻度
    ax.set_xticks([i*int(clock/20) for i in range(20)])
    ax.set_xticklabels([i*int(clock/20) for i in range(20)])

    # 设置Y轴的标签
    ax.set_yticks(list(data.keys()))
    ax.set_yticklabels(list(data.keys()))

    # 设置网格和标题
    ax.grid(True, which='major', axis='x')
    ax.set_title('甘特图')

    # 旋转日期标签避免重叠
    plt.gcf().autofmt_xdate()

    # 显示图形
    plt.tight_layout()
    plt.savefig(resource_path+"gantt"+str(i)+".png")