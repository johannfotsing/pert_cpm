/***
 * @brief This file contains the definition of class @code{activity}.
 * 
 * @author Johann Fotsing
 * @date 2024-09-08
 * @file activity.h
 */

#pragma once

#include <ctime>
#include "event.h"

namespace pert_cpm
{
    using namespace std;

    /**
     * This class defines the @code{activity} class.
     * 
     * Class specification:
     *  - estimated_duration(): get the activity's estimated duration
     *  - set_estimated_duration(std::time_t): set the activity's estimated duration
     * 
     */
    class activity
    {
        // Constructor(s)
        activity() : _estimated_duration(0), _start_event(nullptr), _finish_event(nullptr) {}

    friend class network;

    public:
        time_t estimated_duration() const;
        void set_estimated_duration(const time_t&);

    private:
        void set_start_event(event* const);
        void set_finish_event(event* const);

    private:
        event *_start_event, *_finish_event;
        time_t _estimated_duration;
    };

} // namespace pert_cpm
