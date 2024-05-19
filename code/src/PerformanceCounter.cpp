#include"PerformanceCounter.h"

cPerformanceCounter::cPerformanceCounter(){
    isCounterStarted = false;
    CurrentClock = 0;
}
void cPerformanceCounter::StartCounter(int clock_cycle)
{   
    CurrentClock = clock_cycle;
    if(isCounterStarted == false)
    {
        isCounterStarted = true;
        StartClockCycle = clock_cycle;
    }
}

void cPerformanceCounter::EndCounter(int clock_cycle)
{
    if(isCounterStarted == true)
    {
        isCounterStarted = false ;
        if(clock_cycle!=StartClockCycle)
            WorkPeriod.push_back(std::make_pair(StartClockCycle,clock_cycle));
    }
}

void cPerformanceCounter::OutputToCsv(std::string TaskName,std::ofstream& OutFile)
{
    if(isCounterStarted == true)
    {
        WorkPeriod.push_back(std::make_pair(StartClockCycle,CurrentClock));
        isCounterStarted = false;
    }
    OutFile<<TaskName;
    for(auto& it:this->WorkPeriod)
    {
        OutFile<<","<<it.first<<","<<it.second;
    }
    OutFile<<std::endl;
}

void cPerformanceCounter::Reset(){
    isCounterStarted = false;
    this->WorkPeriod.clear();
    CurrentClock = 0;
}


int cPerformanceCounter::GetWorkClockSum()
{
    int sum = 0;
    for(auto& it:WorkPeriod)
    {
        sum += (it.second-it.first);
    }
    return sum;
}