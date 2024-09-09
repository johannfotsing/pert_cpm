/**
 * @brief
 * 
 * @author Johann Fotsing
 * @date 2024-09-08
 * @file network.cpp
 */

#include "../include/network.h"
#include <algorithm>
#include <map>

using namespace pert_cpm;

std::set<activity> network::predecessor_activities(const activity& an_activity) const
{
    std::set<activity> activities_;
    for (const activity& act: _activities)
    {
        if (*act._finish_event == *an_activity._start_event)
            activities_.emplace(act);
    }
    return activities_;
}

std::set<activity> network::successor_activities(const activity& an_activity) const
{
    std::set<activity> activities_;
    for (const activity& act: _activities)
    {
        if (*act._start_event == *an_activity._finish_event)
            activities_.emplace(act);
    }
    return activities_;
}

std::vector<datetime_t> network::get_start_time_constraints(const activity& an_activity) const
{
    std::vector<datetime_t> time_constraints = std::vector<datetime_t>();
    for (const activity& act: predecessor_activities(an_activity))
    {
        // EF(act) + optional_FS(act, an_activity)
        datetime_t predecessor_constraint = earliest_finish_of(act);
        if (is_constrained(act, an_activity) and (get_time_constraint(act, an_activity).get_type() == time_constraint::type::finish_to_start))
        {
            time_constraint fs_tc = get_time_constraint(act, an_activity);
            predecessor_constraint = predecessor_constraint + fs_tc.length();
        }
        time_constraints.emplace_back(predecessor_constraint);

        // ES(act) + optional_SS(act, an_activity)
        predecessor_constraint = earliest_start_of(act);
        if (is_constrained(act, an_activity) and (get_time_constraint(act, an_activity).get_type() == time_constraint::type::start_to_start))
        {
            time_constraint ss_tc = get_time_constraint(act, an_activity);
            predecessor_constraint = predecessor_constraint + ss_tc.length();
        }
        time_constraints.emplace_back(predecessor_constraint);


        // EF(act) + optional_FF(act, an_activity)
        predecessor_constraint = earliest_finish_of(act);
        if (is_constrained(act, an_activity) and (get_time_constraint(act, an_activity).get_type() == time_constraint::type::finish_to_finish))
        {
            time_constraint ff_tc = get_time_constraint(act, an_activity);
            predecessor_constraint = predecessor_constraint + ff_tc.length() - get_activity(an_activity).estimated_duration();
        }
        time_constraints.emplace_back(predecessor_constraint);

        // ES(act) + optional_SF(act, an_activity)
        predecessor_constraint = earliest_start_of(act);
        if (is_constrained(act, an_activity) and (get_time_constraint(act, an_activity).get_type() == time_constraint::type::start_to_finish))
        {
            time_constraint sf_tc = get_time_constraint(act, an_activity);
            predecessor_constraint = predecessor_constraint + sf_tc.length() - get_activity(an_activity).estimated_duration();
        }
        time_constraints.emplace_back(predecessor_constraint);
    }

    return time_constraints;
}

std::vector<datetime_t> network::get_finish_time_constraints(const activity& an_activity) const
{
    std::vector<datetime_t> time_constraints = std::vector<datetime_t>();
    for (const activity& act: successor_activities(an_activity))
    {
        // LS(act) - optional_FS(an_activity, act)
        datetime_t successor_constraint = latest_start_of(act);
        if (is_constrained(an_activity, act) and (get_time_constraint(an_activity, act).get_type() == time_constraint::type::finish_to_start))
        {
            time_constraint fs_tc = get_time_constraint(an_activity, act);
            successor_constraint = successor_constraint - fs_tc.length();
        }
        time_constraints.emplace_back(successor_constraint);

        // LF(act) - optional_FF(an_activity, act)
        successor_constraint = latest_finish_of(act);
        if (is_constrained(an_activity, act) and (get_time_constraint(an_activity, act).get_type() == time_constraint::type::finish_to_finish))
        {
            time_constraint ff_tc = get_time_constraint(an_activity, act);
            successor_constraint = successor_constraint - ff_tc.length();
        }
        time_constraints.emplace_back(successor_constraint);


        // LS(act) - optional_SS(an_activity, act)
        successor_constraint = latest_start_of(act);
        if (is_constrained(an_activity, act) and (get_time_constraint(an_activity, act).get_type() == time_constraint::type::start_to_start))
        {
            time_constraint ss_tc = get_time_constraint(act, an_activity);
            successor_constraint = successor_constraint - ss_tc.length() + get_activity(an_activity).estimated_duration();
        }
        time_constraints.emplace_back(successor_constraint);

        // LF(act) + optional_SF(an_activity, act)
        successor_constraint = latest_finish_of(act);
        if (is_constrained(an_activity, act) and (get_time_constraint(an_activity, act).get_type() == time_constraint::type::start_to_finish))
        {
            time_constraint sf_tc = get_time_constraint(act, an_activity);
            successor_constraint = successor_constraint - sf_tc.length() - get_activity(an_activity).estimated_duration();
        }
        time_constraints.emplace_back(successor_constraint);
    }

    return time_constraints;
}

/// Forward Pass

void network::set_earliest_start_time(const datetime_t& a_start_time)
{
    event_ptr(event::root)->_earliest_occurence = a_start_time;
}

datetime_t network::earliest_start_of(const activity& an_activity) const
{
    // check activity present
    bool contains_activity = (_activities.count(an_activity) == 1);
    if (not contains_activity)
        return std::chrono::system_clock::now();

    // network contains activity

    // if start event is the root event
    if (*an_activity._start_event == event::root)
    {
        return get_event(event::root)._earliest_occurence;
    }
    // else
    std::vector<datetime_t> time_constraints = get_start_time_constraints(an_activity);
    time_constraints.emplace_back(get_event(event::root)._earliest_occurence);
    return *std::max_element(time_constraints.begin(), time_constraints.end());
}

datetime_t network::earliest_finish_of(const activity& an_activity) const
{
    return earliest_start_of(an_activity) + get_activity(an_activity).estimated_duration();
}

datetime_t network::earliest_occurence_of(const event& an_event) const
{
    // check event present in network
    bool contains_event = _events.count(an_event) == 1;
    if (not contains_event)
        return std::chrono::system_clock::now();

    // find any activity starting with event and return earliest_start_of activity
    for (const activity& act: _activities)
    {
        if (*act._start_event == an_event)
        {
            return earliest_start_of(act);
        }
    }
}

/// Backward Pass

void network::set_latest_finish_time(const datetime_t& a_finish_time)
{
    event_ptr(event::finish)->_earliest_occurence = a_finish_time;
}

datetime_t network::latest_finish_of(const activity& an_activity) const
{
    // check activity present
    bool contains_activity = (_activities.count(an_activity) == 1);
    if (not contains_activity)
        return std::chrono::system_clock::now();

    // network contains activity

    // if finish event is the terminal event
    if (*an_activity._finish_event == event::finish)
    {
        return get_event(event::finish)._latest_occurence;
    }
    // else
    std::vector<datetime_t> time_constraints = get_finish_time_constraints(an_activity);
    time_constraints.emplace_back(get_event(event::finish)._latest_occurence);
    return *std::min_element(time_constraints.begin(), time_constraints.end());
}

datetime_t network::latest_start_of(const activity& an_activity) const
{
    return latest_finish_of(an_activity) - get_activity(an_activity).estimated_duration();
}

datetime_t network::latest_occurence_of(const event& an_event) const
{
    // check event present in network
    bool contains_event = _events.count(an_event) == 1;
    if (not contains_event)
        return std::chrono::system_clock::now();

    // find any activity starting with event and return earliest_start_of activity
    for (const activity& act: _activities)
    {
        if (*act._start_event == an_event)
        {
            return latest_start_of(act);
        }
    }
}

/// Slack

pert_cpm::time_t network::activity_slack(const activity& an_activity)
{
    return earliest_occurence_of(*(an_activity._finish_event)) - earliest_finish_of(an_activity);
}

pert_cpm::time_t network::path_slack(const activity& an_activity)
{
    return latest_start_of(an_activity) - earliest_start_of(an_activity);
}

pert_cpm::time_t network::interfering_slack(const activity& an_activity)
{
    return path_slack(an_activity) - activity_slack(an_activity);
}

pert_cpm::time_t network::independent_slack(const activity& an_activity)
{
    return earliest_occurence_of(*(an_activity._finish_event)) - latest_occurence_of(*(an_activity._start_event)) - an_activity.estimated_duration();
}

/// Mutators

void network::add(const event& an_event)
{
    event ev(an_event);
    ev.set_id(_nb_events++);
    _events.emplace(ev);
}

void network::add(const activity& an_activity, const event& a_start_event, const event& a_finish_event)
{
    // check start event in network
    bool contains_start_event = (_events.count(a_start_event) == 1);
    if (not contains_start_event)
        return;

    // create finish event if necessary
    bool contains_finish_event = (_events.count(a_finish_event) == 1);
    if (not contains_finish_event)
    {
        add(a_finish_event);
    }
    else
    {
        // check no duplicate activity
        for (const activity& act: activities())
        {
            if (*act._finish_event == a_finish_event and *act._start_event == a_start_event)
                return;
        }
    }

    // Create new activity
    activity new_act(an_activity);
    new_act.set_start_event(event_ptr(a_start_event));
    new_act.set_finish_event(event_ptr(a_finish_event));
    // add to set
    _activities.insert(new_act);
}

network::activity_pq network::find_critical_path()
{
    set_earliest_start_time(std::chrono::system_clock::now());
    set_latest_finish_time(earliest_occurence_of(event::finish));

    network::activity_pq critical_path;
    for (activity act: _activities)
    {
        if (path_slack(act) == pert_cpm::time_t(0))
            critical_path.emplace(act);
    }
    return critical_path;
}