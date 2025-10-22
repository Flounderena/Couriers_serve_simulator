#include "Courier.h"

Courier::Courier(int p) : free(true), place(p), backpack(), history() {}

void Courier::change_status(bool f) {
	free = f;
}

bool Courier::is_free() {
	return free;
}

void Courier::set_pos(int p) {
	place = p;
}

void Courier::take_let(Letter l) {
	backpack.push_back(l);
	free = false;
}

Letter Courier::first_let() {
	return backpack.front();
}

Letter Courier::last_let() {
	return backpack.back();
}

void Courier::deliver_let() {
	if (backpack.front().get_order_time() > 0) {
		history.push_back(backpack.front());
	}
	++rides_num;
	rides_time += backpack.front().get_t_end() - backpack.front().get_t_begin();
	backpack.pop_front();
	if (backpack.size() == 0) free = true;
}

int Courier::get_pos() {
	return place;
}

int Courier::get_num_of_let() {
	return backpack.size();
}

void Courier::move_to_end() {
	backpack.push_back(backpack.front());
	backpack.pop_front();
}

int Courier::get_mid_travels() {
	return rides_time / rides_num;
}

std::pair<int, int> Courier::get_coordinates(std::pair<int, int> from, std::pair<int, int> to, int cur_time) {
	std::pair<int, int> vec_coords = { to.first - from.first, to.second - from.second };
	int delta_t = cur_time - backpack.front().get_t_begin();
	int ride_t = backpack.front().get_t_end() - backpack.front().get_t_begin();
	double part_of = delta_t / ride_t;
	if (part_of >= 1) {
		vec_coords = to;
	}
	else {
		vec_coords.first *= part_of;
		vec_coords.second *= part_of;
	}
	return vec_coords;
}