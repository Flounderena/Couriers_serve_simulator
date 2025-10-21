#include "Courier.h"

Courier::Courier(int c, int p) : free(true), capacity(c), place(p), backpack(), history() {}

void Courier::change_status(bool f) {
	free = f;
}

bool Courier::is_free() {
	return free;
}

void Courier::set_pos(int p) {
	place = p;
}

int Courier::num_of_letters() {
	return backpack.size();
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
	backpack.pop_front();
	if (backpack.size() == 0) free = true;
}

void Courier::fr_ride(int t) {
	free_rides += t;
}

void Courier::ride(int t) {
	rides += t;
}

int Courier::get_pos() {
	return place;
}

int Courier::get_free_space() {
	return capacity - backpack.size();
}

int Courier::get_num_of_let() {
	return backpack.size();
}

void Courier::move_to_end() {
	backpack.push_back(backpack.front());
	backpack.pop_front();
}