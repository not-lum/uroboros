gcc -g -O0 -Wall -Wextra -fsanitize=address "$1" src/dlist_verifiers.c src/dlist.c src/dlist_dump.c src/dot_api.c
