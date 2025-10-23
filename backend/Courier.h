#pragma once
#include <vector>
#include <queue>
#include <deque>
#include <utility>
#include "Letter.h"

class Courier
{
public:
    explicit Courier(int p);
    ~Courier() {}

    void  change_status(bool f);
    bool  is_free() const;

    void  set_pos(int p);
    int   get_pos() const;

    void  take_let(const Letter& l);
    int   get_num_of_let() const;

    Letter first_let();
    Letter last_let();

    void  deliver_let();
    void  move_to_end();

    int   get_mid_travels() const;
    int   get_busy_time() const;

    std::pair<int, int> get_coordinates(std::pair<int, int> from,
        std::pair<int, int> to,
        int cur_time);

private:
    bool free_;
    int  place_;
    int  rides_time_;
    int  rides_num_;
    int  busy_time_;

    std::deque<Letter> backpack_;
    std::vector<Letter> history_;

    static Letter& dummy();
};
