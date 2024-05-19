import random
import math
screen_width = 800
screen_height = 600
resource_path = 'C:\\Users\\86173\\Documents\\code\\GraduationProject\\zjuthesis\\code\\resource\\'

def is_collinear(p0, p1, p2, p3):
    # 计算向量p0p1和p0p2的叉积，如果叉积为0，则共线
    # 向量叉积计算公式：p0p1 x p0p2 = (p1.x - p0.x) * (p2.y - p0.y) - (p1.y - p0.y) * (p2.x - p0.x)
    x1, y1 = p1[0] - p0[0], p1[1] - p0[1]
    x2, y2 = p2[0] - p0[0], p2[1] - p0[1]
    x3, y3 = p3[0] - p0[0], p3[1] - p0[1]
    cross_product = x1 * y2 - y1 * x2
    return cross_product == 0
def generate_triangle():
    # 生成一个随机的中心点
    center_x = random.randint(0, screen_width)
    center_y = random.randint(0, screen_height)
    
    # 随机生成两个角度，确保它们不互补（即不构成直线）
    angle1 = random.uniform(0, math.pi)  # 0到π之间的随机角度
    angle2 = random.uniform(0, 2 * math.pi)  # 0到2π之间的随机角度
    while angle2 < angle1 + math.pi or angle2 > angle1 + 2 * math.pi:
        angle2 = random.uniform(0, 2 * math.pi)  # 重新生成angle2，直到它与angle1不互补

    # 计算两个顶点相对于中心点的位置
    # 为了避免三角形退化成一条线段，我们可以给顶点一个合适的偏移量
    offset = random.randint(50, 150)
    point1_x = center_x + offset * math.cos(angle1)
    point1_y = center_y + offset * math.sin(angle1)
    point2_x = center_x + offset * math.cos(angle2)
    point2_y = center_y + offset * math.sin(angle2)

    # 确保点在屏幕范围内
    point1_x = max(0, min(point1_x, screen_width))
    point1_y = max(0, min(point1_y, screen_height))
    point2_x = max(0, min(point2_x, screen_width))
    point2_y = max(0, min(point2_y, screen_height))

    # 第三个点可以是中心点，或者再随机生成一个与前两个点不共线的点
    point3_x = random.randint(0, screen_width)
    point3_y = random.randint(0, screen_height)

    # 确保第三个点与前两个点不共线
    while is_collinear((center_x, center_y), (point1_x, point1_y), (point2_x, point2_y), (point3_x, point3_y)):
        point3_x = random.randint(0, screen_width)
        point3_y = random.randint(0, screen_height)

    return ((int(point1_x), int(point1_y)), (int(point2_x),int(point2_y)), (int(point3_x), int(point3_y)))
n = 100
outfile = resource_path + "RandomTriangleList.txt"
with open(outfile,"w") as TriangleList:
    TriangleList.write("{}\n".format(n))
    for i in range (n):
        p1,p2,p3 = generate_triangle()
        TriangleList.write("{} {} {} {} {} {}\n".format(p1[0],p1[1],p2[0],p2[1],p3[0],p3[1]))