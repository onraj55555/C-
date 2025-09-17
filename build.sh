gcc -Iinclude \
    "$1" \
    src/compilation_unit.c \
    src/util.c \
    src/allocator.c \
    src/lexer.c \
    src/string_builder.c \
    src/string_slice.c \
    -o "$2"