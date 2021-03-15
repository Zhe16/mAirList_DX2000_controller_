# mAirList Behringer DX2000 controller (inDev)

Arduino-base controller for mAirList and Behringer DX2000USB mixer


Disclamer
--
This project is our own development. You may see a lot of irrational decisions. I would be grateful if you could suggest your edits.
***The project is under development and, perhaps, some functions are not working.*** It takes time to fully work.
I reserve the copyright for this development. I give permission to freely use this project for personal purposes with attribution. But ***if you want to use the development for commercial purposes - contact me for details.***

Main functions
--
* Carwall buttons 1-9 and Page navigation buttons (Page Up/Page Down)
* Fader start
	* **Note:** _Mixer modification required! The warranty will be lost!_
	* Deactivation of the function is provided 
* Microphones faders driving monitor out muting
* Track-start buttons
* LED-lamp for "MIC ON", "EOF WARNING", "ON AIR", "SILENCE DETECTED" indicator (on WS2811 Chip recomended)

Communication with mAirList via COM port or via MIDI _(MIDI in developing)_  


Hardware requirements 
--
* Arduino Mega 2560
* 2-Relay module (For monitor muting function) 
* 7 x Micro Limit Switch (For fader-start function)
* Jack plug (5x for Track-start buttons, 2x for Monitor line out)
* 3,5 Jack socket (for connection monitors)
* RGB-lamp with 1-wire interface ()
* Behringer DX2000USB

Development process
--
**March, 15 2021:** Alpha 0.2
* A section on github has been created
* Prototype readiness by 60%
* Pre-firmware version 0.2a 

To-do list
--
**March, 15 2021:** Alpha 0.2
* End of prototype assembly
* Adaptation of the firmware for the prototype
* Creating a script for mAirList for feedback 


# mAirList Behringer DX2000 контроллер (inDev)

Контроллер для системы автоматизации радиовещания mAirList и микшерного пульта Behringer DX2000USB на базе Arduino MEGA


Примечание
--
Этот проект - собственная разработка. Вы можете увидеть множество иррациональных решений. Буду признателен, если вы предложите свои правки. 
***Проект на стадии разработки и, возможно, часть функций не работает совсем или работает некорректно.*** 
Права на данную разработку оставляю за собой, вы можете использовать её свободно в личных целях с указанием авторства. ***Если вы хотите использовать её в коммерческих целях - свяжитесь со мной.***

Основные функции
--
* Джингл-машина (CartWall) на 9 плееров + 2 навигационные кнопки (След/Пред страница)
* Фейдер-старт
	* **Примечание:** _Требуется доработка микшера! Возможна потеря гарантии!_
	* Возможна деактивация функции
* Отключение мониторной линии (динамиков прослушивания) при включенных микрофонах
* Сопряжение кнопок Track start микшера
* Светодиодная индикация состояний:
	* **"MIC ON"** - Микрофон включён
	* **"EOF WARNING"** - Файл подходит к концу
	* **"ON AIR"** - В эфире (Энкодер активен)
	* **"SILENCE"** - Тишина в эфире

Подключение к mAirList через COM порт (Arduino) или через протокол MIDI _(MIDI в разработке)_  


Требования к оборудованию 
--
* Arduino Mega 2560
* Модуль на 2 реле (Для отключения мониторов) 
* 7 x микропереключателей с лапкой (концевики для фейдеров)
* Jack-штекеры (5x для Track-start кнопок, 2x для выхода на мониторы)
* 3,5 Jack разъём (для подключения мониторов)
* RGB панель для индикации с 1-проводным интерфейсом
* Behringer DX2000USB

Разработка
--
**15 мая 2021:** Альфа 0.2
* Создана ветка на GitHub
* Прототип готов на 60%
* Предварительная версия прошивки 0.2a 

To-do list
--
**15 мая 2021:** Альфа 0.2
* Окончание сборки прототипа
* Адаптация прошивки под прототип
* Создание скрипта mAirList для обратной связи с пультом 
