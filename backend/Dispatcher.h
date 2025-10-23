#pragma once
#include "Courier.h"
#include "Office.h"
#include <iostream>
#include <algorithm>
#include <array>
#include <vector>
#include <queue>

struct TripEvent {
    int day = 0;
    int courier = -1;
    int courier_id = -1;
    int from_of = -1;
    int to_of = -1;
    int t_begin = 0;
    int t_end = 0;
    int letter_id = -1;
    bool deadhead = false;
};

class Dispatcher
{
public:
    Dispatcher(int off, int cour, int s);
    ~Dispatcher() {}

    std::queue<Letter> get_delivered_letters();
    std::queue<Letter> get_ordered_letters();
    std::queue<Letter> get_waiting_letters();

    int     get_distance(int a, int b);
    Office  get_ofice(int n);
    Courier get_courier(int n);

    int get_num_free_rides();
    int get_sum_of_deviations();
    int get_free_rides_time();

    const std::vector<TripEvent>& get_day_events(int day) const { return day_events_[day]; }
    const std::vector<TripEvent>& get_event_log(int day) const { return day_events_[day]; }
    const std::vector<TripEvent>& get_event_log() const { return all_events_; }

    void lett_deliver(Courier& cour, int t);
    void equal_begin_time(Courier& cour);

    void programm_work();

private:
    std::queue<Letter> letters;
    std::queue<Letter> letters_delivered;
    std::queue<Letter> wait_for_take;

    std::vector<Courier> couriers;
    std::vector<Office>  offices;
    std::vector<std::vector<int>> ways;

    int num_free_rides, sum_of_deviations, letters_cnt, free_rides_time;
    int num_of_offices, num_of_couriers, step;

    std::array<std::vector<TripEvent>, 7> day_events_;
    std::vector<TripEvent> all_events_;

    int pick_best_free_courier_for_office(int office) const;
    void assign_letter_to_courier(int day, int cur_time, Letter& L, int courier_idx);
    void assign_deadhead_to_courier(int day, int cur_time, int courier_idx, int from_office, int to_office);
};
