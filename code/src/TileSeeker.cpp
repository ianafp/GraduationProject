#include"TileSeeker.h"
#include<iostream>
#include<fstream>
#include<string>
cTileSeekerFifoData::cTileSeekerFifoData(cTile _tile,cTileStatus _status,bool _isLastTile):tile(_tile),status(_status),isLastTile(_isLastTile){}
inline cTile GetRightTile(cTile tile){return cTile(tile.LeftTopX+tile.size,tile.LeftTopY,tile.size);}
inline cTile GetLeftTile(cTile tile){return cTile(tile.LeftTopX-tile.size,tile.LeftTopY,tile.size);}
inline cTile GetTopTile(cTile tile){return cTile(tile.LeftTopX,tile.LeftTopY+tile.size,tile.size);}
inline cTile GetBottomTile(cTile tile){return cTile(tile.LeftTopX,tile.LeftTopY-tile.size,tile.size);}
cTile::cTile(int x,int y,int _size):LeftTopX(x),LeftTopY(y),size(_size){}


cTileSeeker::cTileSeeker(cTriangleInfo _TriangleInfo,LatencyUnitFifo<cTile>& _TileFifo,LatencyUnitFifo<cTile>& _InsideTriangleTileFifo):TriangleInfo(_TriangleInfo),ResultTileFifo(_TileFifo),InsideTriangleTileFifo(_InsideTriangleTileFifo),HorizantalDirection(Direction::GO_RIGHT),VerticalDirection(Direction::GO_UP),isLeftDirectionValid(false),isDownDirectionValid(false),isTopDirectionValid(false),isNewRow(true),isFirstRow(true),isFinish(false),isTileCacheValid(false),isCacheTileInsideTriangle(false){
    CurrentTile = cTile((int)TriangleInfo.tri[TriangleInfo.midy].x,(int)TriangleInfo.tri[TriangleInfo.midy].y,TileSize);
}



cTileStatus cTileSeeker::GetTileStatus(cTile tile)
{
    cTileStatus status;
    // 检测四个顶点是否在三角形内
    // left top
    // int TileLeft = tile.LeftTopX,TileRight = tile.LeftTopX + tile.size-1,TileTop = tile.LeftTopY + tile.size-1, TileBottom = tile.LeftTopY;
    // status.is4ConerValid[0] = EdgeFunction(this->TriangleInfo.EdgeFunctionParameter,PixelCheckPositionX+TileLeft-this->TriangleInfo.InitPoint.x,TileTop-this->TriangleInfo.InitPoint.y+PixelCheckPositionY);
    // // right top
    // status.is4ConerValid[1] = EdgeFunction(this->TriangleInfo.EdgeFunctionParameter,PixelCheckPositionX+TileRight-this->TriangleInfo.InitPoint.x,TileTop-this->TriangleInfo.InitPoint.y+PixelCheckPositionY);
    // // left bottom
    // status.is4ConerValid[2] = EdgeFunction(this->TriangleInfo.EdgeFunctionParameter,PixelCheckPositionX+TileLeft-this->TriangleInfo.InitPoint.x,TileBottom-this->TriangleInfo.InitPoint.y+PixelCheckPositionY);
    // // right bottom
    // status.is4ConerValid[3] = EdgeFunction(this->TriangleInfo.EdgeFunctionParameter,PixelCheckPositionX+TileRight-this->TriangleInfo.InitPoint.x,TileBottom-this->TriangleInfo.InitPoint.y+PixelCheckPositionY);

    // status.TileValid = this->TriangleInfo.TileCheck(tile);
    // status.RightTileValid = this->TriangleInfo.TileCheck(cTile(tile.LeftTopX+tile.size,tile.LeftTopY,tile.size));
    // status.BottomTileValid = this->TriangleInfo.TileCheck(cTile(tile.LeftTopX,tile.LeftTopY-tile.size,tile.size));
    // status.LeftTileValid = this->TriangleInfo.TileCheck(cTile(tile.LeftTopX-tile.size,tile.LeftTopY,tile.size));
    // status.TopTileValid = this->TriangleInfo.TileCheck(cTile(tile.LeftTopX,tile.LeftTopY+tile.size,tile.size));


    status.RightTileValid  = TriangleInfo.is4ConerValid(cTile(tile.LeftTopX+tile.size,tile.LeftTopY,tile.size),status.is4ConerValid[0],status.is4ConerValid[1],status.is4ConerValid[2],status.is4ConerValid[3]);

    status.BottomTileValid  = TriangleInfo.is4ConerValid(cTile(tile.LeftTopX,tile.LeftTopY-tile.size,tile.size),status.is4ConerValid[0],status.is4ConerValid[1],status.is4ConerValid[2],status.is4ConerValid[3]);

    status.LeftTileValid  = TriangleInfo.is4ConerValid(cTile(tile.LeftTopX-tile.size,tile.LeftTopY,tile.size),status.is4ConerValid[0],status.is4ConerValid[1],status.is4ConerValid[2],status.is4ConerValid[3]);

    status.TopTileValid  = TriangleInfo.is4ConerValid(cTile(tile.LeftTopX,tile.LeftTopY+tile.size,tile.size),status.is4ConerValid[0],status.is4ConerValid[1],status.is4ConerValid[2],status.is4ConerValid[3]);

    status.TileValid = TriangleInfo.is4ConerValid(tile,status.is4ConerValid[0],status.is4ConerValid[1],status.is4ConerValid[2],status.is4ConerValid[3]);
    return status;
}

void cTileSeeker::RunCycle(int clock_cycle)
{
    if(isTileCacheValid)
    {
        PerformanceCounter.EndCounter(clock_cycle);
        if(isCacheTileInsideTriangle)
        {
            if(this->InsideTriangleTileFifo.CanPush())
            {
                this->InsideTriangleTileFifo.push(std::make_pair<int,cTile>(TileCommitCycle+clock_cycle,std::move(TileCache)));
                isTileCacheValid = false;
            }
        }
        else{
            if(this->ResultTileFifo.CanPush())
            {
                this->ResultTileFifo.push(std::make_pair<int,cTile>(TileSeekClock+clock_cycle,std::move(TileCache)));
                isTileCacheValid = false;
            }
        }
        return;
    }
    if(isFinish)
    {
        PerformanceCounter.EndCounter(clock_cycle);
        return;
    }
    // tile cache check
    PerformanceCounter.StartCounter(clock_cycle);

    cTileStatus status = this->GetTileStatus(this->CurrentTile);
    cTile NextTile;
    // 首先进行水平方向的检索，优先检索右侧
    // 起始tile
    if(CurrentTile.LeftTopX == TriangleInfo.InitPoint.x && CurrentTile.LeftTopY == TriangleInfo.InitPoint.y)
    {
        // to be implemented
    }
    // 更新 left cache tile
    
    // 更新换行tile
    if(VerticalDirection == Direction::GO_UP)
    {
        if(isTopDirectionValid == false && status.TopTileValid)
        {
            TopCachedTile = GetTopTile(CurrentTile);
            isTopDirectionValid = true;   
        }
        if(isFirstRow && isDownDirectionValid == false && status.BottomTileValid)
        {
            DownCachedTile = GetBottomTile(CurrentTile);
            isDownDirectionValid = true;
        }
    }
    else
    {
        if(isDownDirectionValid == false&&status.BottomTileValid)
        {
            DownCachedTile = GetBottomTile(CurrentTile);
            isDownDirectionValid = true;
        }
    }
    // 换行处理 
    if(isNewRow)
    {
        if(status.LeftTileValid)
        {
            isLeftDirectionValid = true;
            LeftCachedTile = GetLeftTile(CurrentTile);
        }
        isNewRow = false;
    }
    if(VerticalDirection == Direction::GO_UP)
    {
        // go up
        if(HorizantalDirection == Direction::GO_RIGHT)
        {
            if(status.RightTileValid)
            {
                NextTile = GetRightTile(CurrentTile);
            }
            else{
                if(isLeftDirectionValid)
                {
                    // 右侧扫描完毕后 向左扫描该行的剩余部分
                    HorizantalDirection = Direction::GO_LEFT;
                    NextTile = LeftCachedTile;
                    isLeftDirectionValid = false;
                }
                else{
                    // 换行 向上换行
                    if(isTopDirectionValid)
                    {
                        NextTile = TopCachedTile;
                        isTopDirectionValid = false;
                        isNewRow = true;
                        if(isFirstRow){
                            isFirstRow = false;
                        }
                    }
                    else
                    {
                        // 向上扫描完毕 向下扫描
                        if(isDownDirectionValid)
                        {
                            NextTile = DownCachedTile;
                            isDownDirectionValid = false;
                            isNewRow = true;
                            VerticalDirection = Direction::GO_DOWN;
                        }
                        else
                        {
                            // 扫描完毕
                            isFinish = true;
                        }
                    }
                }
                
            }
        }
        else
        {
            if(status.LeftTileValid)
            {
                NextTile = GetLeftTile(CurrentTile);
            }
            else
            {
                HorizantalDirection = Direction::GO_RIGHT;
                // 向上换行
                    if(isTopDirectionValid)
                    {
                        NextTile = TopCachedTile;
                        isTopDirectionValid = false;
                        isNewRow = true;
                        if(isFirstRow){
                            isFirstRow = false;
                        }
                    }
                    else
                    {
                        // 向上扫描完毕 向下扫描
                        if(isDownDirectionValid)
                        {
                            NextTile = DownCachedTile;
                            isDownDirectionValid = false;
                            isNewRow = true;
                            VerticalDirection = Direction::GO_DOWN;
                        }
                        else
                        {
                            // 扫描完毕
                            isFinish = true;
                        }
                    }
            }
        }
    }
    else{
        // go down
        if(HorizantalDirection == Direction::GO_RIGHT)
        {
            if(status.RightTileValid)
            {
                NextTile = GetRightTile(CurrentTile);
            }
            else{
                if(isLeftDirectionValid)
                {
                    // 右侧扫描完毕后 向左扫描该行的剩余部分
                    HorizantalDirection = Direction::GO_LEFT;
                    NextTile = LeftCachedTile;
                    isLeftDirectionValid = false;
                }
                else{
                    // 换行 向下换行
                    if(isDownDirectionValid)
                    {
                        NextTile = DownCachedTile;
                        isDownDirectionValid = false;
                        isNewRow = true;
                        if(isFirstRow)
                        {
                            isFirstRow = false;
                        }
                    }
                    else
                    {
                        // 向下扫描完毕 扫描完毕
                        isFinish = true;
                    }
                }
                
            }
        }
        else
        {
            if(status.LeftTileValid)
            {
                NextTile = GetLeftTile(CurrentTile);
            }
            else
            {
                // 换行 向下换行
                HorizantalDirection = GO_RIGHT;
                if(isDownDirectionValid)
                {
                    NextTile = DownCachedTile;
                    isDownDirectionValid = false;
                    isNewRow = true;
                    if(isFirstRow)
                    {
                        isFirstRow = false;
                    }
                }
                else
                {
                    // 向下扫描完毕 扫描完毕
                    isFinish = true;
                }
            }
        }
    }

    // 将tile提交到fifo中
    if(status.is4ConerValid[0] == true && status.is4ConerValid[1] == true && status.is4ConerValid[2] == true && status.is4ConerValid[3] == true)
    {
        // tile在三角形内
        if(this->InsideTriangleTileFifo.CanPush()){
            this->InsideTriangleTileFifo.push(std::make_pair<int,cTile>(TileSeekClock+clock_cycle,std::move(CurrentTile)));
        }
        else{
            isTileCacheValid = true;
            isCacheTileInsideTriangle = true;
            TileCache = CurrentTile;
        }
    }
    else{
        // tile部分在三角形内
        if(this->ResultTileFifo.CanPush()){
            this->ResultTileFifo.push(std::make_pair<int,cTile>(TileSeekClock+clock_cycle,std::move(CurrentTile)));
        }
        else{
            isTileCacheValid = true;
            isCacheTileInsideTriangle = false;
            TileCache = CurrentTile;
        }
    }
    CurrentTile = NextTile;
}

bool cTileSeeker::isTileSeekingFinished(){return this->isFinish;}


void cTileSeeker::ResetTriangle(cTriangleInfo _TriangleInfo)
{
    this->isFinish = false;
    this->TriangleInfo = _TriangleInfo;
    CurrentTile = cTile((int)TriangleInfo.tri[TriangleInfo.midy].x,(int)TriangleInfo.tri[TriangleInfo.midy].y,TileSize);
}