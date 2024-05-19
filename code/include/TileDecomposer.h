#include"Tile.h"
#include<queue>



class cTileDecomposer{
public:
    cTileDecomposer(int _CurrentSize,cTriangleInfo _TriangleInfo,LatencyUnitFifo<cTile>& _InsideTriangleTileFifo,LatencyUnitFifo<cTile>& _ValidTriangleTileFifo,LatencyUnitFifo<cTile>& _NextStageValidTileFifo);
    void RunCycle(int clock_cycle);
    bool isTileDecomposingFinished();
    cPerformanceCounter PerformanceCounter;
    std::queue<cCommitTile> CommitQueue; 
private:
    bool isFinished;
    int CurrentSize;
    cTriangleInfo TriangleInfo;
    LatencyUnitFifo<cTile>& InsideTriangleTileFifo;
    LatencyUnitFifo<cTile>& ValidTriangleTileFifo;
    LatencyUnitFifo<cTile>& NextStageValidTileFifo;
};

class cInsideTileDecomposer{
public:
    cInsideTileDecomposer(LatencyUnitFifo<cTile>& _InsideTriangleTileFifo,LatencyUnitFifo<cMaskTile>& _MaskTileFifo);
    void RunCycle(int clock_cycle);
    bool isInsideTileDecomposingFinished();
    cPerformanceCounter PerformanceCounter;
    bool isCacheTileValid; 
    
private:
    bool isFinished;
    bool isCurrentTileFinished;
    cMaskTile MaskTileCache;
    int CurrentX,CurrentY;
    cTile CurrentTile;
    LatencyUnitFifo<cTile>& InsideTriangleTileFifo;
    LatencyUnitFifo<cMaskTile>& MaskTileFifo;
    std::vector<cTile> x2TileAccumalateBuffer;
    std::vector<cTile> x4TileAccumalateBuffer;

};