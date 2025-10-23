#pragma once

class Letter
{
public:
    Letter();
    Letter(int f, int s, int t, int l_t, int l_cnt);
    ~Letter() {}

    void set_t_begin(int t);
    void set_t_end(int t);
    void set_beg_of(int of);

    int get_t_begin();
    int get_t_end();
    int get_beg_of();
    int get_end_of();
    int get_order_time();
    int get_deadline();
    int get_letter_num();

private:
    int from_of, to_of, order_time, deadline, num_of_letter;
    bool delivered;
    int t_begin, t_end;
};
