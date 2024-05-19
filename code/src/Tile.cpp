#include"Tile.h"
#include<math.h>

int TileSeekClock = 8;
int TileValidCheckCycle = 1;
int PointCheckCycle = 1;
int TileDecomposeCycle = 4;
int TileCommitCycle = 1;
int CommitQueueCapacity = 4;
int DispatchUnitCount = 4;
int TileDecomposerUnitInBufferSize = 16;
int LatencyUnitSize = 256;
int DispatchCycle = 1;
bool isLineIntersectTile(cTile tile,Point p1,Point p2)
{
    double TileLeft = tile.LeftTopX,TileRight = tile.LeftTopX + tile.size - 1,TileTop = tile.LeftTopY + tile.size - 1,TileBottom = tile.LeftTopY ;

    if(p1.x <= TileLeft && p2.x <= TileLeft || p1.x >= TileRight && p2.x >= TileRight || p1.y <= TileBottom && p2 .y <= TileBottom || p1.y >= TileTop && p2.y >= TileTop)
    {
        // 线段在tile bdbox外面
        return false;
    }
    // 计算四个点与线段的叉积
    double product_LeftTop = crossProduct(Point(TileLeft,TileTop),p1,p2);
    double product_RightTop = crossProduct(Point(TileRight,TileTop),p1,p2);
    double product_LeftBottom = crossProduct(Point(TileLeft,TileBottom),p1,p2);
    double product_RightBottom = crossProduct(Point(TileRight,TileBottom),p1,p2);

    if(product_LeftTop >= 0 && product_RightTop >= 0 && product_RightBottom >=0 && product_LeftBottom >=0 || product_LeftTop <= 0 && product_RightTop <= 0 && product_RightBottom <=0 && product_LeftBottom <=0)
    {
        return false;
    }
    else
    {
        return true;
    }
}
void find_index(double x0,double x1,double x2,int& min,int& mid,int& max)
{
    if(x0 < x1){
        if(x0 < x2)
        {
            min = 0;
            if(x1 < x2){
                mid = 1;
                max = 2;
            }else{
                mid = 2;
                max = 1;
            }
        }
        else
        {
            min = 2;
            mid = 0;
            max = 1;
        }
    }
    else
    {
        if(x1 < x2)
        {
            min = 1;
            if(x0 < x2){
                mid = 0;
                max = 2;
            }else{
                mid = 2;
                max = 0;
            }
        }
        else
        {
            min = 2;
            mid = 1;
            max = 0;
        }
    }
}
cTriangleInfo::cTriangleInfo(Triangle _tri):tri(_tri)
{
    // 顶点排序
    // min x
    find_index(tri[0].x,tri[1].x,tri[2].x,minx,midx,maxx);
    find_index(tri[0].y,tri[1].y,tri[2].y,miny,midy,maxy);
    // 计算bounding box
    bdLeft = tri[minx].x ;
    bdRight = tri[maxx].x;
    bdTop = tri[maxy].y;
    bdBottom = tri[miny].y;
    InitPoint = tri[midy];
    
    // 计算边界方程的参数
    // E_ab
    EdgeFunctionParameter.E_ab_init = crossProduct(tri.p1,InitPoint,tri.p2);
    // E_bc
    EdgeFunctionParameter.E_bc_init = crossProduct(tri.p2,InitPoint,tri.p3);
    // E_ca
    EdgeFunctionParameter.E_ca_init = crossProduct(tri.p3,InitPoint,tri.p1);
    // dx dy
    EdgeFunctionParameter.dx_ab = tri.p2.x - tri.p1.x;
    EdgeFunctionParameter.dy_ab = tri.p2.y - tri.p1.y;
    EdgeFunctionParameter.dx_bc = tri.p3.x - tri.p2.x;
    EdgeFunctionParameter.dy_bc = tri.p3.y - tri.p2.y;
    EdgeFunctionParameter.dx_ca = tri.p1.x - tri.p3.x;
    EdgeFunctionParameter.dy_ca = tri.p1.y - tri.p3.y;

    // slope
    if(fabs(tri.p2.y-tri.p1.y)<ZERO)
    {
        slope_ab_valid = true;
        slope_ab = (tri.p2.x-tri.p1.x)/(tri.p2.y-tri.p1.y);

    }
    else
    {
        slope_ab_valid = false;
    }

    if(fabs(tri.p3.y-tri.p2.y)<ZERO)
    {
        slope_bc_valid = true;
        slope_bc = (tri.p3.x-tri.p2.x)/(tri.p3.y-tri.p2.y);

    }
    else
    {
        slope_bc_valid = false;
    }


    if(fabs(tri.p1.y-tri.p3.y)<ZERO)
    {
        slope_ca_valid = true;
        slope_ca = (tri.p1.x-tri.p3.x)/(tri.p1.y-tri.p3.y);

    }
    else
    {
        slope_ca_valid = false;
    }


}

bool cTriangleInfo::TileCheck(cTile tile)
{
    // 计算tile的bounding box
    double TileLeft = tile.LeftTopX,TileRight = tile.LeftTopX + tile.size - 1,TileTop = tile.LeftTopY+tile.size - 1,TileBottom = tile.LeftTopY;
    // 检测tile是否超出bounding box
    if(TileRight < this->bdLeft || TileLeft > this->bdRight || TileBottom > this->bdTop || TileTop < this-> bdBottom)
    {
        return false;
    }
    // // 检测四个顶点是否在三角形内
    // bool isConerValid[4];
    // // left top
    // isConerValid[0] = EdgeFunction(this->EdgeFunctionParameter,TileLeft-this->InitPoint.x+PixelCheckPositionX,TileTop-this->InitPoint.y+PixelCheckPositionY);
    // // right top
    // isConerValid[1] = EdgeFunction(this->EdgeFunctionParameter,TileRight-this->InitPoint.x+PixelCheckPositionX,TileTop-this->InitPoint.y+PixelCheckPositionY);
    // // left bottom
    // isConerValid[2] = EdgeFunction(this->EdgeFunctionParameter,TileLeft-this->InitPoint.x+PixelCheckPositionX,TileBottom-this->InitPoint.y+PixelCheckPositionY);
    // // right bottom
    // isConerValid[3] = EdgeFunction(this->EdgeFunctionParameter,TileRight-this->InitPoint.x+PixelCheckPositionX,TileBottom-this->InitPoint.y+PixelCheckPositionY);

    // // 注意以上四个点的判断过程是逻辑上并行的
    // if(isConerValid[0] == false && isConerValid[1] == false && isConerValid[2] == false && isConerValid[3] == false)
    // {
    //     return false;
    // }
    return true;
}
bool cTriangleInfo::is4ConerValid(cTile tile,bool& isLeftTopValid,bool& isRightTopValid,bool& isLeftBottomValid,bool& isRightBottomValid){
    // 计算tile的bounding box
    double TileLeft = tile.LeftTopX,TileRight = tile.LeftTopX + tile.size - 1,TileTop = tile.LeftTopY + tile.size - 1,TileBottom = tile.LeftTopY ;
    // 检测tile是否超出bounding box
    if(TileRight < this->bdLeft || TileLeft > this->bdRight || TileBottom > this->bdTop || TileTop < this-> bdBottom)
    {
        return false;
    }
    // 检测四个顶点是否在三角形内
    bool isConerValid[4];
    // left top
    isConerValid[0] = EdgeFunction(this->EdgeFunctionParameter,TileLeft-this->InitPoint.x+PixelCheckPositionX,TileTop-this->InitPoint.y+PixelCheckPositionY);
    // right top
    isConerValid[1] = EdgeFunction(this->EdgeFunctionParameter,TileRight-this->InitPoint.x+PixelCheckPositionX,TileTop-this->InitPoint.y+PixelCheckPositionY);
    // left bottom
    isConerValid[2] = EdgeFunction(this->EdgeFunctionParameter,TileLeft-this->InitPoint.x+PixelCheckPositionX,TileBottom-this->InitPoint.y+PixelCheckPositionY);
    // right bottom
    isConerValid[3] = EdgeFunction(this->EdgeFunctionParameter,TileRight-this->InitPoint.x+PixelCheckPositionX,TileBottom-this->InitPoint.y+PixelCheckPositionY);
    isLeftTopValid = isConerValid[0];
    isRightTopValid = isConerValid[1];
    isLeftBottomValid = isConerValid[2];
    isRightBottomValid = isConerValid[3];
    // 注意以上四个点的判断过程是逻辑上并行的
    if(isConerValid[0] == false && isConerValid[1] == false && isConerValid[2] == false && isConerValid[3] == false)
    {
        // 判断三角形是否在tile内部
        if(this->bdBottom >= TileBottom && this->bdTop <= TileTop && this->bdRight <= TileRight && this->bdLeft>=bdLeft ){
            return true;
        }
        // 判断三角形的边是否与tile相交
        if(isLineIntersectTile(tile,tri.p1,tri.p2)||isLineIntersectTile(tile,tri.p2,tri.p3)||isLineIntersectTile(tile,tri.p3,tri.p1))
        {
            return true;
        }
        
        return false;
    }
    return true;
}
bool cTriangleInfo::PointCheck(Point point)
{
    return EdgeFunction(this->EdgeFunctionParameter,PixelCheckPositionX+point.x-InitPoint.x,PixelCheckPositionY+point.y-InitPoint.y);
}
cMaskTile::cMaskTile(int x,int y):LeftTopX(x),LeftTopY(y){}

cCommitTile::cCommitTile(cTileCommitType _type,cTile _tile):type(_type),tile(_tile)
{

}

cMaskTile& cMaskTile:: operator = (const cMaskTile& _tile)
{
    this->LeftTopX = _tile.LeftTopX;
    this->LeftTopY = _tile.LeftTopY;
    for(int i=0;i<MinimumTileSize;++i)
    {
        for(int j = 0;j < MinimumTileSize; ++j)
        {
            this->Mask[i][j] = _tile.Mask[i][j];
        }
    }
    return *this;
}

