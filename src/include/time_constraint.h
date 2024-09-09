/**
 * @brief
 * 
 * @author Johann Fotsing
 * @date 2024-09-08
 * @file time_constraint.h
 */

#pragma once

#include <ctime>
#include "activity.h"

namespace pert_cpm
{
    class time_constraint
    {

    public:
        typedef enum class typeT
        {
            finish_to_start,
            start_to_start,
            finish_to_finish,
            start_to_finish,
            ss_ff
        } type;

        time_constraint(type, activity, activity, time_t, time_t);
        
        time_constraint finish_to_start(activity, activity, time_t);
        time_constraint start_to_start(activity, activity, time_t);
        time_constraint finish_to_finish(activity, activity, time_t);
        time_constraint start_to_finish(activity, activity, time_t);
        time_constraint ss_ff(activity, activity, time_t, time_t);

        void relax();
        time_t length() const;
        type get_type() const;
        bool is_active();
        void activate();

    private:
        bool _active;
        type _type;
        activity* _left_activity;
        activity* _right_activity;
    };

} // namespace pert_cpm
