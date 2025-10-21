#include "Dispatcher.h"

Dispatcher::Dispatcher(int off, int cour, int st) : num_of_offices(off), num_of_couriers(cour), step(st), num_free_rides(0), sum_of_deviations(0), letters_cnt(0) {
    ways.resize(num_of_offices, std::vector<int>(num_of_offices, 0));
    for (int i = 0; i < num_of_couriers; ++i) {
        Courier c(5, (rand() % num_of_offices));
        couriers.push_back(c);
    }
    for (int i = 0; i < num_of_offices; ++i) {
        Office o(rand() % 30, rand() % 30);
        offices.push_back(o);
    }
    for (int i = 0; i < num_of_offices; ++i) {
        for (int j = 0; j < num_of_offices; ++j) {
            if (i != j && i) {
                int l = (offices[i].get_distance(offices[j]));
                ways[i][j] = l;
                ways[j][i] = l;
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

void Dispatcher::lett_deliver(Courier& cour, int t) {
    while (cour.num_of_letters() != 0 && cour.first_let().get_t_end() <= t) {
        if (cour.first_let().get_order_time() >= 0) {
            letters_delivered.push(cour.first_let());
            sum_of_deviations += std::max(0, cour.first_let().get_t_end() - cour.first_let().get_deadline());
        }
        cour.set_pos(cour.first_let().get_end_of());
        cour.deliver_let();
    }
}


void Dispatcher::equal_begin_time(Courier& cour) {
    std::vector<int> latest_time(num_of_offices, -1);
    int new_time = 0;
    int prev_time = 0;
    for (int i = 0; i < cour.get_num_of_let(); ++i) {
        latest_time[cour.first_let().get_end_of()] = std::max(latest_time[cour.first_let().get_end_of()], cour.last_let().get_t_begin());
        cour.move_to_end();
    }
    for (int i = 0; i < num_of_offices; ++i) {
        if (latest_time[i] > -1) {
            new_time = std::max(latest_time[i], prev_time) + ways[cour.first_let().get_beg_of()][i] + (rand() % 36 - 5);
            for (int j = 0; j < cour.get_num_of_let(); ++j) {
                if (cour.first_let().get_end_of() == i) {
                    cour.first_let().set_t_begin(std::max(prev_time, latest_time[i]));
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

    for (int day = 0; day < 7; ++day) {
        std::cout << "num_of_day " << day << std::endl;
        for (int period = 0; period <= num_of_periods; ++period) {
            std::cout << std::endl;
            std::cout << "num_of_period " << period << std::endl;
            cur_time = period * step;
            std::cout << "current_time " << cur_time << std::endl;

            if (period < num_of_periods) {
                letter_frequency = frequency_coef / std::max(1, (abs(300 - cur_time) / 10));
                num_of_letters = (step / 20) * (20 * letter_frequency);

                std::vector<std::pair<int, int>> times(num_of_letters, { 0, 0 });

                for (int i = 0; i < num_of_letters; ++i) {
                    from = rand() % (step - 1) + cur_time;
                    to = std::min(540, rand() % std::max(1, (535 - from + 1)) + from);
                    times[i] = { from, to };
                }

                std::sort(times.begin(), times.end());

                for (int i = 0; i < num_of_letters; ++i) {
                    ++letters_cnt;
                    from = rand() % num_of_offices;
                    to = rand() % num_of_offices;
                    if (to == from) {
                        if (to + 1 < num_of_offices) ++to;
                        else --to;
                    }
                    Letter l(from, to, times[i].first, times[i].second, letters_cnt);
                    letters.push(l);
                }
            }

            std::cout << "extra_letters: " << std::endl;
            if (wait_for_take.size() > 0) {
                for (int i = 0; i < wait_for_take.size(); ++i) {
                    std::cout << wait_for_take.front().get_order_time() << " " << wait_for_take.front().get_deadline() << " " << wait_for_take.front().get_beg_of() << " " << wait_for_take.front().get_end_of() << std::endl;
                    wait_for_take.push(wait_for_take.front());
                    wait_for_take.pop();
                }
            }
            std::cout << std::endl;

            std::cout << "letters: " << std::endl;
            if (letters.size() > 0) {
                for (int i = 0; i < letters.size(); ++i) {
                    std::cout << letters.front().get_order_time() << " " << letters.front().get_deadline() << " " << letters.front().get_beg_of() << " " << letters.front().get_end_of() << std::endl;
                    letters.push(letters.front());
                    letters.pop();
                }
            }
            std::cout << std::endl;


            for (int c = 0; c < num_of_couriers; ++c) {
                lett_deliver(couriers[c], cur_time);
            }

            if (wait_for_take.size() > 0) {
                for (int i = 0; i < num_of_couriers && wait_for_take.size() > 0; ++i) {
                    std::cout << "take_all_letters" << std::endl;
                    if (couriers[i].is_free() && wait_for_take.size() > 0) {
                        min_time = -1;
                        std::cout << "free_courier " << i << std::endl;
                        for (int j = 0; j < wait_for_take.size(); ++j) {
                            letter_office = wait_for_take.front().get_beg_of();

                            if (couriers[i].get_pos() == letter_office && (min_time == -1 || min_time + 5 <= wait_for_take.front().get_order_time())) {
                                if (min_time == -1) min_time = std::max(cur_time, wait_for_take.front().get_order_time());
                                wait_for_take.front().set_t_begin(std::max(cur_time, wait_for_take.front().get_order_time()));
                                time_of_ride = ways[wait_for_take.front().get_beg_of()][wait_for_take.front().get_end_of()] + (rand() % 36 - 5);
                                wait_for_take.front().set_t_end(std::max(cur_time, wait_for_take.front().get_t_begin()) + time_of_ride);
                                couriers[i].take_let(wait_for_take.front());
                                std::cout << i << "  took the letter3" << std::endl;
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
                                time_of_ride = ways[letters.front().get_beg_of()][letters.front().get_end_of()] + (rand() % 36 - 5);
                                letters.front().set_t_end(std::max(cur_time, letters.front().get_t_begin()) + time_of_ride);
                                couriers[i].take_let(letters.front());
                                nobody_to_send = false;
                                std::cout << i << "  took the letter1" << std::endl;
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
                    std::cout << "take_all_letters" << std::endl;
                    if (couriers[i].is_free()) {
                        all_are_busy = true;
                        min_time = -1;
                        std::cout << "free_courier " << i << std::endl;
                        for (int j = 0; j < letters.size(); ++j) {
                            letter_office = letters.front().get_beg_of();
                            if (couriers[i].get_pos() == letter_office && (min_time == -1 || min_time + 5 < letters.front().get_order_time())) {
                                if (min_time == -1) min_time = std::max(cur_time, letters.front().get_order_time());
                                letters.front().set_t_begin(std::max(cur_time, letters.front().get_order_time()));
                                time_of_ride = ways[letters.front().get_beg_of()][letters.front().get_end_of()] + (rand() % 36 - 5);
                                letters.front().set_t_end(std::max(cur_time, letters.front().get_t_begin()) + time_of_ride);
                                couriers[i].take_let(letters.front());
                                nobody_to_send = false;
                                std::cout << i << "  took the letter1" << std::endl;
                                letters.pop();
                            }
                            else {
                                letters.push(letters.front());
                                letters.pop();
                            }
                        }
                        if (min_time != -1) equal_begin_time(couriers[i]);
                    }

                    //std::cout << "x" << std::endl;
                    if (couriers[i].is_free() && letters.size() > 0) {
                        for (int l = 0; l < letters.size(); ++l) {
                            //std::cout << "kkkk" << std::endl;
                            all_are_busy = true;
                            road_time = ways[couriers[i].get_pos()][letters.front().get_beg_of()];
                            delta = rand() % (36) - 5;
                            faster = false;
                            for (int k = 0; k < num_of_couriers; ++k) {
                                //std::cout << "kkkk" << std::endl;
                                if (i != k) {
                                    if (couriers[k].get_pos() == letters.front().get_beg_of() && couriers[k].is_free()) {
                                        faster = true;
                                        break;
                                    }
                                    sec_road_time = ways[couriers[k].get_pos()][letters.front().get_beg_of()];
                                    if (road_time > sec_road_time && couriers[k].is_free()) {
                                        faster = true;
                                        break;
                                    }
                                    if (couriers[k].num_of_letters() > 0) {
                                        if (couriers[k].last_let().get_end_of() == letters.front().get_beg_of() && couriers[k].last_let().get_t_end() < cur_time + road_time + delta) {
                                            faster = true;
                                            break;
                                        }
                                    }
                                }
                            }
                            if (!faster) {
                                nobody_to_send = false;
                                std::cout << i << "  took the letter2" << std::endl;
                                ++num_free_rides;
                                Letter l(couriers[i].get_pos(), letters.front().get_beg_of(), -1, -1, -1);
                                l.set_t_begin(cur_time);
                                l.set_t_end(cur_time + road_time + delta);
                                couriers[i].take_let(l);
                                target_of = letters.front().get_beg_of();
                                for (int i = 0; i < letters.size(); ++i) {
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


        std::cout << "extra_letters: " << std::endl;
        if (wait_for_take.size() > 0) {
            for (int i = 0; i < wait_for_take.size(); ++i) {
                std::cout << wait_for_take.front().get_order_time() << " " << wait_for_take.front().get_deadline() << " " << wait_for_take.front().get_beg_of() << " " << wait_for_take.front().get_end_of() << std::endl;
                wait_for_take.push(wait_for_take.front());
                wait_for_take.pop();
            }
        }
        std::cout << std::endl;
        std::cout << "letters: " << std::endl;
        if (letters.size() > 0) {
            for (int i = 0; i < letters.size(); ++i) {
                std::cout << letters.front().get_order_time() << " " << letters.front().get_deadline() << " " << letters.front().get_beg_of() << " " << letters.front().get_end_of() << std::endl;
                letters.push(letters.front());
                letters.pop();
            }
        }
        std::cout << std::endl;
        std::cout << "THE END OF THE " << day << " DAY" << std::endl;
        std::cout << std::endl;
        std::cout << std::endl;
    }
}