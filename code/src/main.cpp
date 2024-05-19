#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "pipeline.h"
#include "bmp.h"
#include<math.h>
const int width = 800;
const int height = 600;
GLFWwindow *window;
const std::string external_file_path = EXTERNAL_FILE_PATH;
unsigned char pixels[width * height * 3]; // 2x2像素块的像素数据
inline int PixelR(int x, int y) { return (x + y * width) * 3; }
inline int PixelG(int x, int y) { return (x + y * width) * 3 + 1; }
inline int PixelB(int x, int y) { return (x + y * width) * 3 + 2; }
extern int TileSeekerWorkClock;
#ifndef _USE_DECOMPOSER_UNIT_
extern int TileDecomposer32WorkClock;
extern int TileDecomposer16WorkClock ;
extern int TileDecomposer8WorkClock;
extern int TileDecomposer4WorkClock;
#else 
extern int DecomposerUnitWorkClock[MaxDispatchUnitCount];
#endif
extern int InsideTileDecomposerWorkClock;
extern int MinTileRasterWorkClock;
extern int TotalClock;
extern int DispatchWorkClock;
const int RandomTriangleCount = 100;


extern int x32TileFifoTileCount ;
extern int x16TileFifoTileCount ;
extern int x8TileFifoTileCount ;
extern int x4TileFifoTileCount ;
extern int x2TileFifoTileCount ;
extern int x32TileWaitClock ;
extern int x16TileWaitClock ;
extern int x8TileWaitClock ;
extern int x4TileWaitClock ;
extern int x2TileWaitClock ;
extern int DecomposerUnitInFifoTileCount[4];
extern int DecomposerUnitInFifoWaitClock[4];
extern int MinRasterMaskTileFifoCount ;
extern int MinRasterMaskTileFifoWaitClock ;
extern int InsideDecomposerTileCount ;
extern int InsideDecomposerWaitClock ;
extern int InsideMaskCount ;
extern int InsideMaskWaitClock ;
void RunStatistics()
{
    std::ifstream RandomTriangleFile(external_file_path+"RandomTriangleList.txt");
    std::ofstream StatisticsFile(external_file_path+"Statistic3.csv");
    std::ofstream DummyFile(external_file_path+"Dummy.csv");
    StatisticsFile << " TileSeekerClock, TileDispatchClock, TileDecomposeClock,Total Cycles, Tile Seeker Work Load, Tile Dispatch Work Load, Mean Tile Decomposer Unit Work Load, Inside Tile Decomposer Work Load, Min Tile Raster Work Load "<<std::endl;
    cPipeline* PipeLinePtr;
    int N;
    int count = 0;
    Triangle RandomTriangleList[RandomTriangleCount];
    RandomTriangleFile >> N;
    for(int i=0;i<N;++i)
    {
        RandomTriangleFile>>RandomTriangleList[i].p1.x>>RandomTriangleList[i].p1.y>>RandomTriangleList[i].p2.x>>RandomTriangleList[i].p2.y>>RandomTriangleList[i].p3.x>>RandomTriangleList[i].p3.y;
    }
    std::cout.precision(3);
    DispatchUnitCount = 4;
    TileDecomposeCycle = 4;
    for(TileSeekClock = 3;TileSeekClock < 9;++TileSeekClock)
    {
        for(DispatchCycle = 1;DispatchCycle < 5;++DispatchCycle)
        {
            for(TileDecomposeCycle = 1;TileDecomposeCycle < 8;TileDecomposeCycle <<=1)
            {

                    for(int i=0;i<DispatchUnitCount;++i)
                    {
                        DecomposerUnitWorkClock[i] = 0;
                    }
                    TileSeekerWorkClock = 0;
                    InsideTileDecomposerWorkClock = 0;
                    MinTileRasterWorkClock = 0;
                    DispatchWorkClock = 0;
                    TotalClock = 0;
                    for(int i=0 ;i<N;++i)
                    {
                        PipeLinePtr = new cPipeline(RandomTriangleList[i],DummyFile);
                        while(!PipeLinePtr->isPipelineFinished())
                        {
                            std::vector<cMaskTile> MaskFifo;
                            PipeLinePtr->RunCycle(MaskFifo);
                            MaskFifo.clear();
                        }
                        delete PipeLinePtr;
                    }
                    int MeanUnitWorkClock = 0;
                    for(int i=0;i<DispatchUnitCount;++i)
                    {
                        MeanUnitWorkClock += DecomposerUnitWorkClock[i];
                    }
                    MeanUnitWorkClock /= DispatchUnitCount;
                    StatisticsFile << TileSeekClock<<"," << DispatchCycle<<"," << TileDecomposeCycle<<"," << TotalClock<<"," << 100 * TileSeekerWorkClock/TotalClock<<"," << 100 * DispatchWorkClock/TotalClock<<"," << 100 * MeanUnitWorkClock/TotalClock<<"," << 100 * InsideTileDecomposerWorkClock/TotalClock<<","<<100 * MinTileRasterWorkClock/TotalClock<<","<<std::endl;

                    count ++;
                    std::cout<<"finish "<<count<<"/"<<3072<<std::endl;
                
            }
        }
    }

}
void CalculateSerial()
{
    std::ifstream RandomTriangleFile(external_file_path+"RandomTriangleList.txt");

    int N;
    // RandomTriangleFile >> N;
    Triangle RandomTriangleList[RandomTriangleCount];
    RandomTriangleFile >> N;
    for(int i=0;i<N;++i)
    {
        RandomTriangleFile>>RandomTriangleList[i].p1.x>>RandomTriangleList[i].p1.y>>RandomTriangleList[i].p2.x>>RandomTriangleList[i].p2.y>>RandomTriangleList[i].p3.x>>RandomTriangleList[i].p3.y;
    }
    int SerialClockCycles = 0;
    std::vector<double> x;
    std::vector<double> y;
    for(int i=0;i<N;++i)
    {
        x.push_back(RandomTriangleList[i].p1.x);
        x.push_back(RandomTriangleList[i].p2.x);
        x.push_back(RandomTriangleList[i].p3.x);
        y.push_back(RandomTriangleList[i].p1.y);
        y.push_back(RandomTriangleList[i].p2.y);
        y.push_back(RandomTriangleList[i].p3.y);
        std::sort(x.begin(),x.end());
        std::sort(y.begin(),y.end());

        SerialClockCycles += int((x[2]-x[0])*(y[2]-y[0]));
        x.clear();
        y.clear();
    }
    std::cout<<SerialClockCycles;
}
void CalculateQueueSize()
{
    std::ifstream RandomTriangleFile(external_file_path+"RandomTriangleList.txt");
    std::ofstream StatisticsFile(external_file_path+"Queue.csv");
    std::ofstream DummyFile(external_file_path+"Dummy.csv");
    int N;
    // RandomTriangleFile >> N;
    Triangle RandomTriangleList[RandomTriangleCount];
    RandomTriangleFile >> N;
    for(int i=0;i<N;++i)
    {
        RandomTriangleFile>>RandomTriangleList[i].p1.x>>RandomTriangleList[i].p1.y>>RandomTriangleList[i].p2.x>>RandomTriangleList[i].p2.y>>RandomTriangleList[i].p3.x>>RandomTriangleList[i].p3.y;
    }
    cPipeline* PipeLinePtr = NULL;
    for(int i = 0;i < N;++i)
    {
        PipeLinePtr = new cPipeline(RandomTriangleList[i],DummyFile);
        while(!PipeLinePtr->isPipelineFinished())
        {
            std::vector<cMaskTile> MaskFifo;
            PipeLinePtr->RunCycle(MaskFifo);
            MaskFifo.clear();
        }
        delete PipeLinePtr;
    }
    StatisticsFile << "Quene, Mean Incoming Elements Per Cycle, Elements Mean Waiting Clock, Ideal Queue Length"<<std::endl;
    StatisticsFile.precision(3);
    StatisticsFile <<"x32TileFifo, "<<double(x32TileFifoTileCount)/TotalClock<<"," <<double(x32TileWaitClock)/TotalClock<<"," << (int)ceil((double(x32TileFifoTileCount)/TotalClock)*(double(x32TileWaitClock)/TotalClock))<<std::endl;

    StatisticsFile <<"x16TileFifo, "<<double(x16TileFifoTileCount)/TotalClock<<"," <<double(x16TileWaitClock)/TotalClock<<"," << (int)ceil((double(x16TileFifoTileCount)/TotalClock)*(double(x16TileWaitClock)/TotalClock))<<std::endl;

    StatisticsFile <<"x8TileFifo, "<<double(x8TileFifoTileCount)/TotalClock<<"," <<double(x8TileWaitClock)/TotalClock<<"," << (int)ceil((double(x8TileFifoTileCount)/TotalClock)*(double(x8TileWaitClock)/TotalClock))<<std::endl;

    StatisticsFile <<"x4TileFifo, "<<double(x4TileFifoTileCount)/TotalClock<<"," <<double(x4TileWaitClock)/TotalClock<<"," << (int)ceil((double(x4TileFifoTileCount)/TotalClock)*(double(x4TileWaitClock)/TotalClock))<<std::endl;

    StatisticsFile <<"x2TileFifo, "<<double(x2TileFifoTileCount)/TotalClock<<"," <<double(x2TileWaitClock)/TotalClock<<"," << (int)ceil((double(x2TileFifoTileCount)/TotalClock)*(double(x2TileWaitClock)/TotalClock))<<std::endl;

    for(int i=0;i < 4; ++i)
    {
        StatisticsFile<<"DecomposerUnit"<<i<<", "<<double(DecomposerUnitInFifoTileCount[i])/TotalClock<<"," <<double(DecomposerUnitInFifoWaitClock[i])/TotalClock<<"," << (int)ceil(double((DecomposerUnitInFifoTileCount[i])/TotalClock)*(double(DecomposerUnitInFifoWaitClock[i])/TotalClock))<<std::endl;
    }

    StatisticsFile <<"InsideTileFifo, "<<double(InsideDecomposerTileCount)/TotalClock<<"," <<double(InsideDecomposerWaitClock)/TotalClock<<"," << (int)ceil((double(InsideDecomposerTileCount)/TotalClock)*(double(InsideDecomposerWaitClock)/TotalClock))<<std::endl;

    StatisticsFile <<"InsideMaskFifo, "<<double(InsideMaskCount)/TotalClock<<"," <<double(InsideMaskWaitClock)/TotalClock<<"," << (int)ceil((double(InsideMaskCount)/TotalClock)*(double(InsideMaskWaitClock)/TotalClock))<<std::endl;

    StatisticsFile <<"RasterMaskFifo, "<<double(MinRasterMaskTileFifoCount)/TotalClock<<"," <<double(MinRasterMaskTileFifoWaitClock)/TotalClock<<"," << (int)ceil((double(MinRasterMaskTileFifoCount)/TotalClock)*(double(MinRasterMaskTileFifoWaitClock)/TotalClock))<<std::endl;
    
}
void updatePixelBlock(cPipeline *pipeline)
{
    // 在这个函数中更新像素块的数据，这里只是随机生成颜色
    if (pipeline->isPipelineFinished())
    {
        return;
    }
    std::vector<cMaskTile> MaskFifo;
    pipeline->RunCycle(MaskFifo);
    for (auto &it : MaskFifo)
    {
        if(it.LeftTopX<0||it.LeftTopY<0) continue;
        if (it.Mask[0][0])
        {
            pixels[PixelR(it.LeftTopX, it.LeftTopY)] = 255;
            pixels[PixelG(it.LeftTopX, it.LeftTopY)] = 0;
            pixels[PixelB(it.LeftTopX, it.LeftTopY)] = 0;
        }
        if (it.Mask[0][1])
        {
            pixels[PixelR(it.LeftTopX + 1, it.LeftTopY)] = 255;
            pixels[PixelG(it.LeftTopX + 1, it.LeftTopY)] = 0;
            pixels[PixelB(it.LeftTopX + 1, it.LeftTopY)] = 0;
        }
        if (it.Mask[1][0])
        {
            pixels[PixelR(it.LeftTopX, it.LeftTopY + 1)] = 255;
            pixels[PixelG(it.LeftTopX, it.LeftTopY + 1)] = 0;
            pixels[PixelB(it.LeftTopX, it.LeftTopY + 1)] = 0;
        }
        if (it.Mask[1][1])
        {
            pixels[PixelR(it.LeftTopX + 1, it.LeftTopY + 1)] = 255;
            pixels[PixelG(it.LeftTopX + 1, it.LeftTopY + 1)] = 0;
            pixels[PixelB(it.LeftTopX + 1, it.LeftTopY + 1)] = 0;
        }
    }
    // MaskFifo.clear();
}
void updateMoreTriangle(std::vector<cPipeline *> &PipelineVec)
{
    static int index = 0;
    if (index == PipelineVec.size())
    {
        return;
    }
    if (!PipelineVec[index]->isPipelineFinished())
    {
        updatePixelBlock(PipelineVec[index]);
    }
    else
    {
        writeBMP(external_file_path+"TriangleImg/Result"+std::to_string(index)+".bmp", pixels, width, height);
        if (index < PipelineVec.size() - 1)
            for (int i = 0; i < height; i++)
            {
                for (int j = 0; j < width; j++)
                {
                    pixels[(i * width + j) * 3 + 0] = 0;
                    pixels[(i * width + j) * 3 + 1] = 0;
                    pixels[(i * width + j) * 3 + 2] = 0;
                }
            }
        else
        {

            std::cout.precision(2);
            std::cout<<"clock:"<<TotalClock<<std::endl;
            std::cout<<"Tile Seeker Work Clock:"<<TileSeekerWorkClock<<","<<100.0*TileSeekerWorkClock/TotalClock<<"%"<<std::endl;
#ifndef _USE_DECOMPOSER_UNIT_
            std::cout<<"Tile Decomposer 32 Work Clock:"<<TileDecomposer32WorkClock<<","<<100.0*TileDecomposer32WorkClock/TotalClock<<"%"<<std::endl;
            std::cout<<"Tile Decomposer 16 Work Clock:"<<TileDecomposer16WorkClock<<","<<100.0*TileDecomposer16WorkClock/TotalClock<<"%"<<std::endl;
            std::cout<<"Tile Decomposer 8 Work Clock:"<<TileDecomposer8WorkClock<<","<<100.0*TileDecomposer8WorkClock/TotalClock<<"%"<<std::endl;
            std::cout<<"Tile Decomposer 4 Work Clock:"<<TileDecomposer4WorkClock<<","<<100.0*TileDecomposer4WorkClock/TotalClock<<"%"<<std::endl;
#else 
            for(int i=0;i<DispatchUnitCount;++i)
            {
                std::cout<<"Decomposer Unit "<<i<<" Work Clock: "<<DecomposerUnitWorkClock[i]<<","<<100.0*DecomposerUnitWorkClock[i]/TotalClock<<"%"<<std::endl;
            }
#endif
            std::cout<<"Inside Tile Work Clock:"<<InsideTileDecomposerWorkClock<<","<<100.0*InsideTileDecomposerWorkClock/TotalClock<<"%"<<std::endl;
            std::cout<<"Min Tile Raster Work Clock:"<<MinTileRasterWorkClock<<","<<100.0*MinTileRasterWorkClock/TotalClock<<"%"<<std::endl;

        }
        delete PipelineVec[index];
        index++;
    }
}
void render()
{
    // 使用glDrawPixels绘制像素块
    glDrawPixels(width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels);
}

void setup()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // 设置清屏颜色为黑色
}

void display(std::vector<cPipeline *> &PiplineVec)
{
    glClear(GL_COLOR_BUFFER_BIT); // 清屏

    for (int i = 0; i < 50; ++i)
        updateMoreTriangle(PiplineVec);
    render(); // 绘制像素块

    glfwSwapBuffers(window); // 交换缓冲区
}

int main()
{
    CalculateQueueSize();
    // CalculateSerial();
    // RunStatistics();
    // // 初始化GLFW
    // if (!glfwInit())
    // {
    //     std::cerr << "Failed to initialize GLFW" << std::endl;
    //     return -1;
    // }

    // // 创建窗口
    // window = glfwCreateWindow(width, height, "Pixel Block", NULL, NULL);
    // if (!window)
    // {
    //     glfwTerminate();
    //     std::cerr << "Failed to create GLFW window" << std::endl;
    //     return -1;
    // }
    // glfwMakeContextCurrent(window);

    // // 初始化GLEW
    // if (glewInit() != GLEW_OK)
    // {
    //     std::cerr << "Failed to initialize GLEW" << std::endl;
    //     return -1;
    // }

    // // 设置OpenGL状态
    // setup();
    // // 初始化流水线
    // // Triangle tri;
    // std::ifstream TriangleList(external_file_path+"RandomTriangleList.txt");
    // if(!TriangleList.is_open())
    // {
    //     std::cerr << "can not open triangle list: "<<external_file_path+"TriangleList.txt" << std::endl;
    //     return 1;
    // }
    // int N; // triangle count
    // TriangleList >> N;
    // Triangle tri;
    // std::vector<std::ofstream*> outCsv;
    // std::vector<cPipeline *> PipelineVec;
    // for(int i=0;i<N;++i)
    // {
    //     TriangleList >> tri.p1.x >> tri.p1.y >> tri.p2.x >> tri.p2.y >> tri.p3.x >> tri.p3.y;
    //     outCsv.push_back(new std::ofstream(external_file_path+"data"+std::to_string(i)+".csv"));
    //     if(!outCsv[i]->is_open())
    //     {
    //         std::cout<<external_file_path+"data"+std::to_string(i)+".csv"<<" not open!\n";
    //         exit(-1);
    //     }
    //     PipelineVec.push_back(new cPipeline(tri,*outCsv[i]));
    // }


    // for (int i = 0; i < height; i += 32)
    // {
    //     for (int j = 0; j < width; j += 32)
    //     {
    //         pixels[(i * width + j) * 3 + 0] = 255;
    //         pixels[(i * width + j) * 3 + 1] = 255;
    //         pixels[(i * width + j) * 3 + 2] = 255;
    //     }
    // }
    // while (!glfwWindowShouldClose(window))
    // {
    //     display(PipelineVec); // 显示内容
    //     glfwPollEvents();     // 处理事件
    // }

    // // 清理
    // glfwDestroyWindow(window);
    // glfwTerminate();

    return 0;
}
