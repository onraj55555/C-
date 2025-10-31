#include <stdio.h>
#define CB_IMPLEMENTATION
#include "cb.h"

void compile_code(char * main_file, char * output) {
    command_t * command = command_init("gcc");
    command_append(command, "-Iinclude");
    command_append_n(command,
                     main_file,
                     "src/compilation_unit.c",
                     "src/util.c",
                     "src/allocator.c",
                     "src/lexer.c",
                     "src/string_builder.c",
                     "src/string_slice.c"
                     );
    command_append_n(command, "-o", output);
    command_execute(command);
}

void run_code() {
    char * output = "c-";
    compile_code("src/main.c", output);
    command_t * command = command_init("./c-");
    command_append(command, "code-examples/1.x");
    command_execute(command);
    printf("Command ran\n");
}

void test_code() {

}

int main(int argc, char ** argv) {
    parse_arguments(argc, argv);
    if(has_argument_at_intex("run", 1)) run_code();
    else if(has_argument_at_intex("test", 1)) test_code();
    return 0;
}
