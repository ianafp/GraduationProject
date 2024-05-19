#include"Tile.h"
#include"TileDecomposeUnit.h"
class cDispatchUnit{
public:
    cDispatchUnit(cTriangleInfo _TriangleInfo,LatencyUnitFifo<cTile>& _InsideTriangleTileFifo,LatencyUnitFifo<cTile>& _ValidTileFifo_s32,LatencyUnitFifo<cTile>& _OutValidTileFifo_s16,LatencyUnitFifo<cTile>& _OutValidTileFifo_s8,LatencyUnitFifo<cTile>& _OutValidTileFifo_s4,LatencyUnitFifo<cTile>& _OutValidTileFifo_s2);
    void RunCycle(int clock_cycle);
    bool isDispatchFifosEmpty();
    cPerformanceCounter PerformanceCounter;
    std::vector<cTileDecomposerUnit*> DecomposerUnitList;
private:
    int GetFreeUnitId(int clock_cycle);
    bool isCanPopFromValidTileFifo(int clock_cycle);
    bool PopValidTile(int clock_cycle,cTile& tile);
    cTriangleInfo TriangleInfo;
    LatencyUnitFifo<cTile>& InsideTriangleTileFifo;
    LatencyUnitFifo<cTile>& ValidTileFifo_s32;
    LatencyUnitFifo<cTile>& ValidTileFifo_s16;
    LatencyUnitFifo<cTile>& ValidTileFifo_s8;
    LatencyUnitFifo<cTile>& ValidTileFifo_s4;
    LatencyUnitFifo<cTile>& ValidTileFifo_s2;

};