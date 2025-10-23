#include "Courier.h"

Courier::Courier(int p)
    : free_(true),
    place_(p),
    rides_time_(0),
    rides_num_(0),
    busy_time_(0),
    backpack_(),
    history_()
{
}

void Courier::change_status(bool f) { free_ = f; }
bool Courier::is_free() const { return free_; }

void Courier::set_pos(int p) { place_ = p; }
int  Courier::get_pos() const { return place_; }

void Courier::take_let(const Letter& l) {
    backpack_.push_back(l);
    free_ = false;
}

int Courier::get_num_of_let() const { return static_cast<int>(backpack_.size()); }

Letter& Courier::dummy() {
    static Letter d(-1, -1, 0, 0, -1);
    return d;
}

Letter Courier::first_let() {
    if (backpack_.empty()) return dummy();
    return backpack_.front();
}

Letter Courier::last_let() {
    if (backpack_.empty()) return dummy();
    return backpack_.back();
}

void Courier::deliver_let() {
    if (backpack_.empty()) return;

    if (backpack_.front().get_order_time() > 0) {
        history_.push_back(backpack_.front());
    }
    int dt = backpack_.front().get_t_end() - backpack_.front().get_t_begin();
    if (dt < 0) dt = 0;

    ++rides_num_;
    rides_time_ += dt;
    busy_time_ += dt;

    backpack_.pop_front();
    if (backpack_.empty()) free_ = true;
}

void Courier::move_to_end() {
    if (backpack_.size() <= 1) return;
    Letter x = backpack_.front();
    backpack_.pop_front();
    backpack_.push_back(x);
}

int Courier::get_mid_travels() const {
    if (rides_num_ == 0) return 0;
    return rides_time_ / rides_num_;
}

int Courier::get_busy_time() const {
    return busy_time_;
}

std::pair<int, int> Courier::get_coordinates(std::pair<int, int> from,
    std::pair<int, int> to,
    int cur_time)
{
    if (backpack_.empty()) return from;

    int delta_t = cur_time - backpack_.front().get_t_begin();
    int ride_t = backpack_.front().get_t_end() - backpack_.front().get_t_begin();
    if (ride_t <= 0) return from;

    double part_of = static_cast<double>(delta_t) / static_cast<double>(ride_t);
    if (part_of >= 1.0) return to;
    if (part_of <= 0.0) return from;

    std::pair<int, int> vec_coords = { to.first - from.first, to.second - from.second };
    vec_coords.first = static_cast<int>(from.first + vec_coords.first * part_of);
    vec_coords.second = static_cast<int>(from.second + vec_coords.second * part_of);
    return vec_coords;
}
