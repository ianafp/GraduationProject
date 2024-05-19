#include"DispatchUnit.h"
cDispatchUnit::cDispatchUnit(cTriangleInfo _TriangleInfo,LatencyUnitFifo<cTile>& _InsideTriangleTileFifo,LatencyUnitFifo<cTile>& _ValidTileFifo_s32,LatencyUnitFifo<cTile>& _OutValidTileFifo_s16,LatencyUnitFifo<cTile>& _OutValidTileFifo_s8,LatencyUnitFifo<cTile>& _OutValidTileFifo_s4,LatencyUnitFifo<cTile>& _OutValidTileFifo_s2):TriangleInfo(_TriangleInfo),InsideTriangleTileFifo(_InsideTriangleTileFifo),ValidTileFifo_s32(_ValidTileFifo_s32),ValidTileFifo_s16(_OutValidTileFifo_s16),ValidTileFifo_s8(_OutValidTileFifo_s8),ValidTileFifo_s4(_OutValidTileFifo_s4),ValidTileFifo_s2(_OutValidTileFifo_s2)
{
    for(int i = 0;i < DispatchUnitCount; ++i)
    {
        DecomposerUnitList.push_back(new cTileDecomposerUnit(TileDecomposerUnitInBufferSize,TriangleInfo,InsideTriangleTileFifo,ValidTileFifo_s16,ValidTileFifo_s8,ValidTileFifo_s4,ValidTileFifo_s2));
    }
}



void cDispatchUnit::RunCycle(int clock_cycle)
{
    cTile tile;
    PerformanceCounter.StartCounter(clock_cycle);
    for(int i=0;i<4;++i)
    {
        int FreeUnitId = this->GetFreeUnitId(clock_cycle);
        if(FreeUnitId >=0)
        {
            if(this->PopValidTile(clock_cycle,tile)){
                this->DecomposerUnitList[FreeUnitId]->Push(clock_cycle+DispatchCycle,tile);
            }
            else
            {
                PerformanceCounter.EndCounter(clock_cycle);
                break;   
            }
        }
        else
        {
            PerformanceCounter.EndCounter(clock_cycle);
            break;
        }
    }

    for(int i = 0; i< DispatchUnitCount; ++ i)
    {
        DecomposerUnitList[i]->RunCycle(clock_cycle);
    }
}

bool cDispatchUnit::isDispatchFifosEmpty()
{
    for(int i=0;i<DispatchUnitCount;++i)
    {
        if(!DecomposerUnitList[i]->isFinished()){
            return false;
        }
    }
    return ValidTileFifo_s32.Empty() && ValidTileFifo_s16.Empty() && ValidTileFifo_s8.Empty() && ValidTileFifo_s4.Empty() && ValidTileFifo_s2.Empty(); 
}

bool cDispatchUnit::isCanPopFromValidTileFifo(int clock_cycle)
{
    return ValidTileFifo_s32.CanPop(clock_cycle) && ValidTileFifo_s16.CanPop(clock_cycle) && ValidTileFifo_s8.CanPop(clock_cycle) && ValidTileFifo_s4.CanPop(clock_cycle); 
}

bool cDispatchUnit::PopValidTile(int clock_cycle,cTile& tile)
{
    // first from full fifo
    
    if(ValidTileFifo_s4.CanPop(clock_cycle))
    {
        tile =  ValidTileFifo_s4.pop(clock_cycle);
        return true;
    }
    if(ValidTileFifo_s8.CanPop(clock_cycle))
    {
        tile =  ValidTileFifo_s8.pop(clock_cycle);
        return true;
    }
    if(ValidTileFifo_s16.CanPop(clock_cycle))
    {
        tile =  ValidTileFifo_s16.pop(clock_cycle);
        return true;
    }
    if(ValidTileFifo_s32.CanPop(clock_cycle))
    {
        tile = ValidTileFifo_s32.pop(clock_cycle);
        return true;
    }
    return false;
}

int cDispatchUnit::GetFreeUnitId(int clock_cycle)
{
    int i,MinUnit = -1;
    int MinSize = LatencyUnitSize;
    for(i = 0;i < this->DecomposerUnitList.size();++i)
    {
        if(!DecomposerUnitList[i]->isBusy()&&DecomposerUnitList[i]->GetBufferSize() < MinSize)
        {
            MinUnit = i;
            MinSize = DecomposerUnitList[i]->GetBufferSize();
        }
    }

    return MinUnit;
}