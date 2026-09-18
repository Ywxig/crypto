# --- Этап 1: Сборка в Alpine ---
FROM alpine:3.20 AS builder

WORKDIR /app

# Устанавливаем alpine-sdk (g++, make и т.д.)
RUN apk add --no-cache alpine-sdk

# Копируем исходный код проекта
COPY . .

# Собираем проект с помощью нашего Makefile
RUN make

# --- Этап 2: Финальный легковесный образ Alpine ---
FROM alpine:3.20

WORKDIR /app

# Устанавливаем стандартные библиотеки C++
RUN apk add --no-cache libstdc++

# Копируем скомпилированный бинарник из первого этапа
COPY --from=builder /app/dist/main /app/cripto

# Запускаем приложение (интерактивный режим)
ENTRYPOINT ["./cripto"]
