#include<iostream>

#include<fstream>
#include<string>
#include<vector>
class cPerformanceCounter{
public:
    cPerformanceCounter();
    void StartCounter(int clock_cycle);
    void EndCounter(int clock_cycle);
    void OutputToCsv(std::string TaskName,std::ofstream& OutFile);
    int GetWorkClockSum();
    void Reset();
private:
    bool isCounterStarted;
    int StartClockCycle;
    int CurrentClock;
    std::vector<std::pair<int,int>> WorkPeriod;
};