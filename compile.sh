FLAGS="-g -O0 -Wall -Wextra -fsanitize=address"
SRC="src/dlist_verifiers.c src/dlist.c src/dlist_dump.c src/dot_api.c"
gcc $FLAGS "$1" $SRC
