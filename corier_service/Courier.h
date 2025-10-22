#pragma once
#include <vector>
#include <queue>
#include <deque>
#include "Letter.h"


class Courier
{
public:

	Courier(int p); // конструктор по 1 параметру: филлиал, в котором оказывается курьер в началае 1 дня
	~Courier() {}

	void change_status(bool f); // меняет статус курьера на f

	bool is_free(); // возвращает, свободен ли курьер

	void set_pos(int p); // устанавливает, в каком филлиале курьер 

	void take_let(Letter l); // закидывает письмо в инвентарь

	Letter first_let(); // возвращает 1 письмо в инвентаре

	Letter last_let(); // возвращает последнее письмо в инвентаре

	void deliver_let(); // убирает письмо из инвентаря в историю доставленных писем

	int get_pos(); // возвращает, в каком филлиале сейчас курьер

	int get_num_of_let();  // возвращает количество писем в инвентаре

	void move_to_end(); // перемещает письмо в инвентаре из начала в конец очереди

	int get_mid_travels(); // возвращает среднее время поездок

	std::pair<int, int> get_coordinates(std::pair<int, int> from, std::pair<int, int> to, int cur_time); // возвращает координаты курьера


private:
	bool free;
	int place, rides_time, rides_num;
	std::deque<Letter> backpack;
	std::vector<Letter> history;
};

