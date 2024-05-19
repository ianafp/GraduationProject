#pragma once
#include"StructDef.h"
#include<execution>
#define _PARDO_
const double PixelCheckPositionX = 0.5;
const double PixelCheckPositionY = 0.5;
bool isPointInTriangle_EdgeFunction(Point p,Triangle t);
bool isPointInTriangle_Area(Point p,Triangle t);
bool isPointInTriangle_Ray(Point p,Triangle t);

struct cEdgeFunctionParameter{
    double E_ab_init;
    double E_bc_init;
    double E_ca_init;
    double dy_ab,dy_bc,dy_ca;
    double dx_ab,dx_bc,dx_ca;
};
bool EdgeFunction(const cEdgeFunctionParameter& const_parameter,double dx_init,double dy_init);