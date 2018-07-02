# FastTop
Для того чтобы приложение собиралось, нужно чтобы на компьютере был установлен conan.

ТОП всегда выдается только для текущей недели. (из условия)
Если для конкретного user_id попытаться запросить ТОП чаще 1 раза в минуту, то запрос будет проигнорирован. (из условия)
Во время текущей (последней) недели user_deal_won(id,time,amount) time_current_week_begin < time <= time(NULL) (НО! также time <= time_current_week_end) (предположил)

Тесты проводились на базе в 30 млн, количество прогонов каждой функции 10 000 000 , на ноутбуке core i5 (Ubuntu 16.04). Результаты:

user_connected() одновременно с user_disconnected(): максимально время: 73 мкс, среднее время: 2 мкс

user_registered():  максимально время:  114 мкс, среднее время: 3 мкс

user_renamed():  максимально время:  50 мкс, среднее время: 1 мкс

user_deal_won():  максимально время:  105 мкс, среднее время: 9 мкс

get_user_top():  максимально время:  733 мкс, среднее время: 24 мкс

Последний unit тест не подвисает, просто нужно подождать 60 сек, тк идет проверка на то что через 60 сек можно повторить запрос ТОПа для указанного user_id

[==========] Running 6 tests from 6 test cases.

[----------] Global test environment set-up.

[----------] 1 test from user_registered

[ RUN      ] user_registered.add_user

[       OK ] user_registered.add_user (1 ms)

[----------] 1 test from user_registered (1 ms total)

[----------] 1 test from user_renamed

[ RUN      ] user_renamed.rename_user

[       OK ] user_renamed.rename_user (0 ms)

[----------] 1 test from user_renamed (0 ms total)

[----------] 1 test from user_connected

[ RUN      ] user_connected.connect_user

[       OK ] user_connected.connect_user (0 ms)

[----------] 1 test from user_connected (0 ms total)

[----------] 1 test from user_disconnected

[ RUN      ] user_disconnected.disconnect_user

[       OK ] user_disconnected.disconnect_user (0 ms)

[----------] 1 test from user_disconnected (0 ms total)

[----------] 1 test from user_deal_won

[ RUN      ] user_deal_won.deal_won_user

[       OK ] user_deal_won.deal_won_user (0 ms) 

[----------] 1 test from user_deal_won (1 ms total)

[----------] 1 test from get_user_top

[ RUN      ] get_user_top.get_top_user

[       OK ] get_user_top.get_top_user (60000 ms)

[----------] 1 test from get_user_top (60000 ms total)

[----------] Global test environment tear-down

[==========] 6 tests from 6 test cases ran. (60002 ms total)

[  PASSED  ] 6 tests.
