#include"PointInTriangle.h"
// 边界方程法
bool isPointInTriangle_EdgeFunction(Point p,Triangle t)
{
    double cp1,cp2,cp3;
#ifndef _PARDO_
    cp1 = crossProduct(t.p1,t.p2,p);
    cp2 = crossProduct(t.p2,t.p3,p);
    cp3 = crossProduct(t.p3,t.p1,p);
#else
    std::vector<std::pair<double,std::vector<Point>>> list{
        std::pair<double,std::vector<Point>>(0,std::vector<Point>{t.p1,t.p2,p}),
        std::pair<double,std::vector<Point>>(0,std::vector<Point>{t.p2,t.p3,p}),
        std::pair<double,std::vector<Point>>(0,std::vector<Point>{t.p3,t.p1,p})
    };
    std::for_each(std::execution::par,list.begin(),list.end(),[](std::pair<double,std::vector<Point>>& it){
        it.first = crossProduct(it.second[0],it.second[1],it.second[2]);
    });
    cp1 = list[0].first;
    cp2 = list[1].first;
    cp3 = list[2].first;
#endif
    if((cp1<0 && cp2<0 && cp3 < 0)||(cp1>0&&cp2>0&&cp3>0))
    {
        return true;
    }
    else return false;
}
// 计算三角形的面积
double triangleArea(Triangle triangle) {
    return abs(0.5 * crossProduct(triangle.p1, triangle.p2, triangle.p3));
}

// 判断点是否在三角形内部（面积法）
bool isPointInTriangle_Area(Point point, Triangle triangle) {
    double totalArea,area1,area2,area3;
#ifndef _PARDO_
    // 计算三角形的总面积
    totalArea = triangleArea(triangle);
    // 计算点到各个顶点的子三角形的面积之和
    area1 = triangleArea({point, triangle.p1, triangle.p2});
    area2 = triangleArea({point, triangle.p2, triangle.p3});
    area3 = triangleArea({point, triangle.p1, triangle.p3});
#else
    std::vector<std::pair<double,std::vector<Point>>> list{
        std::pair<double,std::vector<Point>>(0,std::vector<Point>{triangle.p3,triangle.p1,triangle.p2}),
        std::pair<double,std::vector<Point>>(0,std::vector<Point>{point,triangle.p1,triangle.p2}),
        std::pair<double,std::vector<Point>>(0,std::vector<Point>{point,triangle.p2,triangle.p3}),
        std::pair<double,std::vector<Point>>(0,std::vector<Point>{point,triangle.p1,triangle.p3})
    };
    std::for_each(std::execution::par,list.begin(),list.end(),[](std::pair<double,std::vector<Point>>& it){
        it.first = triangleArea({it.second[0],it.second[1],it.second[2]});
    });
    totalArea = list[0].first;
    area1 = list[1].first;
    area2 = list[2].first;
    area3 = list[3].first;
#endif
    // 判断点是否在三角形内部
    return totalArea == (area1 + area2 + area3);
}

// 定义射线与边的交点计数函数
int countIntersections(Point point, Point p1, Point p2) {
    // 计算交点个数
    if ((p1.y <= point.y && p2.y > point.y) || (p1.y > point.y && p2.y <= point.y)) {
        double slope = (p2.x - p1.x) / (p2.y - p1.y);
        double intersectX = p1.x + (point.y - p1.y) * slope;
        if (point.x < intersectX) {
            return 1;
        }
    }
    return 0;
}

// 判断点是否在三角形内部（射线法）

bool isPointInTriangle_Ray(Point p,Triangle t){
    int count;
#ifndef _PARDO_
    // 统计与三角形边的交点个数
    count = countIntersections(p, t.p1, t.p2) +
                countIntersections(p, t.p2, t.p3) +
                countIntersections(p, t.p3, t.p1);
#else
    std::vector<std::pair<int,std::vector<Point>>> list{
        std::pair<int,std::vector<Point>>(0,std::vector<Point>{p,t.p1,t.p2}),
        std::pair<int,std::vector<Point>>(0,std::vector<Point>{p,t.p2,t.p3}),
        std::pair<int,std::vector<Point>>(0,std::vector<Point>{p,t.p3,t.p1})
    };
    std::for_each(std::execution::par,list.begin(),list.end(),[](std::pair<int,std::vector<Point>>& it){
        it.first = countIntersections(it.second[0],it.second[1],it.second[2]);
    });
    count = list[0].first+list[1].first+list[2].first;
#endif
    // 判断点是否在三角形内部
    return count % 2 == 1;
}

bool EdgeFunction(const cEdgeFunctionParameter& const_parameter,double dx_init,double dy_init)
{
    
    double E_ab = const_parameter.E_ab_init + dx_init*const_parameter.dy_ab-dy_init*const_parameter.dx_ab;
    double E_bc = const_parameter.E_bc_init + dx_init*const_parameter.dy_bc-dy_init*const_parameter.dx_bc;
    double E_ca = const_parameter.E_ca_init + dx_init*const_parameter.dy_ca-dy_init*const_parameter.dx_ca;
    if(E_ab >= 0 && E_bc >= 0 && E_ca >= 0 || E_ab <= 0 && E_bc <= 0 && E_ca <= 0)
    {
        return true;
    }
    else 
    {
        return false;
    }
}