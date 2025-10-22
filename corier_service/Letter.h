#pragma once
class Letter
{
public:
	
	Letter(int f, int s, int t, int l_t, int l_cnt); // конструктор по 5 параметрам: филлиал заказа, целевой филлиал, время заказа, дедлайн, порядковый номер
	~Letter() {}

	void set_t_begin(int t); // устанавливает время взятия письма курьером

	void set_t_end(int t); // устанавливает время доставки письма

	void set_beg_of(int of); // устанавливает филлиал, из которого данное письмо везётся

	int get_t_begin(); // возвращает время взятия письма курьером

	int get_t_end(); // возвращает время доставки письма

	int get_beg_of(); // возвращает филлиал, в котором его заказали

	int get_end_of(); // возвращает филлиал, в который надо отвезти

	int get_order_time(); // возвращает время заказа

	int get_deadline(); // возвращает дэдлайн заказа

	int get_letter_num(); // возвращает порядковый номер письма (который по счёту из всех)

private:
	int from_of, to_of, order_time, deadline, num_of_letter;
	bool delivered;
	int t_begin, t_end;
};

