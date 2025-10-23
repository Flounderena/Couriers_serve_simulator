#include "Letter.h"

Letter::Letter()
    : from_of(-1), to_of(-1), order_time(-1), deadline(-1),
    delivered(false), t_begin(0), t_end(0), num_of_letter(-1) {
}

Letter::Letter(int f, int s, int t, int l_t, int l_cnt)
    : from_of(f), to_of(s), order_time(t), deadline(l_t),
    delivered(false), t_begin(0), t_end(0), num_of_letter(l_cnt) {
}

void Letter::set_t_begin(int t) { t_begin = t; }
void Letter::set_t_end(int t) { t_end = t; }
void Letter::set_beg_of(int of) { from_of = of; }

int Letter::get_t_begin() { return t_begin; }
int Letter::get_t_end() { return t_end; }
int Letter::get_beg_of() { return from_of; }
int Letter::get_end_of() { return to_of; }
int Letter::get_order_time() { return order_time; }
int Letter::get_deadline() { return deadline; }
int Letter::get_letter_num() { return num_of_letter; }
