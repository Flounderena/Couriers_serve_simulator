#pragma once
class Letter
{
public:
	
	Letter(int f, int s, int t, int l_t) : from_of(f), to_of(s), order_time(t), deadline(l_t), delivered(false), t_begin(0), t_end(0) {}
	~Letter() {}

	void set_t_begin(int t) {
		t_begin = t;
	}

	void set_t_end(int t) {
		t_end = t;
	}

	int get_t_begin() {
		return t_begin;
	}

	int get_t_end() {
		return t_end;
	}

	int get_beg_of() {
		return from_of;
	}

	int get_end_of() {
		return to_of;
	}

	int get_order_time() {
		return order_time;
	}

	int get_deadline() {
		return deadline;
	}

	bool is_delievered() {
		return delivered;
	}

	bool get_delivered() {
		delivered = true;
	}

private:
	int from_of, to_of, order_time, deadline;
	bool delivered;
	int t_begin, t_end;
};

