/***
 * @brief This file describes the basic template classes we used to represent an activity network.
 * @author Johann Fotsing
 * @date 2024-09-10
 * @file pert.h
 */

#pragma once

#include <map>
#include <set>
#include <algorithm>
#include <list>
#include <iostream>
#include <vector>
#include <string>
#include <bits/stdc++.h>

/**
 * @brief PERT classes and methods are in namespace pert
 * 
 */
namespace pert
{

    /**
     * @brief This classes describes a template activity network.
     * A network is made of distinct event objects connected by activity objects of specific durations.
     * 
     * @tparam EventIDType type of the event objects, derives std::less (and std::equal ?)
     * @tparam DurationType the type of the duration objects used, derives std::less, operator-
     */
    template<typename EventIDType, typename DurationType>
    class network
    {

    public:

        /// @brief context types
        using event = EventIDType;
        using duration = DurationType;

        /**
         * @brief This class describes an activity
         * An activity links a trigger event to a completion event.
         * 
         */
        class activity
        {

        public:
            activity(event a_trigger_event, event a_completion_event) : __trigger_event(a_trigger_event), __completion_event(a_completion_event) {}
            bool precedes(const event& e) const { return __completion_event == e; }
            bool follows(const event& e) const { return __trigger_event == e; }
            event trigger_event() const { return __trigger_event; }
            event completion_event() const { return __completion_event; }
            activity reverse() const { return activity(__completion_event, __trigger_event); }
            bool operator<(const activity& a) const
            {
                if (__trigger_event == a.__trigger_event)
                    return __completion_event < a.__completion_event;
                
                return __trigger_event < a.__trigger_event;
            };
            bool operator==(const activity& a) const
            {
                
                return __trigger_event == a.__trigger_event and __completion_event == a.__completion_event;
            };

        private:
            event __trigger_event;
            event __completion_event;
        };

        /// @brief context types
        using segment = std::pair<activity, duration>;
        using path = std::vector<segment>;
        // TODO: find a way to guarantee that a path is correctly ordered

        /// @brief A schedule defines an earliest start time and a latest finish time for the network completion.
        struct schedule
        {
            duration initial_time;
            duration terminal_time;
            schedule(duration an_initial_time, duration a_terminal_time) : initial_time(an_initial_time), terminal_time(a_terminal_time) {}
        };

    public:

        /// @brief return a set made of activities in the network
        /// @return the set of activities of the network
        std::set<activity> activities() const
        {
            std::set<activity> _activities;
            for (const auto& a: __data)
                _activities.emplace(a.first);

            return _activities;
        };
        
        /// @brief Add an activity to the network
        /// @param a_trigger_event activity's trigger event
        /// @param a_completion_event activity's completion event
        /// @param a_duration activity's duration
        /// @return a reference to this network (for syntactic sugar)
        network& add_activity(const event& a_trigger_event, const event& a_completion_event, const duration& a_duration)
        {
            return add_activity(activity(a_trigger_event, a_completion_event), a_duration);
        };
        
        /// @brief Add an activity to the network
        /// @param an_activity a reference to an activity object that will be copied into the network
        /// @param a_duration the activity's duration
        /// @return a reference to this network (for syntactic sugar)
        network& add_activity(const activity& an_activity, const duration& a_duration)
        {
            // no two activities can directly connect two events
            auto search = __data.find(an_activity.reverse());
            if (search != __data.end())
            {
                // DEBUG
                std::cout << "Reverse present: " << an_activity.trigger_event() << " ---> " << an_activity.completion_event() << std::endl;
                return *this;
            }

            __data.insert({an_activity, a_duration});

            return *this;
        };
        
        /// @brief Delete an activity from the network
        /// @param a_trigger_event the activity's trigger event
        /// @param a_completion_event the activity's completion event
        /// @return a reference to this network (for syntactic sugar)
        network& delete_activity(const event& a_trigger_event, const event& a_completion_event)
        {
            return delete_activity(activity(a_trigger_event, a_completion_event));
        };
        
        /// @brief Delete an activity from the network
        /// @param an_activity the value of the activity object to remove from network
        /// @return a reference to this network (for syntactic sugar)
        network& delete_activity(const activity& an_activity)
        {
            __data.erase(an_activity);
            return *this;
        };
        
        /// @brief Get the estimated duration of an activity in the network
        /// @param an_activity the value of the activity which duration is requested
        /// @return the duration of the activity
        duration estimated_duration(const activity& an_activity) const
        {
            // DEBUG ?
            // TODO: throw an exception
            auto search_activity = __data.find(an_activity);
            if (search_activity == __data.end())
                return -1;

            return __data.at(an_activity);
        };

        /// @brief Set the estimated duration of an activity in the network
        /// @param an_activity the value of the activity which duration is to be set
        /// @param a_duration the duration of the activity
        void set_estimated_duration(const activity& an_activity, const duration& a_duration)
        {
            // TODO: throw exception if activity is not present
            __data[an_activity] = a_duration;
        };

        /// @brief A network is well formed if it contains no loop, exactly one start event and exactly one terminal event.
        /// @return true if the network is well formed, false otherwise.
        bool is_well_formed() const
        {
            // check ends
            if (initial_events().size() != 1 or terminal_events().size() != 1)
                return false;
            
            // check loops
            const event initial_event = *initial_events().begin();
            const event terminal_event = *terminal_events().begin();
            return loop_paths(initial_event, terminal_event).empty();
        };

        /// @brief Get the initial events of the network (single element list for well formed network)
        ///        An initial event is an event with no predecessor activity.
        /// @return a set of events (unique by definition)
        std::set<event> initial_events() const
        {
            std::set<event> _initial_events;
            auto _trigger_events = trigger_events();
            auto _completion_events = completion_events();
            for (const auto& e: _trigger_events)
            {
                if (_completion_events.find(e) == _completion_events.end())
                    _initial_events.emplace(e);
            }
            return _initial_events;
        };

        /// @brief Get the terminal events of the network (single element list for well formed network)
        ///        A terminal event is an event with no successor activity.
        /// @return a set of events (unique by definition)
        std::set<event> terminal_events() const
        {
            std::set<event> _terminal_events;
            auto _trigger_events = trigger_events();
            auto _completion_events = completion_events();
            for (const auto& e: _completion_events)
            {
                if (_trigger_events.find(e) == _trigger_events.end())
                    _terminal_events.emplace(e);
            }
            return _terminal_events;
        };


        // schedule
        //---------------------

        /// @brief Set net earliest start time and latest finish time.
        ///        A network should be properly scheduled before it can be simulated
        /// @param an_initial_time an initial time
        /// @param a_terminal_time a terminal time
        void schedule(const duration& an_initial_time, const duration& a_terminal_time)
        {
            __initial_time = an_initial_time;
            __terminal_time = a_terminal_time;
        };

        /// @brief Get the network scheduled earliest start time
        /// @return an initial duration value
        duration initial_time() const
        {
            return __initial_time;
        };

        /// @brief Get the network scheduled earliest start time
        /// @return a terminal duration value
        duration terminal_time() const
        {
            return __terminal_time;
        };
        
        /// @brief Get the slack of an activity
        /// @param an_activity activity object value which slack is computed
        /// @return slack (duration) of the activity
        duration activity_float(const activity& an_activity) const
        {
            return earliest_occurence(an_activity.completion_event()) - earliest_finish(an_activity);
        };

        /// @brief Get the free float of an activity
        /// @param an_activity activity object value which slack is computed
        /// @return free float (duration) of the activity
        duration free_float(const activity& an_activity) const
        {
            return latest_occurence(an_activity.completion_event()) - earliest_finish(an_activity);
        };

        /// @brief Get the interfering float of an activity
        /// @param an_activity id of the activity which interfering float is computed
        /// @return interfering float (duration) of the activity
        duration interfering_float(const activity& an_activity) const
        {
            return std::max(0, earliest_occurence(an_activity.completion_event()) - latest_occurence(an_activity.trigger_event()) - estimated_duration(an_activity));
        };

        /// @brief Get the independent float of an activity
        /// @param an_activity id of the activity which independent float is computed
        /// @return independent float of the activity
        duration independent_float(const activity& an_activity) const
        {
            return free_float(an_activity) - activity_float(an_activity);
        };

        // - forward pass
        
        /// @brief Get the earliest occurence of an event
        /// @param an_event event id
        /// @return earliest occurence date
        duration earliest_occurence(const event& an_event) const
        {
            // initial event's occurence time is given by schedule
            event _initial_event = *initial_events().begin();
            if (an_event == _initial_event) 
                return __initial_time;

            // max earliest finish of incoming activities
            std::set<activity> _incoming_activities = incoming_activities(an_event);
            std::list<duration> _earliest_finish_times;
            for (const auto& a: _incoming_activities)
            {
                _earliest_finish_times.push_back(earliest_finish(a));
            }
            return *std::max_element(_earliest_finish_times.begin(), _earliest_finish_times.end());
        };
        
        /// @brief Get the earliest finish date of an activity
        /// @param an_activity activity object value which earliest finish date is to be computed
        /// @return earliest finish date of the parameter activity
        duration earliest_finish(const activity& an_activity) const
        {
            // earliest occurence of trigger + estimated duration
            return earliest_occurence(an_activity.trigger_event()) + estimated_duration(an_activity);
        };

        // - backward pass
        
        /// @brief Get the latest occurence date of an event
        /// @param an_event event id
        /// @return latest occurence date of the parameter event
        duration latest_occurence(const event& an_event) const
        {
            // terminal event's occurence time is given by schedule
            event _terminal_event = *terminal_events().begin();
            if (an_event == _terminal_event)
            {
                return __terminal_time;
            }

            // min latest start of outgoing activities
            std::set<activity> _outgoing_activities = outgoing_activities(an_event);
            std::list<duration> _latest_start_times;
            for (const auto& a: _outgoing_activities)
            {
                _latest_start_times.push_back(latest_start(a));
            }
            return *std::min_element(_latest_start_times.begin(), _latest_start_times.end());
        };
        
        /// @brief Get the latest start of an activity
        /// @param an_activity activity object value which latest start date is to be computed
        /// @return latest start date of parameter activity
        duration latest_start(const activity& an_activity) const
        {
            // latest occurence of completion - estimated duration
            return latest_occurence(an_activity.completion_event()) - estimated_duration(an_activity);
        };

        // critical path
        //---------------------

        /// @brief Find the critical path of the network
        /// @return list of activity segments ordered by precedence
        std::vector<segment> find_critical_path() const
        {
            // Copy this network
            network a_network = *this;
            // Tight schedule
            a_network.schedule(0, 1);
            a_network.schedule(0, a_network.earliest_occurence(*terminal_events().begin()));
            // Take activities with no free float
            std::vector<segment> _critical_path;
            std::copy_if(__data.cbegin(), __data.cend(), std::inserter(_critical_path, _critical_path.end()), [a_network](const auto& segment)
            {
                return a_network.free_float(segment.first) == 0; 
            });
            std::sort(_critical_path.begin(), _critical_path.end(), [](const auto& s1, const auto& s2){ return s1.first < s2.first; });
            return _critical_path;
        };

        /// @brief Get displayable string of an activity segment
        /// @param a_segment the segment object to be stringified
        /// @return a string representing the activity segment
        static std::string to_str(const segment& a_segment)
        {
            std::stringstream str_stream; 
            str_stream << "[" << a_segment.first.trigger_event() << "]" << " --=" << a_segment.second << "=--> " << "[" << a_segment.first.completion_event() << "]";
            return str_stream.str();
        }

        /// @brief Get list of paths between two events
        /// @param a_start_event the start event of the paths
        /// @param a_finish_event the finish event of the paths
        /// @return a list of paths from a_start_event to a_finish_event
        std::vector<path> paths(const event& a_start_event, const event& a_finish_event)
        {
            return paths(path({}), a_start_event, a_finish_event);
        }

        /// @brief Assuming a partial path has already been found up until a current event, list eventual paths starting with the partial path and leading to a finish event.
        /// @param a_partial_path path already browsed up to the current event
        /// @param the_current_event the completion event of the partial path's last activity segment
        /// @param a_finish_event the finish event aimed by the partial path
        /// @return the list of paths from the current event to the finish event prepended with the partial path
        std::vector<path> paths(const path a_partial_path, const event& the_current_event, const event& a_finish_event) const
        {
            // TODO: check that the current event is the last segment's completion event

            std::vector<path> _paths;

            for (const auto& a: outgoing_activities(the_current_event))
            {
                segment next_segment = std::make_pair(a, estimated_duration(a));
                
                // next segment creates loop
                bool next_segment_creates_loop = false;
                for (const auto& s: a_partial_path)
                {
                    if (next_segment.first.completion_event() == s.first.trigger_event())
                    {
                        //std::cout << next_segment.first.completion_event() << "==" << s.first.trigger_event() << std::endl;
                        next_segment_creates_loop = true;
                        break;
                    }
                }
                if (next_segment_creates_loop)
                {
                    continue;
                }

                // next segment leads to finish event
                if (next_segment.first.completion_event() == a_finish_event)
                {
                    path complete_path { a_partial_path };
                    complete_path.push_back(next_segment);
                    _paths.push_back(complete_path);
                    return _paths;
                }

                // next segment stacks on the partial path
                path next_partial_path { a_partial_path };
                next_partial_path.push_back(next_segment);
                std::vector<path> paths_through = paths(next_partial_path, next_segment.first.completion_event(), a_finish_event);
                for (const auto& p: paths_through)
                {
                    _paths.push_back(p);
                }
            }

            return _paths;
        };

        /// @brief Get the list of loops in between two events of the network
        /// @param a_start_event the start event 
        /// @param a_finish_event the finish event
        /// @return list of loop paths found in between two events
        std::vector<path> loop_paths(const event& a_start_event, const event& a_finish_event) const
        {
            return loop_paths(path({}), a_start_event, a_finish_event);
        }

        /// @brief Assuming a partial path has already been found up until a current event, list eventual loop paths starting with the partial path.
        /// @param a_partial_path path already browsed up to the current event (has no loops and has not reach finish event)
        /// @param the_current_event the completion event of the partial path's last activity segment 
        /// @param a_finish_event the finish event of the loop paths search
        /// @return list of loop paths in between the current event and the finish event
        std::vector<path> loop_paths(const path& a_partial_path, const event& the_current_event, const event& a_finish_event) const
        {
            std::vector<path> _paths;

            for (const auto& a: outgoing_activities(the_current_event))
            {
                segment next_segment = std::make_pair(a, estimated_duration(a));
                
                // next segment creates loop
                for (const auto& s: a_partial_path)
                {
                    if (next_segment.first.completion_event() == s.first.trigger_event())
                    {
                        path loop_path { a_partial_path };
                        loop_path.push_back(next_segment);
                        _paths.push_back(loop_path);
                        return _paths;
                    }
                }

                // next segment leads to finish event
                if (next_segment.first.completion_event() == a_finish_event)
                {
                    return _paths;
                }

                // next segment stacks on the partial path
                path next_partial_path { a_partial_path };
                next_partial_path.push_back(next_segment);
                std::vector<path> loop_paths_through = loop_paths(next_partial_path, next_segment.first.completion_event(), a_finish_event);
                for (const auto& p: loop_paths_through)
                {
                    _paths.push_back(p);
                }
            }

            return _paths;
        };


        // Constructors
        //---------------------

        /// @brief Construct empty network
        network() : __data({}) {};

        /// @brief Construct a network from a list of paths
        /// @param some_paths list of paths to include in network
        network(const std::vector<path>& some_paths) : network()
        {
            for (const path& p: some_paths)
            {
                for (const segment& s: p)
                {
                    this->add_activity(s.first, s.second);
                }
            }
        };

        /// @brief Construct a network from a list of paths and a schedule
        /// @param some_paths list of paths to include in network
        /// @param a_start_time scheduled earliest start time of the network project
        /// @param a_finish_time scheduled latest finish time of the network project
        network(const std::vector<path>& some_paths, const duration& a_start_time, const duration& a_finish_time) : network(some_paths) 
        {
            __initial_time = a_start_time;
            __terminal_time = a_finish_time;
        };
        
        /// @brief Subset of network made of activities in between two events
        /// @param a_start_event event id of the subnet's initial event
        /// @param a_finish_event event id of the subnet's terminal event
        /// @return a partial network
        network subnet(const event& a_start_event, const event& a_finish_event)
        {
            return network(paths(a_start_event, a_finish_event), earliest_occurence(a_start_event), latest_occurence(a_finish_event));
        }

        // automation
        //---------------------
        
        /// @brief create network object from a network description as string
        /// @param txt string representation of network
        /// @return network object
        static network from_txt(const std::string& txt)
        {
            network txt_network;
            
            // Split text into lines and lines into event event duration
            std::string _line;
            char delimiter = '\n';
            std::stringstream txt_stream(txt);
            // Get schedule times
            duration _initial_time;
            duration _terminal_time;
            std::getline(txt_stream, _line, delimiter);
            std::stringstream(_line) >> _initial_time;
            std::getline(txt_stream, _line, delimiter);
            std::stringstream(_line) >> _terminal_time;
            txt_network.schedule(_initial_time, _terminal_time);
            // Get activities
            while (std::getline(txt_stream, _line, delimiter))
            {
                event s, f;
                duration d;
                std::stringstream _line_stream(_line);
                _line_stream >> s >> f >> d;
                txt_network.add_activity(s, f, d);
            }

            return txt_network;
        };
        
    private:

        /// @brief Get the trigger events of the network
        /// @return a set of events
        std::set<event> trigger_events() const
        {
            std::set<event> _trigger_events;
            for (auto& act: __data)
            {
                _trigger_events.emplace(act.first.trigger_event());
            }
            return _trigger_events;
        };

        /// @brief Get the completion events of the network
        /// @return a set of events
        std::set<event> completion_events() const
        {
            std::set<event> _completion_events;
            for (auto& act: __data)
            {
                _completion_events.emplace(act.first.completion_event());
            }
            return _completion_events;
        };

        /// @brief Get the activities that lead to a certain event
        /// @param an_event the event which predecessor activities are returned
        /// @return a set of activity objects
        std::set<activity> incoming_activities(const event& an_event) const
        {
            auto _activities = activities();
            std::set<activity> _incoming_activities;
            std::copy_if(_activities.cbegin(), _activities.cend(), std::inserter(_incoming_activities, _incoming_activities.end()), [an_event](const activity& a)
            {
                return a.precedes(an_event); 
            });
            return _incoming_activities;
        };

        /// @brief Get the activities preceded by a certain event
        /// @param an_event the event which next activities are returned
        /// @return a set of activity objects
        std::set<activity> outgoing_activities(const event& an_event) const
        {
            auto _activities = activities();
            std::set<activity> _outgoing_activities;
            std::copy_if(_activities.cbegin(), _activities.cend(), std::inserter(_outgoing_activities, _outgoing_activities.end()), [an_event](const activity& a)
            {
                return a.follows(an_event); 
            });
            return _outgoing_activities;
        };

    // data members
    private:
        std::map<activity, duration> __data;
        duration __initial_time;
        duration __terminal_time;
        
    };

    template<typename EventIDType, typename DurationType>
    bool operator<(const typename network<EventIDType, DurationType>::activity& a1, const typename network<EventIDType, DurationType>::activity& a2)
    {
        return a1 < a2;
    }

    template<typename EventIDType, typename DurationType>
    bool operator==(const typename network<EventIDType, DurationType>::activity& a1, const typename network<EventIDType, DurationType>::activity& a2)
    {
        return a1 == a2;
    }


} // namespace pert