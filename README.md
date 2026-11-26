# Tarea 5 - La cena de los filosofos

Solucion al problema de la cena de los filosofos (5 filosofos) con hilos POSIX
(`pthread`) y semaforos. Cada filosofo tiene un estado (pensando, hambriento,
comiendo) y solo come si ninguno de sus vecinos esta comiendo, lo que evita el
bloqueo mutuo. Cada filosofo come 3 veces.

- `filosofos.c`: codigo fuente
- `salida.txt`: salida de la ejecucion en Debian 13 (Trixie)

Compilar: `gcc -Wall -Wextra -pthread -o filosofos filosofos.c`

Ejecutar: `./filosofos > salida.txt`
