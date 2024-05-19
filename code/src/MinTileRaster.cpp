#include"MinTileRaster.h"
cMinTileRaster::cMinTileRaster(cTriangleInfo _TriangleInfo,LatencyUnitFifo<cTile>& _MinimumTileFifo,LatencyUnitFifo<cMaskTile>& _MaskTileFifo):TriangleInfo(_TriangleInfo),MinimumTileFifo(_MinimumTileFifo),MaskTileFifo(_MaskTileFifo),isMaskTileCacheValid(false){}

void cMinTileRaster::RunCycle(int clock_cycle)
{
    if(isMaskTileCacheValid)
    {
        PerformanceCounter.EndCounter(clock_cycle);
        if(MaskTileFifo.CanPush())
        {
            MaskTileFifo.push(std::make_pair(clock_cycle+TileCommitCycle,std::move(MaskTileCache)));
            isMaskTileCacheValid = false;
        }
        return;
    }
    if(!this->MinimumTileFifo.CanPop(clock_cycle))
    {
        PerformanceCounter.EndCounter(clock_cycle);
        return;
    }
    cTile tile = this->MinimumTileFifo.pop(clock_cycle);
    PerformanceCounter.StartCounter(clock_cycle);
    cMaskTile MaskTile(tile.LeftTopX,tile.LeftTopY);
    // 并行检测所有测试点
    for(int i = 0;i < MinimumTileSize; ++i)
    {
        for(int j = 0;j < MinimumTileSize; ++j)
        {
            MaskTile.Mask[i][j] = this->TriangleInfo.PointCheck(Point(MaskTile.LeftTopX+j,MaskTile.LeftTopY+i));
        }
    }
    // 提交mask
    if(this->MaskTileFifo.CanPush()){
        this->MaskTileFifo.push(std::make_pair(clock_cycle+PointCheckCycle,std::move(MaskTile)));
    }
    else
    {
        this->isMaskTileCacheValid = true;
        this->MaskTileCache.LeftTopX = MaskTile.LeftTopX;
        this->MaskTileCache.LeftTopY = MaskTile.LeftTopY;
        for(int i=0;i<MinimumTileSize;++i)
        {
            for(int j = 0;j < MinimumTileSize; ++j)
            {
                MaskTileCache.Mask[i][j] = MaskTile.Mask[i][j];
            }
        }
    }
}