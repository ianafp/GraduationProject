#include"Tile.h"
#include<queue>
class cTileDecomposerUnit{
public:
    cTileDecomposerUnit(int _InValidFifoSize,cTriangleInfo _TriangleInfo,LatencyUnitFifo<cTile>& _InsideTriangleTileFifo,LatencyUnitFifo<cTile>& _OutValidTileFifo_s16,LatencyUnitFifo<cTile>& _OutValidTileFifo_s8,LatencyUnitFifo<cTile>& _OutValidTileFifo_s4,LatencyUnitFifo<cTile>& _OutValidTileFifo_s2);
    int GetBufferSize();
    void RunCycle(int clock_cycle);
    bool isBusy();
    bool isFinished();
    void Push(int clock_cycle,cTile tile);
    cPerformanceCounter PerformanceCounter;
    LatencyUnitFifo<cTile> InValidTileFifo;
private:
    e4ConerId ConerId;
    bool isCurrentTileValid;
    cTile CurrentTile;
    
    std::queue<cCommitTile> CommitQueue; 
    cTriangleInfo TriangleInfo;
    
    LatencyUnitFifo<cTile>& InsideTriangleTileFifo;
    LatencyUnitFifo<cTile>& ValidTileFifo_s16;
    LatencyUnitFifo<cTile>& ValidTileFifo_s8;
    LatencyUnitFifo<cTile>& ValidTileFifo_s4;
    LatencyUnitFifo<cTile>& ValidTileFifo_s2;
};