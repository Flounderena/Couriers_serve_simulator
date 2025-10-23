#include "Dispatcher.h"

Dispatcher::Dispatcher(int off, int cour, int st)
    : num_of_offices(off), num_of_couriers(cour), step(st),
    num_free_rides(0), sum_of_deviations(0), letters_cnt(0), free_rides_time(0) {
    ways.resize(num_of_offices, std::vector<int>(num_of_offices, 0));
    for (int i = 0; i < num_of_couriers; ++i) {
        Courier c((rand() % num_of_offices));
        couriers.push_back(c);
    }
    for (int i = 0; i < num_of_offices; ++i) {
        Office o(rand() % 30, rand() % 30);
        offices.push_back(o);
    }
    for (int i = 0; i < num_of_offices; ++i) {
        for (int j = 0; j < num_of_offices; ++j) {
            if (i != j && i >= 0) {
                // патч перевод Ђрассто€ни€ї в ћ»Ќ”“џ (ср. скорость ~30 км/ч)
                double dist_units = offices[i].get_distance(offices[j]); // условные единицы
                const double km_per_unit = 0.25; // калибровка сетки
                const double km_per_min = 0.5;  // 30 км/ч
                int mean_minutes = std::max(1, (int)std::lround((dist_units * km_per_unit) / km_per_min));
                ways[i][j] = mean_minutes;
                ways[j][i] = mean_minutes;
            }
        }
    }
}

std::queue<Letter> Dispatcher::get_delivered_letters() {
    return letters_delivered;
}

std::queue<Letter> Dispatcher::get_ordered_letters() {
    return letters;
}

std::queue<Letter> Dispatcher::get_waiting_letters() {
    return wait_for_take;
}

int Dispatcher::get_distance(int a, int b) {
    return ways[a][b];
}

Office Dispatcher::get_ofice(int n) {
    return offices[n];
}

Courier Dispatcher::get_courier(int n) {
    return couriers[n];
}

int Dispatcher::get_num_free_rides() {
    return num_free_rides;
}

int Dispatcher::get_sum_of_deviations() {
    return sum_of_deviations;
}

int Dispatcher::get_free_rides_time() {
    return free_rides_time;
}

void Dispatcher::lett_deliver(Courier& cour, int t) {
    while (cour.get_num_of_let() != 0 && cour.first_let().get_t_end() <= t) {
        if (cour.first_let().get_order_time() >= 0) {
            letters_delivered.push(cour.first_let());
            sum_of_deviations += std::max(0, cour.first_let().get_t_end() - cour.first_let().get_deadline());
        }
        cour.set_pos(cour.first_let().get_end_of());
        cour.deliver_let();
    }
    for (int i = 0; i < cour.get_num_of_let(); ++i) {
        cour.first_let().set_beg_of(cour.get_pos());
        cour.move_to_end();
    }
}


void Dispatcher::equal_begin_time(Courier& cour) {
    std::vector<int> latest_time(num_of_offices, -1);
    int new_time = 0;
    int prev_time = 0;
    int mx_t = 0;
    for (int i = 0; i < cour.get_num_of_let(); ++i) {
        latest_time[cour.first_let().get_end_of()] = 1;
        mx_t = std::max(mx_t, latest_time[cour.first_let().get_end_of()]);
        cour.move_to_end();
    }
    for (int i = 0; i < num_of_offices; ++i) {
        if (latest_time[i] > -1) {
            new_time = std::max(mx_t, prev_time) + ways[cour.first_let().get_beg_of()][i] + (rand() % 36 - 5);
            for (int j = 0; j < cour.get_num_of_let(); ++j) {
                if (cour.first_let().get_end_of() == i) {
                    cour.first_let().set_t_begin(mx_t);
                    cour.first_let().set_t_end(new_time);
                }
                cour.move_to_end();
            }
            prev_time = new_time;
        }
    }
}


void Dispatcher::programm_work() {
    int num_of_periods = 9 * 60 / step;
    int num_of_letters = 0, from = 0, to = 0, letter_office = 0, time_of_ride = 0, min_time = -1, road_time = 0, sec_road_time = 0, delta = 0, target_of = 0, cur_time = 0;
    bool faster = false, all_are_busy = true, nobody_to_send = false;
    double letter_frequency = 0, frequency_coef = 0.5;

    // PATCH: простые веса офисов дл€ неравномерности (чем индекс меньше Ч тем Ђт€желееї)
    std::vector<int> office_weights(num_of_offices);
    for (int i = 0; i < num_of_offices; ++i) office_weights[i] = 1 + (i % 3); // 1..3

    auto weighted_office = [&]() {
        int sum = 0;
        for (int w : office_weights) sum += w;
        int r = rand() % sum;
        int acc = 0;
        for (int i = 0; i < num_of_offices; ++i) {
            acc += office_weights[i];
            if (r < acc) return i;
        }
        return num_of_offices - 1;
        };

    for (int day = 0; day < 7; ++day) {
        for (int period = 0; period <= num_of_periods; ++period) {
            cur_time = period * step;

            // PATCH: генераци€ за€вок через интервалы 2..20 мин в пределах окна периода
            if (period < num_of_periods) {
                int window_start = cur_time;
                int window_end = std::min(540, cur_time + step);

                int t = window_start;
                while (t < window_end) {
                    // пик в середине дн€: чем ближе к 300, тем меньше интервал
                    int dist_to_peak = std::abs(300 - t);
                    double k = 1.0 / std::max(1, dist_to_peak / 30); // 1..~30 -> 1..~1
                    // базовый интервал 2..20
                    int gap = 2 + (rand() % 19);
                    int adj_gap = std::max(1, (int)std::lround(gap / std::clamp(0.5 * k + 0.5, 0.5, 2.0)));
                    t += adj_gap;
                    if (t >= window_end) break;

                    ++letters_cnt;
                    from = weighted_office();
                    to = weighted_office();
                    if (to == from) to = (to + 1) % num_of_offices;

                    // дедлайн: в пределах дн€ Ч от 40 до 240 мин после заказа (но не позже 18:00)
                    int deadline = std::min(540, t + (40 + (rand() % 201)));

                    Letter l(from, to, t, deadline, letters_cnt);
                    letters.push(l);
                }
            }

            // (остальной код блока Ч как у теб€; ниже Ч только патчи про шум времени)

            if (wait_for_take.size() > 0) {
                for (int i = 0; i < wait_for_take.size(); ++i) {
                    wait_for_take.push(wait_for_take.front());
                    wait_for_take.pop();
                }
            }

            if (letters.size() > 0) {
                for (int i = 0; i < letters.size(); ++i) {
                    letters.push(letters.front());
                    letters.pop();
                }
            }

            for (int c = 0; c < num_of_couriers; ++c) {
                lett_deliver(couriers[c], cur_time);
            }

            if (wait_for_take.size() > 0) {
                for (int i = 0; i < num_of_couriers && wait_for_take.size() > 0; ++i) {
                    if (couriers[i].is_free() && wait_for_take.size() > 0) {
                        min_time = -1;
                        for (int j = 0; j < wait_for_take.size(); ++j) {
                            letter_office = wait_for_take.front().get_beg_of();

                            if (couriers[i].get_pos() == letter_office && (min_time == -1 || min_time + 5 <= wait_for_take.front().get_order_time())) {
                                if (min_time == -1) min_time = std::max(cur_time, wait_for_take.front().get_order_time());
                                wait_for_take.front().set_t_begin(std::max(cur_time, wait_for_take.front().get_order_time()));
                                // PATCH: шум добавл€етс€ к ¬–≈ћ≈Ќ» (ways Ч в минутах)
                                delta = (rand() % 36) - 5; // [-5..30]
                                time_of_ride = std::max(1, ways[wait_for_take.front().get_beg_of()][wait_for_take.front().get_end_of()] + delta);
                                wait_for_take.front().set_t_end(std::max(cur_time, wait_for_take.front().get_t_begin()) + time_of_ride);
                                couriers[i].take_let(wait_for_take.front());
                                wait_for_take.pop();
                            }
                            else {
                                wait_for_take.push(wait_for_take.front());
                                wait_for_take.pop();
                            }
                        }
                        for (int j = 0; j < letters.size(); ++j) {
                            letter_office = letters.front().get_beg_of();
                            if (couriers[i].get_pos() == letter_office && (min_time == -1 || min_time + 5 < letters.front().get_order_time())) {
                                letters.front().set_t_begin(std::max(cur_time, letters.front().get_order_time()));
                                // PATCH: шум к времени
                                delta = (rand() % 36) - 5; // [-5..30]
                                time_of_ride = std::max(1, ways[letters.front().get_beg_of()][letters.front().get_end_of()] + delta);
                                letters.front().set_t_end(std::max(cur_time, letters.front().get_t_begin()) + time_of_ride);
                                couriers[i].take_let(letters.front());
                                nobody_to_send = false;
                                letters.pop();
                            }
                            else {
                                letters.push(letters.front());
                                letters.pop();
                            }
                        }
                        if (min_time != -1) equal_begin_time(couriers[i]);
                    }
                }
            }
            all_are_busy = true;
            nobody_to_send = false;
            while (letters.size() > 0 && all_are_busy && !nobody_to_send) {
                all_are_busy = false;
                nobody_to_send = true;
                for (int i = 0; i < num_of_couriers && letters.size() > 0; ++i) {
                    if (couriers[i].is_free()) {
                        all_are_busy = true;
                        min_time = -1;
                        for (int j = 0; j < letters.size(); ++j) {
                            letter_office = letters.front().get_beg_of();
                            if (couriers[i].get_pos() == letter_office && (min_time == -1 || min_time + 5 < letters.front().get_order_time())) {
                                if (min_time == -1) min_time = std::max(cur_time, letters.front().get_order_time());
                                letters.front().set_t_begin(std::max(cur_time, letters.front().get_order_time()));
                                // PATCH: шум к времени
                                delta = (rand() % 36) - 5;
                                time_of_ride = std::max(1, ways[letters.front().get_beg_of()][letters.front().get_end_of()] + delta);
                                letters.front().set_t_end(std::max(cur_time, letters.front().get_t_begin()) + time_of_ride);
                                couriers[i].take_let(letters.front());
                                nobody_to_send = false;
                                letters.pop();
                            }
                            else {
                                letters.push(letters.front());
                                letters.pop();
                            }
                        }
                        if (min_time != -1) equal_begin_time(couriers[i]);
                    }

                    if (couriers[i].is_free() && letters.size() > 0) {
                        for (int l = 0; l < letters.size(); ++l) {
                            all_are_busy = true;
                            road_time = ways[couriers[i].get_pos()][letters.front().get_beg_of()];
                            delta = rand() % (36) - 5;
                            bool faster = false;
                            for (int k = 0; k < num_of_couriers; ++k) {
                                if (i != k) {
                                    if (couriers[k].get_pos() == letters.front().get_beg_of() && couriers[k].is_free()) {
                                        faster = true; break;
                                    }
                                    sec_road_time = ways[couriers[k].get_pos()][letters.front().get_beg_of()];
                                    if (road_time > sec_road_time && couriers[k].is_free()) {
                                        faster = true; break;
                                    }
                                    if (couriers[k].get_num_of_let() > 0) {
                                        if (couriers[k].last_let().get_end_of() == letters.front().get_beg_of() && couriers[k].last_let().get_t_end() < cur_time + road_time + delta) {
                                            faster = true; break;
                                        }
                                    }
                                }
                            }
                            if (!faster) {
                                nobody_to_send = false;
                                ++num_free_rides;
                                free_rides_time += std::max(1, road_time + delta); // PATCH: не даЄм уйти в 0/отриц.
                                Letter l(couriers[i].get_pos(), letters.front().get_beg_of(), -1, -1, -1);
                                l.set_t_begin(cur_time);
                                l.set_t_end(cur_time + std::max(1, road_time + delta));
                                couriers[i].take_let(l);
                                target_of = letters.front().get_beg_of();
                                for (int i2 = 0; i2 < letters.size(); ++i2) {
                                    if (letters.front().get_beg_of() == target_of) {
                                        wait_for_take.push(letters.front());
                                        letters.pop();
                                    }
                                    else {
                                        letters.push(letters.front());
                                        letters.pop();
                                    }
                                }
                            }

                            if (letters.size() > 0) {
                                letters.push(letters.front());
                                letters.pop();
                            }
                        }
                    }
                }
            }
        }

        for (int c = 0; c < num_of_couriers; ++c) {
            lett_deliver(couriers[c], 600);
        }

        if (wait_for_take.size() > 0) {
            for (int i = 0; i < wait_for_take.size(); ++i) {
                wait_for_take.push(wait_for_take.front());
                wait_for_take.pop();
            }
        }
        if (letters.size() > 0) {
            for (int i = 0; i < letters.size(); ++i) {
                letters.push(letters.front());
                letters.pop();
            }
        }
        std::cout << "THE END OF THE " << day << " DAY" << std::endl;
    }

    // PATCH: итогова€ статистика по “«
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
}
