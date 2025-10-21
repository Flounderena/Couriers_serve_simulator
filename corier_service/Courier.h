#pragma once
#include <vector>
#include <queue>
#include <deque>
#include "Letter.h"


class Courier
{
public:

	Courier(int c, int p);
	~Courier() {}

	void change_status(bool f);

	bool is_free();

	void set_pos(int p);

	int num_of_letters();

	void take_let(Letter l);

	Letter first_let();

	Letter last_let();

	void deliver_let();

	void fr_ride(int t);

	void ride(int t);

	int get_pos();

	int get_free_space();

	int get_num_of_let();

	void move_to_end();

private:
	bool free;
	int capacity, place, free_rides, rides;
	std::deque<Letter> backpack;
	std::vector<Letter> history;
};

