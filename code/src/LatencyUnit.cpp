#include"LatencyUnit.h"
cClockCycle::cClockCycle():clock(0){}
void cClockCycle::ResetClock(){
    this->clock = 0;
}
int cClockCycle::GetClock(){
    return this->clock;
}
void cClockCycle::Tick(){
    this->clock++;
}
