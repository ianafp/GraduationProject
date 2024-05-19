#include"Tile.h"
class cTileSeeker{
public:
    cTileSeeker(cTriangleInfo _TriangleInfo,LatencyUnitFifo<cTile>& _TileFifo,LatencyUnitFifo<cTile>& _InsideTriangleTileFifo);
    void RunCycle(int clock_cycle);
    cTileStatus GetTileStatus(cTile tile);
    bool isTileSeekingFinished();
    void ResetTriangle(cTriangleInfo _TriangleInfo);
    cPerformanceCounter PerformanceCounter;
private:
    cTriangleInfo TriangleInfo;
    cTile CurrentTile;
    // int minx,midx,maxx,miny,midy,maxy;
    bool isTileCacheValid;
    bool isCacheTileInsideTriangle;
    cTile TileCache;
    LatencyUnitFifo<cTile>& InsideTriangleTileFifo;
    LatencyUnitFifo<cTile>& ResultTileFifo;
    bool isLeftDirectionValid,isDownDirectionValid,isTopDirectionValid,isNewRow,isFirstRow,isFinish;
    cTile LeftCachedTile,DownCachedTile,TopCachedTile;
    Direction HorizantalDirection,VerticalDirection;
};