# Тренажёр 81-717 на основе ESP32 и Metrostroi Subway Simulator

## Программная часть

Зависимости: gmcl_gwsockets_winXX.dll

Локальные изменения в библиотеке gilmaimon/ArduinoWebsockets, чтобы header 'upgrade' сравнивался регистронезависимо (websockets_server.cpp, строка 67)
if(params.headers["Connection"].find("Upgrade") == std::string::npos && params.headers["Connection"].find("upgrade") == std::string::npos) return {};

## Аппаратная часть

### Распиновка

| Пин ESP32 | Назначение | Требования |
|-----------|------------|------------|
| 18 | Подсвет пульта (1 блок) | - |
| -- | Подсвет пульта (3 блок) | - |
| 14 | Подсвет В-В приборов | - |
| 5 | Манометр ТМ | ШИМ |
| 17 | Манометр НМ | ШИМ |
| 16 | Манометр ТЦ | ШИМ |
| 21 | Вольтметр АКБ | ШИМ |
| 34 | Потенциометр (1 блок) | АЦП |
| 35 | Потенциометр (3 блок) | АЦП |
| 32 | Потенциометр (КМ) | АЦП |
| 23 | Амперметр ТЭД (+) | ШИМ |
| 15 | Амперметр ТЭД (-) | ШИМ |
| 22 | Киловольтметр | ШИМ |
| 27 | 74HC165N QH (data) | - |
| 13 | 74HC165N SH (latch) | - |
| 4 | 74HC165N CLK (clock) | - |
| 19 | 74HC595N DS (data) | - |
| 25 | 74HC595N ST_CP (latch) | - |
| 26 | 74HC595N SH_CP (clock) | - |
| 12 | 74HC595N !OE | - |

### Подключение КВ

В = Верхняя рейка
Н = Нижняя рейка

Главный вал:
Кулачки: В-3, Н-1, Н-3, земля
Перемычки: (В-1, Н-4)

Реверсивный вал:
Кулачки: Н-2, Н-3, земля
Перемычки: нет