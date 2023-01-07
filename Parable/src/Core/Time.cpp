#include "Time.h"
#include "Core/Base.h"

#include <GLFW/glfw3.h>


namespace Parable
{


double Time::frame_elapsed_time()
{
    return glfwGetTime() - m_frame_start_time;
}

void Time::start_frame()
{
    m_frame_start_time = glfwGetTime();
}

void Time::end_frame()
{
    m_delta_time = frame_elapsed_time();
}


}