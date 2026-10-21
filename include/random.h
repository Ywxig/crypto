#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// Выбирает реализацию по платформе (Windows / Linux)
long long randInt(void);

// Реализация для Windows (определена только при сборке под Windows)
long long randIntWin(void);

#ifdef __cplusplus
}
#endif
