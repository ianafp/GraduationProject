#include"StructDef.h"
Point::Point(double _x,double _y):x(_x),y(_y){}
Point::Point():x(0),y(0){}
// 计算两个向量的叉积,p1->p2 X p1->p3
double crossProduct(Point p1, Point p2, Point p3) {
    return (p2.x - p1.x) * (p3.y - p1.y) - (p2.y - p1.y) * (p3.x - p1.x);
}
double crossProduct(Vector v1,Vector v2){
    return v1.x*v2.y-v1.y*v2.x;
}
Point Triangle::operator[](const int index)
{
    if(index < 0 || index > 2)
    {
        throw std::runtime_error("triangle vetex index error!\n");
    }
    if(index == 0)
    {
        return this->p1;
    }
    else if(index == 1)
    {
        return this->p2;
    }
    else return this->p3;
}
Point::Point(const Point& _p):x(_p.x),y(_p.y){}

Triangle::Triangle(Point _p1,Point _p2,Point _p3)
:p1(_p1),p2(_p2),p3(_p3)
{
}