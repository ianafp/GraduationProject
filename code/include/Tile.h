#pragma once
#include"StructDef.h"
#include"LatencyUnit.h"
#include"PointInTriangle.h"
#include"PerformanceCounter.h"
#define _USE_DECOMPOSER_UNIT_
const int TileSize = 32;
const int MinimumTileSize = 2;
const int MaxDispatchUnitCount = 32;
const double ZERO = 1e-6;
extern int TileSeekClock;
extern int TileValidCheckCycle;
extern int PointCheckCycle;
extern int TileDecomposeCycle;
extern int TileCommitCycle;
extern int CommitQueueCapacity;
extern int DispatchUnitCount;
extern int TileDecomposerUnitInBufferSize;
extern int LatencyUnitSize;
extern int DispatchCycle;
class cTile{
    public:
        int LeftTopX;
        int LeftTopY;
        int size;
        cTile() = default;
        cTile(int x,int y,int _size);
};
struct cMaskTile{
    int LeftTopX;
    int LeftTopY;
    bool Mask[MinimumTileSize][MinimumTileSize];
    cMaskTile() = default;
    cMaskTile(int x,int y);
    cMaskTile& operator= (const cMaskTile& _tile); 
};
struct cTileStatus
{
    bool TileValid;
    bool RightTileValid;
    bool LeftTileValid;
    bool TopTileValid;
    bool BottomTileValid;
    bool is4ConerValid[4];
};
struct cTileSeekerFifoData
{
    cTile tile;
    cTileStatus status;
    bool isLastTile;
    cTileSeekerFifoData(cTile _tile,cTileStatus _status,bool _isLastTile);
};
enum Direction{
    GO_RIGHT,GO_LEFT,GO_UP,GO_DOWN
};
enum e4ConerId{
    LEFT_TOP,RIGHT_TOP,LEFT_BOTTOM,RIGHT_BOTTOM
};
enum cTileCommitType{
    INSIDE_TRIANGLE,PARTRIAL_INSIDE_TRIANGLE,MINMUM_TILE
};
struct cCommitTile{
    cTileCommitType type;
    cTile tile;
    cCommitTile() = default;
    cCommitTile(cTileCommitType _type,cTile _tile);
};

class cTriangleInfo{
public:
    cTriangleInfo(Triangle _tri);
    bool is4ConerValid(cTile tile,bool& isLeftTopValid,bool& isRightTopValid,bool& isLeftBottomValid,bool& isRightBottomValid);
    bool TileCheck(cTile tile);
    bool PointCheck(Point point);

    Triangle tri;
    double bdLeft,bdRight,bdBottom,bdTop;
    Point InitPoint;
    cEdgeFunctionParameter EdgeFunctionParameter;
    int minx,midx,maxx,miny,midy,maxy;
    double slope_ab,slope_bc,slope_ca;
    bool slope_ab_valid,slope_bc_valid,slope_ca_valid;
};
bool isLineIntersectTile(cTile tile,Point p1,Point p2);
