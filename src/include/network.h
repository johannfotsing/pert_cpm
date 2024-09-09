/***
 * @brief This file contains the definition of class @code{network}.
 * 
 * @author Johann Fotsing
 * @date 2024-09-08
 * @file network.h
 *
 */

#pragma once

#include <queue>
#include <set>
#include <functional>
#include "activity.h"
#include "event.h"
#include "time_constraint.h"

namespace pert_cpm
{
    //using namespace std;

    /**
     * This class defines the @code{network} class.
     * 
     * Class specification:
     *  - add(activity, event, event): add an activity between start event (should be present in network) and end event (created if absent from network).
     *      Preconditions: 
     *          - start_event must exist in activity network. 
     *      Postconditions:
     *          - no duplicate arrows in between events.
     * - events():
     * - activities():
     * - predecessor_activities(activity):
     * - start(): starts the activity network ?
     * - start_event(): get the start event
     * - finish_event(): get the end event
     * - time_constraints():
     * 
     */
    class network
    {
        // Constructor(s)
        network();
        
    public:
        void add(const activity&, const event&, const event&);
        std::queue<event> events();
        std::set<activity> activities();
        std::set<activity> predecessor_activities(const activity&) const;
        std::set<activity> successor_activities(const activity&) const;
        const activity& get_activity(const activity&) const;
        const event& get_event(const event&) const;

        // forward_pass
        void set_earliest_start_time(const datetime_t&);
        datetime_t earliest_start_of(const activity&) const;
        datetime_t earliest_occurence_of(const event&) const;
        datetime_t earliest_finish_of(const activity&) const;

        // backward_pass
        void set_latest_finish_time(const datetime_t&);
        datetime_t latest_start_of(const activity&) const;
        datetime_t latest_occurence_of(const event&) const;
        datetime_t latest_finish_of(const activity&) const;

        // Slack
        time_t activity_slack(const activity&);
        time_t path_slack(const activity&);
        time_t interfering_slack(const activity&);
        time_t independent_slack(const activity&);

        // Critical path
        bool CompareActivitiesInPQ(const activity a1, const activity a2)
        {
            return this->earliest_finish_of(a1) < this->earliest_finish_of(a2);
        }
        using activity_pq = std::priority_queue<activity, std::vector<activity>, std::function<bool(const activity, const activity)>>;
        activity_pq find_critical_path();

    private:
        bool is_full();
        event start_event();
        event finish_event();
        event* event_ptr(const event&);
        bool find(const event&);
        void add(const event&);
        bool is_constrained(const activity&, const activity&) const;
        time_constraint get_time_constraint(const activity&, const activity&) const;
        std::vector<datetime_t> get_start_time_constraints(const activity&) const;
        std::vector<datetime_t> get_finish_time_constraints(const activity&) const;

    private:
        std::set<activity> _activities;
        std::set<event> _events;
        int _nb_events = 0;
    };

} // namespace pert_cpm


