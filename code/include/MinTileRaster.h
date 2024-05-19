#include"Tile.h"

class cMinTileRaster
{
public:
    cMinTileRaster(cTriangleInfo _TriangleInfo,LatencyUnitFifo<cTile>& _MinimumTileFifo,LatencyUnitFifo<cMaskTile>& _MaskTileFifo);
    void RunCycle(int clock_cycle);
    cPerformanceCounter PerformanceCounter;
    bool isMaskTileCacheValid;
private:
    cMaskTile MaskTileCache;
    cTriangleInfo TriangleInfo;
    LatencyUnitFifo<cTile>& MinimumTileFifo;
    LatencyUnitFifo<cMaskTile>& MaskTileFifo;
};