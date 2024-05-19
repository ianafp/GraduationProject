import pandas as pd
from sklearn.linear_model import LinearRegression
import matplotlib.pyplot as plt
resource_path = 'C:\\Users\\86173\\Documents\\code\\GraduationProject\\zjuthesis\\code\\resource\\'
# 步骤1: 导入数据
df = pd.read_excel(resource_path+'Statistics.xlsx')

# 步骤2: 数据清洗（根据实际情况可能需要）
# 例如，检查是否有缺失值
print(df.isnull().sum())

# 步骤3: 探索性数据分析
# 查看前几行数据
print(df.head())

# 绘制柱状图来展示第五列数据与其他列的关系
df.plot(kind='bar', xlabel='Decomposer Unit Count', ylabel='Total Cycles', rot=0)
plt.show()

# 步骤4: 相关性分析
correlation = df.corr()
print(correlation)

# 步骤5: 回归分析
# 假设我们使用前四列作为特征，第五列作为目标变量
X = df[['TileSeekerClock', 'TileDispatchClock', 'TileDecomposeClock', 'Decomposer Unit Count']]
y = df['Total Cycles']

# 创建线性回归模型
model = LinearRegression()
model.fit(X, y)

# 步骤6: 模型评估
# 打印回归系数
print(model.coef_)

# 预测
predictions = model.predict(X)

# 绘制真实值与预测值
plt.scatter(y, predictions)
plt.xlabel('Actual Values')
plt.ylabel('Predicted Values')
plt.title('Actual vs Predicted Total Cycles')
plt.show()