/***
 * @brief 
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

namespace pert
{

    template<typename EventIDType, typename DurationType>
    class network
    {
    public:

        /// @brief 
        using event = EventIDType;
        using duration = DurationType;

        /// @brief 
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

        /// @brief 
        using segment = std::pair<activity, duration>;
        using path = std::vector<segment>;
        // TODO: find a way to guarantee that a path is correctly ordered

        /// @brief 
        struct schedule
        {
            duration initial_time;
            duration terminal_time;
            schedule(duration an_initial_time, duration a_terminal_time) : initial_time(an_initial_time), terminal_time(a_terminal_time) {}
        };

    public:
        /// @brief 
        /// @return 
        std::set<activity> activities() const
        {
            std::set<activity> _activities;
            for (const auto& a: __data)
                _activities.emplace(a.first);

            return _activities;
        };
        
        /// @brief 
        /// @param a_trigger_event 
        /// @param a_completion_event 
        /// @param a_duration 
        /// @return 
        network& add_activity(const event& a_trigger_event, const event& a_completion_event, const duration& a_duration)
        {
            return add_activity(activity(a_trigger_event, a_completion_event), a_duration);
        };
        
        /// @brief 
        /// @param an_activity 
        /// @param a_duration 
        /// @return 
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
        
        /// @brief 
        /// @param a_trigger_event 
        /// @param a_completion_event 
        /// @return 
        network& delete_activity(const event& a_trigger_event, const event& a_completion_event)
        {
            return delete_activity(activity(a_trigger_event, a_completion_event));
        };
        
        /// @brief 
        /// @param an_activity 
        /// @return 
        network& delete_activity(const activity& an_activity)
        {
            __data.erase(an_activity);
            return *this;
        };
        
        /// @brief 
        /// @param an_activity 
        /// @return 
        duration estimated_duration(const activity& an_activity) const
        {
            // DEBUG ?
            auto search_activity = __data.find(an_activity);
            if (search_activity == __data.end())
                return -1;

            return __data.at(an_activity);
        };

        /// @brief 
        /// @param an_activity 
        /// @param a_duration 
        void set_estimated_duration(const activity& an_activity, const duration& a_duration)
        {
            __data[an_activity] = a_duration;
        };

        /// @brief 
        /// @return 
        bool is_well_formed() const
        {
            return initial_events().size() == 1 and terminal_events().size() == 1;
        };

        /// @brief 
        /// @return 
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

        /// @brief 
        /// @return 
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

        /// @brief 
        /// @param an_initial_time 
        /// @param a_terminal_time 
        void schedule(const duration& an_initial_time, const duration& a_terminal_time)
        {
            __initial_time = an_initial_time;
            __terminal_time = a_terminal_time;
        };

        /// @brief 
        /// @return 
        duration initial_time() const
        {
            return __initial_time;
        };

        /// @brief 
        /// @return 
        duration terminal_time() const
        {
            return __terminal_time;
        };
        
        /// @brief 
        /// @param an_activity 
        /// @return 
        duration activity_float(const activity& an_activity) const
        {
            return earliest_occurence(an_activity.completion_event()) - earliest_finish(an_activity);
        };

        /// @brief 
        /// @param an_activity 
        /// @return 
        duration free_float(const activity& an_activity) const
        {
            return latest_occurence(an_activity.completion_event()) - earliest_finish(an_activity);
        };

        /// @brief 
        /// @param an_activity 
        /// @return 
        duration interfering_float(const activity& an_activity) const
        {
            return std::max(0, earliest_occurence(an_activity.completion_event()) - latest_occurence(an_activity.trigger_event()) - estimated_duration(an_activity));
        };

        /// @brief 
        /// @param an_activity 
        /// @return 
        duration independent_float(const activity& an_activity) const
        {
            return free_float(an_activity) - activity_float(an_activity);
        };

        // - forward pass
        
        /// @brief 
        /// @param an_event 
        /// @return 
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
        
        /// @brief 
        /// @param an_activity 
        /// @return 
        duration earliest_finish(const activity& an_activity) const
        {
            // earliest occurence of trigger + estimated duration
            return earliest_occurence(an_activity.trigger_event()) + estimated_duration(an_activity);
        };

        // - backward pass
        
        /// @brief 
        /// @param an_event 
        /// @return 
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
        
        /// @brief 
        /// @param an_activity 
        /// @return 
        duration latest_start(const activity& an_activity) const
        {
            // latest occurence of completion - estimated duration
            return latest_occurence(an_activity.completion_event()) - estimated_duration(an_activity);
        };

        // critical path

        /// @brief 
        /// @return 
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

        static std::string to_str(const segment& s)
        {
            std::stringstream str_stream; 
            str_stream << "[" << s.first.trigger_event() << "]" << " --=" << s.second << "=--> " << "[" << s.first.completion_event() << "]";
            return str_stream.str();
        }

        std::vector<path> paths(const event& a_start_event, const event& a_finish_event)
        {
            return paths(path({}), a_start_event, a_finish_event);
        }

        std::vector<path> paths(const path a_partial_path, const event& a_start_event, const event& a_finish_event)
        {
            std::vector<path> _paths;

            for (const auto& a: outgoing_activities(a_start_event))
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


        // Constructors

        network() : __data({}) {};

        /// @brief 
        /// @param some_paths 
        network(const std::vector<path>& some_paths)
        {
            network _n;
            for (const path& p: some_paths)
            {
                for (const segment& s: p)
                {
                    _n.add_activity(s.first, s.second);
                }
            }
        };

        /// @brief 
        /// @param some_paths 
        /// @param a_start_time 
        /// @param a_finish_time 
        network(const std::vector<path>& some_paths, const duration& a_start_time, const duration& a_finish_time) : network(some_paths) 
        {
            __initial_time = a_start_time;
            __terminal_time = a_finish_time;
        };
        
        /// @brief 
        /// @param a_start_event 
        /// @param a_finish_event 
        /// @return 
        network subnet(const event& a_start_event, const event& a_finish_event)
        {
            return network(paths(a_start_event, a_finish_event), earliest_occurence(a_start_event), latest_occurence(a_finish_event));
        }

        // automation
        
        /// @brief 
        /// @param txt 
        /// @return 
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

        /// @brief 
        /// @return 
        std::set<event> trigger_events() const
        {
            std::set<event> _trigger_events;
            for (auto& act: __data)
            {
                _trigger_events.emplace(act.first.trigger_event());
            }
            return _trigger_events;
        };

        /// @brief 
        /// @return 
        std::set<event> completion_events() const
        {
            std::set<event> _completion_events;
            for (auto& act: __data)
            {
                _completion_events.emplace(act.first.completion_event());
            }
            return _completion_events;
        };

        /// @brief 
        /// @param an_event 
        /// @return 
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

        /// @brief 
        /// @param an_event 
        /// @return 
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