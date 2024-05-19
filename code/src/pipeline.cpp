#include"pipeline.h"
#ifndef _USE_DECOMPOSER_UNIT_
int TileSeekerWorkClock = 0;
int TileDecomposer32WorkClock = 0;
int TileDecomposer16WorkClock = 0;
int TileDecomposer8WorkClock = 0;
int TileDecomposer4WorkClock = 0;
int InsideTileDecomposerWorkClock = 0;
int MinTileRasterWorkClock = 0;
int TotalClock = 0;
cPipeline::cPipeline(Triangle _tri,std::ofstream& _OutFile):TriangleInfo(_tri),ValidTileFifo_s32(LatencyUnitSize),ValidTileFifo_s16(LatencyUnitSize),ValidTileFifo_s8(LatencyUnitSize),ValidTileFifo_s4(LatencyUnitSize),ValidTileFifo_s2(LatencyUnitSize),InsideTileFifo(LatencyUnitSize),MaskTileFifo1(LatencyUnitSize),MaskTileFifo2(LatencyUnitSize),TileSeeker(TriangleInfo,ValidTileFifo_s32,InsideTileFifo),
TileDecomposer_s32(32,TriangleInfo,InsideTileFifo,ValidTileFifo_s32,ValidTileFifo_s16),
TileDecomposer_s16(16,TriangleInfo,InsideTileFifo,ValidTileFifo_s16,ValidTileFifo_s8),
TileDecomposer_s8(8,TriangleInfo,InsideTileFifo,ValidTileFifo_s8,ValidTileFifo_s4),
TileDecomposer_s4(4,TriangleInfo,InsideTileFifo,ValidTileFifo_s4,ValidTileFifo_s2),
InsideTileDecomposer(InsideTileFifo,MaskTileFifo1),
MinTileRaster(TriangleInfo,ValidTileFifo_s2,MaskTileFifo2),isFinished(false),OutFile(_OutFile)
{

}

void cPipeline::RunCycle(std::vector<cMaskTile>& OutputMask){
    if(isFinished){return;}
    int clock_cycle = ClockCycle.GetClock();
    TileSeeker.RunCycle(clock_cycle);
    TileDecomposer_s32.RunCycle(clock_cycle);
    TileDecomposer_s16.RunCycle(clock_cycle);
    TileDecomposer_s8.RunCycle(clock_cycle);
    TileDecomposer_s4.RunCycle(clock_cycle);
    InsideTileDecomposer.RunCycle(clock_cycle);
    MinTileRaster.RunCycle(clock_cycle);
    while(MaskTileFifo1.CanPop(clock_cycle))
    {
        OutputMask.push_back(MaskTileFifo1.pop());
        // MaskTileFifo1.pop();
    }
    while(MaskTileFifo2.CanPop(clock_cycle))
    {

        OutputMask.push_back(MaskTileFifo2.pop());
    }
    if(TileSeeker.isTileSeekingFinished() && ValidTileFifo_s32.Empty() && ValidTileFifo_s16.Empty() && ValidTileFifo_s8.Empty() && ValidTileFifo_s4.Empty() &&  MaskTileFifo1.Empty() && MaskTileFifo2.Empty() && InsideTileFifo.Empty() && TileDecomposer_s32.CommitQueue.empty() && TileDecomposer_s16.CommitQueue.empty() && TileDecomposer_s8.CommitQueue.empty() && TileDecomposer_s4.CommitQueue.empty() && !InsideTileDecomposer.isCacheTileValid && !MinTileRaster.isMaskTileCacheValid){
        isFinished = true;
        // OutFile<<"Task,Start Clock,End Clock"<<std::endl;
        TotalClock+=clock_cycle;
        TileSeekerWorkClock += TileSeeker.PerformanceCounter.GetWorkClockSum();
        TileDecomposer32WorkClock += TileDecomposer_s32.PerformanceCounter.GetWorkClockSum();
        TileDecomposer16WorkClock += TileDecomposer_s16.PerformanceCounter.GetWorkClockSum();
        TileDecomposer8WorkClock += TileDecomposer_s8.PerformanceCounter.GetWorkClockSum();
        TileDecomposer4WorkClock += TileDecomposer_s4.PerformanceCounter.GetWorkClockSum();
        InsideTileDecomposerWorkClock += InsideTileDecomposer.PerformanceCounter.GetWorkClockSum();
        MinTileRasterWorkClock += MinTileRaster.PerformanceCounter.GetWorkClockSum();
        TileSeeker.PerformanceCounter.OutputToCsv("TileSeeker",OutFile);
        TileDecomposer_s32.PerformanceCounter.OutputToCsv("TileDecomposer_s32",OutFile);
        TileDecomposer_s16.PerformanceCounter.OutputToCsv("TileDecomposer_s16",OutFile);
        TileDecomposer_s8.PerformanceCounter.OutputToCsv("TileDecomposer_s8",OutFile);
        TileDecomposer_s4.PerformanceCounter.OutputToCsv("TileDecomposer_s4",OutFile);
        InsideTileDecomposer.PerformanceCounter.OutputToCsv("InsideTileDecomposer",OutFile);
        MinTileRaster.PerformanceCounter.OutputToCsv("MinTileRaster",OutFile);
        OutFile.close();
    }
    ClockCycle.Tick();
}

bool cPipeline::isPipelineFinished()
{
    return isFinished;
}
#else 
int TileSeekerWorkClock = 0;
int DecomposerUnitWorkClock[MaxDispatchUnitCount];
int InsideTileDecomposerWorkClock = 0;
int MinTileRasterWorkClock = 0;
int TotalClock = 0;
int DispatchWorkClock = 0;

int x32TileFifoTileCount = 0;
int x16TileFifoTileCount = 0;
int x8TileFifoTileCount = 0;
int x4TileFifoTileCount = 0;
int x2TileFifoTileCount = 0;
int x32TileWaitClock = 0;
int x16TileWaitClock = 0;
int x8TileWaitClock = 0;
int x4TileWaitClock = 0;
int x2TileWaitClock = 0;
int DecomposerUnitInFifoTileCount[4] = {0,0,0,0};
int DecomposerUnitInFifoWaitClock[4] = {0,0,0,0};
int MinRasterMaskTileFifoCount = 0;
int MinRasterMaskTileFifoWaitClock = 0;
int InsideDecomposerTileCount = 0;
int InsideDecomposerWaitClock = 0;
int InsideMaskCount = 0;
int InsideMaskWaitClock = 0;
cPipeline::cPipeline(Triangle _tri,std::ofstream& _OutFile):TriangleInfo(_tri),ValidTileFifo_s32(LatencyUnitSize),ValidTileFifo_s16(LatencyUnitSize),ValidTileFifo_s8(LatencyUnitSize),ValidTileFifo_s4(LatencyUnitSize),ValidTileFifo_s2(LatencyUnitSize),InsideTileFifo(LatencyUnitSize),MaskTileFifo1(LatencyUnitSize),MaskTileFifo2(LatencyUnitSize),TileSeeker(TriangleInfo,ValidTileFifo_s32,InsideTileFifo),
DispatchUnit(TriangleInfo,InsideTileFifo,ValidTileFifo_s32,ValidTileFifo_s16,ValidTileFifo_s8,ValidTileFifo_s4,ValidTileFifo_s2),
InsideTileDecomposer(InsideTileFifo,MaskTileFifo1),
MinTileRaster(TriangleInfo,ValidTileFifo_s2,MaskTileFifo2),isFinished(false),OutFile(_OutFile)
{

}

void cPipeline::RunCycle(std::vector<cMaskTile>& OutputMask){
    if(isFinished){return;}
    int clock_cycle = ClockCycle.GetClock();
    TileSeeker.RunCycle(clock_cycle);
    DispatchUnit.RunCycle(clock_cycle);
    InsideTileDecomposer.RunCycle(clock_cycle);
    MinTileRaster.RunCycle(clock_cycle);
    for(int i=0;i < 16; ++i)
    {
        if(MaskTileFifo1.CanPop(clock_cycle))
        {
            OutputMask.push_back(MaskTileFifo1.pop(clock_cycle));
        }
        else if(MaskTileFifo2.CanPop(clock_cycle))
        {
            OutputMask.push_back(MaskTileFifo2.pop(clock_cycle));
        }
        else{
            break;
        }
    }
    // while(MaskTileFifo1.CanPop(clock_cycle))
    // {
    //     OutputMask.push_back(MaskTileFifo1.pop());
    //     // MaskTileFifo1.pop();
    // }
    // while(MaskTileFifo2.CanPop(clock_cycle))
    // {

    //     OutputMask.push_back(MaskTileFifo2.pop());
    //     // MaskTileFifo2.pop();
    // }
    if(TileSeeker.isTileSeekingFinished() && ValidTileFifo_s32.Empty() && ValidTileFifo_s16.Empty() && ValidTileFifo_s8.Empty() && ValidTileFifo_s4.Empty() &&  MaskTileFifo1.Empty() && MaskTileFifo2.Empty() && InsideTileFifo.Empty() && DispatchUnit.isDispatchFifosEmpty() && !InsideTileDecomposer.isCacheTileValid && !MinTileRaster.isMaskTileCacheValid && InsideTileDecomposer.isInsideTileDecomposingFinished()){
        isFinished = true;
        // OutFile<<"Task,Start Clock,End Clock"<<std::endl;
        // statistics
        x32TileFifoTileCount += this->ValidTileFifo_s32.GetElementsCount();
        x16TileFifoTileCount += this->ValidTileFifo_s16.GetElementsCount();
        x8TileFifoTileCount += this->ValidTileFifo_s16.GetElementsCount();
        x4TileFifoTileCount += this->ValidTileFifo_s16.GetElementsCount();
        x2TileFifoTileCount += this->ValidTileFifo_s16.GetElementsCount();
        x32TileWaitClock += this->ValidTileFifo_s32.GetWaitCycles();
        x16TileWaitClock += this->ValidTileFifo_s16.GetWaitCycles();
        x8TileWaitClock += this->ValidTileFifo_s8.GetWaitCycles();
        x4TileWaitClock += this->ValidTileFifo_s4.GetWaitCycles();
        x2TileWaitClock += this->ValidTileFifo_s2.GetWaitCycles();

        for(int i=0;i<4;++i)
        {
            DecomposerUnitInFifoTileCount[i] += DispatchUnit.DecomposerUnitList[i]->InValidTileFifo.GetElementsCount();
            DecomposerUnitInFifoWaitClock[i] += DispatchUnit.DecomposerUnitList[i]->InValidTileFifo.GetWaitCycles();
        }

        InsideDecomposerTileCount += this->InsideTileFifo.GetElementsCount();
        InsideDecomposerWaitClock += this->InsideTileFifo.GetWaitCycles();

        InsideMaskCount += this->MaskTileFifo1.GetElementsCount();        
        InsideMaskWaitClock += this->MaskTileFifo2.GetWaitCycles();
        
        MinRasterMaskTileFifoCount += this->MaskTileFifo2.GetElementsCount();
        MinRasterMaskTileFifoWaitClock += this->MaskTileFifo2.GetWaitCycles();

        TotalClock+=clock_cycle;
        TileSeekerWorkClock += TileSeeker.PerformanceCounter.GetWorkClockSum();
        DispatchWorkClock += DispatchUnit.PerformanceCounter.GetWorkClockSum();
        for(int i=0;i<DispatchUnitCount;++i)
        {
            DecomposerUnitWorkClock[i] += DispatchUnit.DecomposerUnitList[i]->PerformanceCounter.GetWorkClockSum();
        }
        InsideTileDecomposerWorkClock += InsideTileDecomposer.PerformanceCounter.GetWorkClockSum();
        MinTileRasterWorkClock += MinTileRaster.PerformanceCounter.GetWorkClockSum();
        TileSeeker.PerformanceCounter.OutputToCsv("TileSeeker",OutFile);
        for(int i=0;i<DispatchUnitCount;++i)
        {
            DispatchUnit.DecomposerUnitList[i]->PerformanceCounter.OutputToCsv("Decomposer Unit "+std::to_string(i),OutFile);
        }
        InsideTileDecomposer.PerformanceCounter.OutputToCsv("InsideTileDecomposer",OutFile);
        MinTileRaster.PerformanceCounter.OutputToCsv("MinTileRaster",OutFile);
        OutFile.close();
    }
    ClockCycle.Tick();
}

bool cPipeline::isPipelineFinished()
{
    return isFinished;
}

#endif