#include "Dispatcher.h"
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <random>

namespace {
    constexpr int DAY_MIN = 9 * 60;
    inline int clamp_min(int v, int mn) { return v < mn ? mn : v; }
}

Dispatcher::Dispatcher(int off, int cour, int st)
    : num_of_offices(off), num_of_couriers(cour), step(st),
    num_free_rides(0), sum_of_deviations(0), letters_cnt(0), free_rides_time(0)
{
    ways.resize(num_of_offices, std::vector<int>(num_of_offices, 0));

    for (int i = 0; i < num_of_couriers; ++i) couriers.emplace_back(rand() % num_of_offices);
    for (int i = 0; i < num_of_offices; ++i) offices.emplace_back(rand() % 30, rand() % 30);

    for (int i = 0; i < num_of_offices; ++i) {
        for (int j = 0; j < num_of_offices; ++j) {
            if (i == j) { ways[i][j] = 0; continue; }
            double dist_units = offices[i].get_distance(offices[j]);
            const double km_per_unit = 0.25;
            const double km_per_min = 0.5;
            int mean_minutes = std::max(1, (int)std::lround((dist_units * km_per_unit) / km_per_min));
            ways[i][j] = ways[j][i] = mean_minutes;
        }
    }
}

std::queue<Letter> Dispatcher::get_delivered_letters() { return letters_delivered; }
std::queue<Letter> Dispatcher::get_ordered_letters() { return letters; }
std::queue<Letter> Dispatcher::get_waiting_letters() { return wait_for_take; }
int Dispatcher::get_distance(int a, int b) { return ways[a][b]; }
Office Dispatcher::get_ofice(int n) { return offices[n]; }
Courier Dispatcher::get_courier(int n) { return couriers[n]; }
int Dispatcher::get_num_free_rides() { return num_free_rides; }
int Dispatcher::get_sum_of_deviations() { return sum_of_deviations; }
int Dispatcher::get_free_rides_time() { return free_rides_time; }

void Dispatcher::lett_deliver(Courier& cour, int t)
{
    while (cour.get_num_of_let() != 0) {
        Letter f = cour.first_let();
        if (f.get_letter_num() == -1) break;
        if (f.get_t_end() > t) break;

        if (f.get_order_time() >= 0) {
            letters_delivered.push(f);
            sum_of_deviations += std::max(0, f.get_t_end() - f.get_deadline());
        }
        cour.set_pos(f.get_end_of());
        cour.deliver_let();
    }
}

void Dispatcher::equal_begin_time(Courier& cour)
{
    std::vector<int> latest_time(num_of_offices, -1);
    int new_time = 0, prev_time = 0, mx_t = 0;

    int cnt = cour.get_num_of_let();
    for (int i = 0; i < cnt; ++i) {
        Letter f = cour.first_let();
        if (f.get_letter_num() == -1) return;
        latest_time[f.get_end_of()] = 1;
        mx_t = std::max(mx_t, latest_time[f.get_end_of()]);
        cour.move_to_end();
    }
    for (int i = 0; i < num_of_offices; ++i) {
        if (latest_time[i] > -1) {
            Letter f = cour.first_let();
            if (f.get_letter_num() == -1) break;
            new_time = std::max(mx_t, prev_time)
                + ways[f.get_beg_of()][i]
                + (rand() % 36 - 5);
            for (int j = 0; j < cnt; ++j) {
                Letter ff = cour.first_let();
                if (ff.get_letter_num() == -1) break;
                if (ff.get_end_of() == i) {
                    cour.first_let().set_t_begin(mx_t);
                    cour.first_let().set_t_end(new_time);
                }
                cour.move_to_end();
            }
            prev_time = new_time;
        }
    }
}

int Dispatcher::pick_best_free_courier_for_office(int office) const
{
    int best = -1;
    int best_time = 1e9;
    for (int i = 0; i < num_of_couriers; ++i) {
        if (!couriers[i].is_free()) continue;
        int pos = couriers[i].get_pos();
        int t = ways[pos][office];
        if (t < best_time || (t == best_time && rand() % 2)) {
            best_time = t;
            best = i;
        }
    }
    return best;
}

void Dispatcher::assign_letter_to_courier(int day, int cur_time, Letter& L, int courier_idx)
{
    L.set_t_begin(std::max(cur_time, L.get_order_time()));
    int delta = (rand() % 36) - 5;
    int time_of_ride = clamp_min(ways[L.get_beg_of()][L.get_end_of()] + delta, 1);
    L.set_t_end(std::max(cur_time, L.get_t_begin()) + time_of_ride);

    day_events_[day].push_back(TripEvent{
        day, courier_idx, courier_idx, L.get_beg_of(), L.get_end_of(),
        L.get_t_begin(), L.get_t_end(), L.get_letter_num(), false
        });

    couriers[courier_idx].take_let(L);
}

void Dispatcher::assign_deadhead_to_courier(int day, int cur_time, int courier_idx, int from_office, int to_office)
{
    int delta = (rand() % 36) - 5;
    int ride_minutes = clamp_min(ways[from_office][to_office] + delta, 1);
    ++num_free_rides;
    free_rides_time += ride_minutes;

    day_events_[day].push_back(TripEvent{
        day, courier_idx, courier_idx, from_office, to_office,
        cur_time, cur_time + ride_minutes, -1, true
        });

    Letter dead(from_office, to_office, -1, -1, -1);
    dead.set_t_begin(cur_time);
    dead.set_t_end(cur_time + ride_minutes);
    couriers[courier_idx].take_let(dead);
}

void Dispatcher::programm_work()
{
    const int num_of_periods = DAY_MIN / step;

    int from = 0, to = 0;
    int cur_time = 0;

    std::vector<int> office_weights(num_of_offices);
    for (int i = 0; i < num_of_offices; ++i) office_weights[i] = 1 + (i % 3);

    auto weighted_office = [&]() {
        int sum = 0; for (int w : office_weights) sum += w;
        int r = rand() % sum, acc = 0;
        for (int i = 0; i < num_of_offices; ++i) { acc += office_weights[i]; if (r < acc) return i; }
        return num_of_offices - 1;
        };

    for (int day = 0; day < 7; ++day) {
        for (int period = 0; period <= num_of_periods; ++period) {
            cur_time = period * step;

            if (period < num_of_periods) {
                int window_start = cur_time;
                int window_end = std::min(DAY_MIN, cur_time + step);

                int t = window_start;
                while (t < window_end) {
                    int dist_to_peak = std::abs(300 - t);
                    double k = 1.0 / std::max(1, dist_to_peak / 30);

                    int gap = 2 + (rand() % 19);
                    int adj_gap = clamp_min((int)std::lround(gap / std::clamp(0.5 * k + 0.5, 0.5, 2.0)), 1);
                    t += adj_gap;
                    if (t >= window_end) break;

                    ++letters_cnt;
                    from = weighted_office();
                    to = weighted_office();
                    if (to == from) to = (to + 1) % num_of_offices;

                    int deadline = std::min(DAY_MIN, t + (40 + (rand() % 201)));
                    letters.emplace(from, to, t, deadline, letters_cnt);
                }
            }

            for (int c = 0; c < num_of_couriers; ++c) lett_deliver(couriers[c], cur_time);

            if (!wait_for_take.empty()) {
                int wcnt = (int)wait_for_take.size();
                for (int k = 0; k < wcnt; ++k) {
                    Letter L = wait_for_take.front(); wait_for_take.pop();
                    int best = pick_best_free_courier_for_office(L.get_beg_of());
                    if (best != -1) assign_letter_to_courier(day, cur_time, L, best);
                    else wait_for_take.push(L);
                }
            }

            if (!letters.empty()) {
                int lcnt = (int)letters.size();
                for (int k = 0; k < lcnt; ++k) {
                    Letter L = letters.front(); letters.pop();
                    int best = pick_best_free_courier_for_office(L.get_beg_of());
                    if (best != -1) assign_letter_to_courier(day, cur_time, L, best);
                    else letters.push(L);
                }
            }

            if (!letters.empty()) {
                int probes = std::min<int>((int)letters.size(), num_of_couriers);
                for (int p = 0; p < probes; ++p) {
                    Letter L0 = letters.front(); letters.pop();

                    int exact = -1;
                    for (int i = 0; i < num_of_couriers; ++i) {
                        if (couriers[i].is_free() && couriers[i].get_pos() == L0.get_beg_of()) { exact = i; break; }
                    }
                    if (exact != -1) {
                        assign_letter_to_courier(day, cur_time, L0, exact);
                        continue;
                    }

                    int best = pick_best_free_courier_for_office(L0.get_beg_of());
                    if (best != -1) {
                        int from_off = couriers[best].get_pos();
                        assign_deadhead_to_courier(day, cur_time, best, from_off, L0.get_beg_of());

                        wait_for_take.push(L0);
                        int cnt = (int)letters.size();
                        for (int i2 = 0; i2 < cnt; ++i2) {
                            Letter tmp = letters.front(); letters.pop();
                            if (tmp.get_beg_of() == L0.get_beg_of()) wait_for_take.push(tmp);
                            else letters.push(tmp);
                        }
                    }
                    else {
                        letters.push(L0);
                    }
                }
            }
        }

        for (int c = 0; c < num_of_couriers; ++c) lett_deliver(couriers[c], 600);

        std::cout << "THE END OF THE " << day << " DAY\n";
        std::cout.flush();
    }

    all_events_.clear();
    for (int d = 0; d < 7; ++d) {
        all_events_.insert(all_events_.end(),
            day_events_[d].begin(),
            day_events_[d].end());
    }

    std::cout << "\n=== STATISTICS ===\n";
    std::cout << "Free rides count: " << num_free_rides << "\n";
    std::cout << "Free rides time (min): " << free_rides_time << "\n";
    std::cout << "Sum lateness (min): " << sum_of_deviations << "\n";

    for (int i = 0; i < num_of_couriers; ++i) {
        std::cout << "Courier #" << i
            << " busy_time(min): " << couriers[i].get_busy_time()
            << " avg_trip(min): " << couriers[i].get_mid_travels()
            << "\n";
    }

    std::cout << "[event counts per day]\n";
    for (int d = 0; d < 7; ++d) {
        std::cout << "  day " << d << ": " << day_events_[d].size() << " events\n";
    }
}
