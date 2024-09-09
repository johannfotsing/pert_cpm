/**
 * @brief
 * 
 * @author Johann Fotsing
 * @date 2024-09-08
 * @file activity.cpp
 */

#include "../include/activity.h"

using namespace pert_cpm;

pert_cpm::time_t activity::estimated_duration() const
{
    return _estimated_duration;
}
        
void activity::set_estimated_duration(const time_t& a_duration)
{
    _estimated_duration = a_duration;
}

void activity::set_start_event(event* const a_start_event)
{
    _start_event = a_start_event;
}

void activity::set_finish_event(event* const a_finish_event)
{
    _finish_event = a_finish_event;
}