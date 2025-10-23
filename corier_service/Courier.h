#pragma once
#include "Letter.h"


class Courier
{
public:
    Courier(int p);
    ~Courier() {}

    void change_status(bool f);
    bool is_free();

    void set_pos(int p);
    void take_let(Letter l);

    Letter first_let();
    Letter last_let();

    void deliver_let();

    int get_pos();
    int get_num_of_let();

    void move_to_end();

    int get_mid_travels();

    std::pair<int, int> get_coordinates(std::pair<int, int> from, std::pair<int, int> to, int cur_time);


    int get_busy_time() const { return busy_time; } // патч зан€тость курьера (мин) Ч дл€ отчЄта

private:
    bool free;
    int place, rides_time, rides_num;
    int busy_time; // патч суммарное врем€ любых поездок (включа€ пустые)
    std::deque<Letter> backpack;
    std::vector<Letter> history;
};
