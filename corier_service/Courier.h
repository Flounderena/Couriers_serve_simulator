#pragma once
#include <vector>
#include <queue>
#include <deque>
#include "Letter.h"


class Courier
{
public:

	Courier(int c, int p) : free(true), capacity(c), place(p), backpack(), history() {}
	~Courier() {}

	void change_status(bool f) {
		free = f;
	}

	bool is_free() {
		return free;
	}

	void set_pos(int p) {
		place = p;
	}

	int num_of_letters() {
		return backpack.size();
	}

	void take_let(Letter l) {
		backpack.push_back(l);
		free = false;
	}

	Letter first_let() {
		return backpack.front();
	}

	Letter last_let() {
		return backpack.back();
	}

	void deliver_let() {
		if (backpack.front().get_order_time() > 0) {
			history.push_back(backpack.front());
		}
		backpack.pop_front();
		if (backpack.size() == 0) free = true;
	}

	void fr_ride(int t) {
		free_rides += t;
	}

	void ride(int t) {
		rides += t;
	}

	int get_pos() {
		return place;
	}

	int get_free_space() {
		return capacity - backpack.size();
	}

	int get_num_of_let() {
		return backpack.size();
	}

	void move_to_end() {
		backpack.push_back(backpack.front());
		backpack.pop_front();
	}

private:
	bool free;
	int capacity, place, free_rides, rides;
	std::deque<Letter> backpack;
	std::vector<Letter> history;
};

