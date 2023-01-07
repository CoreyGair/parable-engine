#pragma once

#include "Core/Base.h"


namespace Parable
{


class Application;

class Time
{
public:
    Time(double target_frame_time) : m_target_frame_time(target_frame_time), m_delta_time(target_frame_time) {}
    Time(unsigned int target_frame_rate) : Time((double)1.0 / (double)target_frame_rate) {}
    Time() : Time((unsigned int)60) {}

    double get_delta_time() { return m_delta_time; }

    double frame_elapsed_time();

    void start_frame();

    void end_frame();

private:
    double m_target_frame_time;

    double m_frame_start_time;
    
    double m_delta_time; // time since prev frame 
};


}