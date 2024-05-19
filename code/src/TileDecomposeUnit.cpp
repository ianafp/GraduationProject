#include"TileDecomposeUnit.h"


cTileDecomposerUnit::cTileDecomposerUnit(int _InValidFifoSize,cTriangleInfo _TriangleInfo,LatencyUnitFifo<cTile>& _InsideTriangleTileFifo,LatencyUnitFifo<cTile>& _OutValidTileFifo_s16,LatencyUnitFifo<cTile>& _OutValidTileFifo_s8,LatencyUnitFifo<cTile>& _OutValidTileFifo_s4,LatencyUnitFifo<cTile>& _OutValidTileFifo_s2):ConerId(e4ConerId::LEFT_TOP),isCurrentTileValid(false),TriangleInfo(_TriangleInfo),InValidTileFifo(_InValidFifoSize),InsideTriangleTileFifo(_InsideTriangleTileFifo),ValidTileFifo_s16(_OutValidTileFifo_s16),ValidTileFifo_s8(_OutValidTileFifo_s8),ValidTileFifo_s4(_OutValidTileFifo_s4),ValidTileFifo_s2(_OutValidTileFifo_s2)
{

}

bool cTileDecomposerUnit::isBusy(){
    return !InValidTileFifo.CanPush();
}

void cTileDecomposerUnit::Push(int clock_cycle,cTile tile)
{
    this->InValidTileFifo.push(std::pair(clock_cycle+TileCommitCycle,std::move(tile)));
}

void cTileDecomposerUnit::RunCycle(int clock_cycle)
{
    // commit queue commit
    if(!CommitQueue.empty())
    {
        cCommitTile CommitTile = CommitQueue.front();
        if(CommitTile.type == cTileCommitType::INSIDE_TRIANGLE)
        {
            if(this->InsideTriangleTileFifo.CanPush())
            {
                this->InsideTriangleTileFifo.push(std::make_pair(clock_cycle+TileDecomposeCycle-1+TileCommitCycle,std::move(CommitTile.tile)));
                CommitQueue.pop();
            }
        }
        else
        {
            switch(CommitTile.tile.size){
                case 16:
                    if(this->ValidTileFifo_s16.CanPush())
                    {
                        this->ValidTileFifo_s16.push(std::make_pair(clock_cycle+TileDecomposeCycle-1+TileCommitCycle,std::move(CommitTile.tile)));
                        CommitQueue.pop();
                    }
                    break;
                case 8:
                    if(this->ValidTileFifo_s8.CanPush())
                    {
                        this->ValidTileFifo_s8.push(std::make_pair(clock_cycle+TileDecomposeCycle-1+TileCommitCycle,std::move(CommitTile.tile)));
                        CommitQueue.pop();
                    }
                    break;
                case 4:
                    if(this->ValidTileFifo_s4.CanPush())
                    {
                        this->ValidTileFifo_s4.push(std::make_pair(clock_cycle+TileDecomposeCycle-1+TileCommitCycle,std::move(CommitTile.tile)));
                        CommitQueue.pop();
                    }
                    break;
                case 2:
                    if(this->ValidTileFifo_s2.CanPush())
                    {
                        this->ValidTileFifo_s2.push(std::make_pair(clock_cycle+TileDecomposeCycle-1+TileCommitCycle,std::move(CommitTile.tile)));
                        CommitQueue.pop();
                    }
                    break;
                default:break;
            }
        }
    }

    if(CommitQueue.size() >= CommitQueueCapacity)
    {
        PerformanceCounter.EndCounter(clock_cycle);
        return;
    }
    // pop tile
    if(!isCurrentTileValid)
    {
        if(InValidTileFifo.CanPop(clock_cycle))
        {
            isCurrentTileValid = true;
            CurrentTile = InValidTileFifo.pop(clock_cycle);
            ConerId = e4ConerId::LEFT_TOP;
        }
        else
        {
            PerformanceCounter.EndCounter(clock_cycle);
            return;
        }
    }
    // start performance counter
    PerformanceCounter.StartCounter(clock_cycle); 
    // chech sub tile
    cTile SubTile;
    SubTile.size = CurrentTile.size >> 1;
    switch(ConerId){
        case e4ConerId::LEFT_TOP:
            SubTile.LeftTopX = CurrentTile.LeftTopX;
            SubTile.LeftTopY = CurrentTile.LeftTopY + SubTile.size;
            ConerId = e4ConerId::RIGHT_TOP;
            break;
        case e4ConerId::RIGHT_TOP:
            SubTile.LeftTopX = CurrentTile.LeftTopX + SubTile.size;
            SubTile.LeftTopY = CurrentTile.LeftTopY + SubTile.size;
            ConerId = e4ConerId::LEFT_BOTTOM;
            break;
        case e4ConerId::LEFT_BOTTOM:
            SubTile.LeftTopX = CurrentTile.LeftTopX;
            SubTile.LeftTopY = CurrentTile.LeftTopY;
            ConerId = e4ConerId::RIGHT_BOTTOM;
            break;
        case e4ConerId::RIGHT_BOTTOM:
            SubTile.LeftTopX = CurrentTile.LeftTopX + SubTile.size;
            SubTile.LeftTopY = CurrentTile.LeftTopY;
            isCurrentTileValid = false;
            break;
        default:break;
    }


    bool isLeftTopCornerValid,isLeftBottomCornerValid,isRightTopCornerValid,isRightBottomCornerValid,isTileValid;
    // Left Top
    isTileValid = this->TriangleInfo.is4ConerValid(SubTile,isLeftTopCornerValid,isRightTopCornerValid,isLeftBottomCornerValid,isRightBottomCornerValid);
    if(isTileValid)
    {
        if(isLeftTopCornerValid&&isLeftBottomCornerValid&&isRightTopCornerValid&&isRightBottomCornerValid)
        {
            this->CommitQueue.push(cCommitTile(cTileCommitType::INSIDE_TRIANGLE,SubTile));
        }
        else
        {
            this->CommitQueue.push(cCommitTile(cTileCommitType::PARTRIAL_INSIDE_TRIANGLE,SubTile));
        }
    }
}

int  cTileDecomposerUnit::GetBufferSize()
{
    return this->InValidTileFifo.Size();
}

bool cTileDecomposerUnit::isFinished()
{
    return this->CommitQueue.empty() &&this->InValidTileFifo.Empty()&& !isCurrentTileValid;
}