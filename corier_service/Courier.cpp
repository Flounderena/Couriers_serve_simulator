#include "Courier.h"
#include <algorithm>
#include <cmath>

// патч инициализация счётчиков
Courier::Courier(int p) : free(true), place(p), backpack(), history() {
    rides_time = 0;
    rides_num = 0;
    busy_time = 0; // суммарное время в движении
}

void Courier::change_status(bool f) { free = f; }
bool Courier::is_free() { return free; }
void Courier::set_pos(int p) { place = p; }

void Courier::take_let(Letter l) {
    backpack.push_back(l);
    free = false;
}

Letter Courier::first_let() { return backpack.front(); }
Letter Courier::last_let() { return backpack.back(); }

void Courier::deliver_let() {
    if (backpack.front().get_order_time() > 0) {
        history.push_back(backpack.front());
    }
    ++rides_num;

    // патч безопасное накопление времени
    int dt = std::max(0, backpack.front().get_t_end() - backpack.front().get_t_begin());
    rides_time += dt;
    busy_time += dt; // учитываем занятость

    backpack.pop_front();
    if (backpack.size() == 0) free = true;
}

int Courier::get_pos() { return place; }
int Courier::get_num_of_let() { return (int)backpack.size(); }

void Courier::move_to_end() {
    backpack.push_back(backpack.front());
    backpack.pop_front();
}

// патч защита от, пусть даже невозможного, деления на ноль
int Courier::get_mid_travels() {
    return (rides_num > 0 ? (rides_time / rides_num) : 0);
}

// патч корректная интерполяция, возвращаем абсолютные координаты
std::pair<int, int> Courier::get_coordinates(std::pair<int, int> from, std::pair<int, int> to, int cur_time) {
    if (backpack.empty()) return from;
    int delta_t = cur_time - backpack.front().get_t_begin();
    int ride_t = std::max(1, backpack.front().get_t_end() - backpack.front().get_t_begin());
    double part_of = std::clamp((double)delta_t / (double)ride_t, 0.0, 1.0);
    int x = from.first + (int)std::lround((to.first - from.first) * part_of);
    int y = from.second + (int)std::lround((to.second - from.second) * part_of);
    return { x, y };
}
