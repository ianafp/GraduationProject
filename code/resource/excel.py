import pandas as pd
import matplotlib.pyplot as plt
resource_path = 'C:\\Users\\86173\\Documents\\code\\GraduationProject\\zjuthesis\\code\\resource\\'

# 读取Excel文件
df = pd.read_excel(resource_path+'Statistics1.xlsx')

# 提取TileSeekerClock和TileDispatchClock作为x轴和y轴的值
x = df['TileSeekerClock']
y = df[' TileDispatchClock']

# 提取Total Cycles作为z轴的值，用于颜色映射
z = df[' Total Cycles']

# 创建一个颜色图，其中x和y是坐标轴，z的值决定了颜色
plt.scatter(x, y, c=z, cmap='viridis')

# 添加颜色条以表示不同的Total Cycles值
plt.colorbar()

# 设置图表标题和坐标轴标签
plt.title('Total Cycles vs TileSeekerClock and TileDispatchClock')
plt.xlabel('TileSeekerClock')
plt.ylabel('TileDispatchClock')

# 显示图表
plt.show()