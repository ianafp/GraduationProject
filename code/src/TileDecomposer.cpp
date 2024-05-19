
#include "TileDecomposer.h"
cTileDecomposer::cTileDecomposer(int _CurrentSize, cTriangleInfo _TriangleInfo, LatencyUnitFifo<cTile> &_InsideTriangleTileFifo, LatencyUnitFifo<cTile> &_ValidTriangleTileFifo, LatencyUnitFifo<cTile> &_NextStageValidTileFifo) : CurrentSize(_CurrentSize), TriangleInfo(_TriangleInfo), InsideTriangleTileFifo(_InsideTriangleTileFifo), ValidTriangleTileFifo(_ValidTriangleTileFifo), NextStageValidTileFifo(_NextStageValidTileFifo), isFinished(false){

                                                                                                                                                                                                                                                                                                                                                                                                                                             };

void cTileDecomposer::RunCycle(int clock_cycle)
{
    if (!this->CommitQueue.empty())
    {
        // 提交缓冲区的tile
        PerformanceCounter.EndCounter(clock_cycle);
        cCommitTile CommitTile = this->CommitQueue.front();
        if (CommitTile.type == cTileCommitType::INSIDE_TRIANGLE)
        {
            if (this->InsideTriangleTileFifo.CanPush())
            {
                this->InsideTriangleTileFifo.push(std::make_pair(clock_cycle + TileCommitCycle, std::move(CommitTile.tile)));
                CommitQueue.pop();
            }
        }
        else
        {
            if (this->NextStageValidTileFifo.CanPush())
            {
                this->NextStageValidTileFifo.push(std::make_pair(clock_cycle + TileCommitCycle, std::move(CommitTile.tile)));
                CommitQueue.pop();
            }
        }
        return;
    }

    if (!this->ValidTriangleTileFifo.CanPop(clock_cycle))
    {
        PerformanceCounter.EndCounter(clock_cycle);
        return;
    }
    cTile CurrentTile = this->ValidTriangleTileFifo.pop(clock_cycle);
    PerformanceCounter.StartCounter(clock_cycle);
    int NewSize = this->CurrentSize >> 1;
    cTile LeftTopTile = cTile(CurrentTile.LeftTopX, CurrentTile.LeftTopY + NewSize, NewSize);
    cTile LeftBottomTile = cTile(CurrentTile.LeftTopX, CurrentTile.LeftTopY, NewSize);
    cTile RightTopTile = cTile(CurrentTile.LeftTopX + NewSize, CurrentTile.LeftTopY + NewSize, NewSize);
    cTile RightBottomTile = cTile(CurrentTile.LeftTopX + NewSize, CurrentTile.LeftTopY, NewSize);

    bool isLeftTopCornerValid, isLeftBottomCornerValid, isRightTopCornerValid, isRightBottomCornerValid, isTileValid;
    // Left Top
    isTileValid = this->TriangleInfo.is4ConerValid(LeftTopTile, isLeftTopCornerValid, isRightTopCornerValid, isLeftBottomCornerValid, isRightBottomCornerValid);
    if (isTileValid)
    {
        if (isLeftTopCornerValid && isLeftBottomCornerValid && isRightTopCornerValid && isRightBottomCornerValid)
        {
            if (this->InsideTriangleTileFifo.CanPush())
            {
                this->InsideTriangleTileFifo.push(std::make_pair(clock_cycle + TileDecomposeCycle, std::move(LeftTopTile)));
            }
            else
            {
                this->CommitQueue.push(cCommitTile(cTileCommitType::INSIDE_TRIANGLE, LeftTopTile));
            }
        }
        else
        {
            if (this->NextStageValidTileFifo.CanPush())
            {
                this->NextStageValidTileFifo.push(std::make_pair(clock_cycle + TileDecomposeCycle, std::move(LeftTopTile)));
            }
            else
            {
                this->CommitQueue.push(cCommitTile(cTileCommitType::PARTRIAL_INSIDE_TRIANGLE, LeftTopTile));
            }
        }
    }

    // Right Top
    isTileValid = this->TriangleInfo.is4ConerValid(RightTopTile, isLeftTopCornerValid, isRightTopCornerValid, isLeftBottomCornerValid, isRightBottomCornerValid);
    if (isTileValid)
    {
        if (isLeftTopCornerValid && isLeftBottomCornerValid && isRightTopCornerValid && isRightBottomCornerValid)
        {
            if (this->InsideTriangleTileFifo.CanPush())
            {
                this->InsideTriangleTileFifo.push(std::make_pair(clock_cycle + TileDecomposeCycle, std::move(RightTopTile)));
            }
            else
            {
                this->CommitQueue.push(cCommitTile(cTileCommitType::INSIDE_TRIANGLE, RightTopTile));
            }
        }
        else
        {
            if (this->NextStageValidTileFifo.CanPush())
            {
                this->NextStageValidTileFifo.push(std::make_pair(clock_cycle + TileDecomposeCycle, std::move(RightTopTile)));
            }
            else
            {
                this->CommitQueue.push(cCommitTile(cTileCommitType::PARTRIAL_INSIDE_TRIANGLE, RightTopTile));
            }
        }
    }

    // Left Bottom
    isTileValid = this->TriangleInfo.is4ConerValid(LeftBottomTile, isLeftTopCornerValid, isRightTopCornerValid, isLeftBottomCornerValid, isRightBottomCornerValid);
    if (isTileValid)
    {
        if (isLeftTopCornerValid && isLeftBottomCornerValid && isRightTopCornerValid && isRightBottomCornerValid)
        {
            if (this->InsideTriangleTileFifo.CanPush())
            {
                this->InsideTriangleTileFifo.push(std::make_pair(clock_cycle + TileDecomposeCycle + TileCommitCycle, std::move(LeftBottomTile)));
            }
            else
            {
                this->CommitQueue.push(cCommitTile(cTileCommitType::INSIDE_TRIANGLE, LeftBottomTile));
            }
        }
        else
        {
            if (this->NextStageValidTileFifo.CanPush())
            {
                this->NextStageValidTileFifo.push(std::make_pair(clock_cycle + TileDecomposeCycle + TileCommitCycle, std::move(LeftBottomTile)));
            }
            else
            {
                this->CommitQueue.push(cCommitTile(cTileCommitType::PARTRIAL_INSIDE_TRIANGLE, LeftBottomTile));
            }
        }
    }

    // right bottom
    isTileValid = this->TriangleInfo.is4ConerValid(RightBottomTile, isLeftTopCornerValid, isRightTopCornerValid, isLeftBottomCornerValid, isRightBottomCornerValid);
    if (isTileValid)
    {
        if (isLeftTopCornerValid && isLeftBottomCornerValid && isRightTopCornerValid && isRightBottomCornerValid)
        {
            if (this->InsideTriangleTileFifo.CanPush())
            {
                this->InsideTriangleTileFifo.push(std::make_pair(clock_cycle + TileDecomposeCycle + TileCommitCycle, std::move(RightBottomTile)));
            }
            else
            {
                this->CommitQueue.push(cCommitTile(cTileCommitType::INSIDE_TRIANGLE, RightBottomTile));
            }
        }
        else
        {
            if (this->NextStageValidTileFifo.CanPush())
            {
                this->NextStageValidTileFifo.push(std::make_pair(clock_cycle + TileDecomposeCycle + TileCommitCycle, std::move(RightBottomTile)));
            }
            else
            {
                this->CommitQueue.push(cCommitTile(cTileCommitType::PARTRIAL_INSIDE_TRIANGLE, RightBottomTile));
            }
        }
    }
}

cInsideTileDecomposer::cInsideTileDecomposer(LatencyUnitFifo<cTile> &_InsideTriangleTileFifo, LatencyUnitFifo<cMaskTile> &_MaskTileFifo) : InsideTriangleTileFifo(_InsideTriangleTileFifo), MaskTileFifo(_MaskTileFifo), isCurrentTileFinished(true), isCacheTileValid(false) {}
bool GetMaskTile(int &x, int &y, cMaskTile &MaskTile, const cTile &tile)
{

    if (y >= tile.LeftTopY + tile.size)
    {
        return false;
    }
    MaskTile.LeftTopX = x;
    MaskTile.LeftTopY = y;
    for (int i = 0; i < MinimumTileSize; ++i)
    {
        for (int j = 0; j < MinimumTileSize; ++j)
        {
            MaskTile.Mask[i][j] = true;
        }
    }

    if (x < tile.LeftTopX + tile.size)
    {
        x += MinimumTileSize;
    }
    else
    {
        x = tile.LeftTopX;
        if (y < tile.LeftTopY + tile.size)
        {
            y += MinimumTileSize;
        }
        else
        {
            return false;
        }
    }
    return true;
}
void cInsideTileDecomposer::RunCycle(int clock_cycle)
{
    cMaskTile MaskTile;
    MaskTile.Mask[0][0] = true;
    MaskTile.Mask[0][1] = true;
    MaskTile.Mask[1][0] = true;
    MaskTile.Mask[1][1] = true;
    PerformanceCounter.StartCounter(clock_cycle);
    if (isCurrentTileFinished && !this->InsideTriangleTileFifo.CanPop(clock_cycle))
    {

        if (!this->x2TileAccumalateBuffer.empty())
        {
            for (cTile &it : x2TileAccumalateBuffer)
            {
                MaskTile.LeftTopX = it.LeftTopX;
                MaskTile.LeftTopY = it.LeftTopY;
                MaskTileFifo.push(std::make_pair(clock_cycle + TileCommitCycle, std::move(MaskTile)));
            }
            x2TileAccumalateBuffer.clear();
        }
        else if (!this->x4TileAccumalateBuffer.empty())
        {
            for (cTile &it : x4TileAccumalateBuffer)
            {
                MaskTile.LeftTopX = it.LeftTopX;
                MaskTile.LeftTopY = it.LeftTopY;
                MaskTileFifo.push(std::make_pair(clock_cycle + TileCommitCycle, std::move(MaskTile)));
                MaskTile.LeftTopX = it.LeftTopX+2;
                MaskTile.LeftTopY = it.LeftTopY;
                MaskTileFifo.push(std::make_pair(clock_cycle + TileCommitCycle, std::move(MaskTile)));
                MaskTile.LeftTopX = it.LeftTopX;
                MaskTile.LeftTopY = it.LeftTopY+2;
                MaskTileFifo.push(std::make_pair(clock_cycle + TileCommitCycle, std::move(MaskTile)));
                MaskTile.LeftTopX = it.LeftTopX+2;
                MaskTile.LeftTopY = it.LeftTopY+2;
                MaskTileFifo.push(std::make_pair(clock_cycle + TileCommitCycle, std::move(MaskTile)));
            }
            x4TileAccumalateBuffer.clear();
        }
        else
        {
            PerformanceCounter.EndCounter(clock_cycle);
            return;
        }
    }
    if (this->x2TileAccumalateBuffer.size() == 16)
    {
        for (cTile &it : x2TileAccumalateBuffer)
        {
            MaskTile.LeftTopX = it.LeftTopX;
            MaskTile.LeftTopY = it.LeftTopY;
            MaskTileFifo.push(std::make_pair(clock_cycle + TileCommitCycle, std::move(MaskTile)));
        }
        x2TileAccumalateBuffer.clear();
        return;
    }
    if (this->x4TileAccumalateBuffer.size() == 4)
    {
        for (cTile &it : x4TileAccumalateBuffer)
        {
                MaskTile.LeftTopX = it.LeftTopX;
                MaskTile.LeftTopY = it.LeftTopY;
                MaskTileFifo.push(std::make_pair(clock_cycle + TileCommitCycle, std::move(MaskTile)));
                MaskTile.LeftTopX = it.LeftTopX+2;
                MaskTile.LeftTopY = it.LeftTopY;
                MaskTileFifo.push(std::make_pair(clock_cycle + TileCommitCycle, std::move(MaskTile)));
                MaskTile.LeftTopX = it.LeftTopX;
                MaskTile.LeftTopY = it.LeftTopY+2;
                MaskTileFifo.push(std::make_pair(clock_cycle + TileCommitCycle, std::move(MaskTile)));
                MaskTile.LeftTopX = it.LeftTopX+2;
                MaskTile.LeftTopY = it.LeftTopY+2;
                MaskTileFifo.push(std::make_pair(clock_cycle + TileCommitCycle, std::move(MaskTile)));
        }
        x4TileAccumalateBuffer.clear();
        return;
    }
    if (isCurrentTileFinished)
    {
        while (this->InsideTriangleTileFifo.CanPop(clock_cycle))
        {

            CurrentTile = this->InsideTriangleTileFifo.pop(clock_cycle);
            if (CurrentTile.size == 2)
            {
                if (this->x2TileAccumalateBuffer.size() < 16)
                {
                    this->x2TileAccumalateBuffer.push_back(CurrentTile);
                }
                else
                {
                    isCurrentTileFinished = false;
                    CurrentX = CurrentTile.LeftTopX;
                    CurrentY = CurrentTile.LeftTopY;
                    break;
                }
            }
            else if(CurrentTile.size == 4)
            {
                if (this->x4TileAccumalateBuffer.size() < 4)
                {
                    this->x4TileAccumalateBuffer.push_back(CurrentTile);
                }
                else
                {
                    isCurrentTileFinished = false;
                    CurrentX = CurrentTile.LeftTopX;
                    CurrentY = CurrentTile.LeftTopY;
                    break;
                }
            }
            else
            {
                isCurrentTileFinished = false;
                CurrentX = CurrentTile.LeftTopX;
                CurrentY = CurrentTile.LeftTopY;
                break;
            }
        }
    }
    if(isCurrentTileFinished)
    {
        if (!this->x2TileAccumalateBuffer.empty())
        {
            for (cTile &it : x2TileAccumalateBuffer)
            {
                MaskTile.LeftTopX = it.LeftTopX;
                MaskTile.LeftTopY = it.LeftTopY;
                MaskTileFifo.push(std::make_pair(clock_cycle + TileCommitCycle, std::move(MaskTile)));
            }
            x2TileAccumalateBuffer.clear();
        }
        else if (!this->x4TileAccumalateBuffer.empty())
        {
            for (cTile &it : x4TileAccumalateBuffer)
            {
                MaskTile.LeftTopX = it.LeftTopX;
                MaskTile.LeftTopY = it.LeftTopY;
                MaskTileFifo.push(std::make_pair(clock_cycle + TileCommitCycle, std::move(MaskTile)));
                MaskTile.LeftTopX = it.LeftTopX+2;
                MaskTile.LeftTopY = it.LeftTopY;
                MaskTileFifo.push(std::make_pair(clock_cycle + TileCommitCycle, std::move(MaskTile)));
                MaskTile.LeftTopX = it.LeftTopX;
                MaskTile.LeftTopY = it.LeftTopY+2;
                MaskTileFifo.push(std::make_pair(clock_cycle + TileCommitCycle, std::move(MaskTile)));
                MaskTile.LeftTopX = it.LeftTopX+2;
                MaskTile.LeftTopY = it.LeftTopY+2;
                MaskTileFifo.push(std::make_pair(clock_cycle + TileCommitCycle, std::move(MaskTile)));
            }
            x4TileAccumalateBuffer.clear();
        }
        PerformanceCounter.EndCounter(clock_cycle);
        return;
    }
    for (int i = 0; i < 16; ++i)
    {
        if (GetMaskTile(CurrentX, CurrentY, MaskTile, CurrentTile))
        {
            this->MaskTileFifo.push(std::make_pair(clock_cycle + 1, std::move(MaskTile)));
        }
        else
        {
            isCurrentTileFinished = true;
            break;
        }
    }
}

bool cTileDecomposer::isTileDecomposingFinished() { return this->isFinished; }

bool cInsideTileDecomposer::isInsideTileDecomposingFinished() { return x2TileAccumalateBuffer.empty() && x4TileAccumalateBuffer.empty() && !isCacheTileValid; }
