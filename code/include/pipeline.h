#ifndef _PIPELINE_
#define _PIPELINE_
#include"TileSeeker.h"
#include"TileDecomposer.h"
#include"MinTileRaster.h"
#include"DispatchUnit.h"
#ifndef _USE_DECOMPOSER_UNIT_
class cPipeline{
public:
    cPipeline(Triangle _tri,std::ofstream& _OutFile);
    void RunCycle(std::vector<cMaskTile>& OutputMask);
    bool isPipelineFinished();
private:
    bool isFinished;
    std::ofstream& OutFile;
    cClockCycle ClockCycle;
    cTriangleInfo TriangleInfo;
    cTileSeeker TileSeeker;


    LatencyUnitFifo<cTile> ValidTileFifo_s32;
    LatencyUnitFifo<cTile> ValidTileFifo_s16;
    LatencyUnitFifo<cTile> ValidTileFifo_s8;
    LatencyUnitFifo<cTile> ValidTileFifo_s4;
    LatencyUnitFifo<cTile> ValidTileFifo_s2;

    LatencyUnitFifo<cTile> InsideTileFifo;
    LatencyUnitFifo<cMaskTile> MaskTileFifo1;
    LatencyUnitFifo<cMaskTile> MaskTileFifo2;
    
    cTileDecomposer TileDecomposer_s32;
    cTileDecomposer TileDecomposer_s16;
    cTileDecomposer TileDecomposer_s8;
    cTileDecomposer TileDecomposer_s4;
    cInsideTileDecomposer InsideTileDecomposer;
    cMinTileRaster MinTileRaster;

};

#else
class cPipeline{
public:
    cPipeline(Triangle _tri,std::ofstream& _OutFile);
    void RunCycle(std::vector<cMaskTile>& OutputMask);
    bool isPipelineFinished();
private:
    bool isFinished;
    std::ofstream& OutFile;
    cClockCycle ClockCycle;
    cTriangleInfo TriangleInfo;
    cTileSeeker TileSeeker;


    LatencyUnitFifo<cTile> ValidTileFifo_s32;
    LatencyUnitFifo<cTile> ValidTileFifo_s16;
    LatencyUnitFifo<cTile> ValidTileFifo_s8;
    LatencyUnitFifo<cTile> ValidTileFifo_s4;
    LatencyUnitFifo<cTile> ValidTileFifo_s2;

    LatencyUnitFifo<cTile> InsideTileFifo;
    LatencyUnitFifo<cMaskTile> MaskTileFifo1;
    LatencyUnitFifo<cMaskTile> MaskTileFifo2;
    
    cDispatchUnit DispatchUnit;
    cInsideTileDecomposer InsideTileDecomposer;
    cMinTileRaster MinTileRaster;

};
#endif
#endif