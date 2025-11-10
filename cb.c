#include <stdio.h>
#define CB_IMPLEMENTATION
#include "cb.h"

void compile_code(char * main_file, char * output) {
    command_t * command = command_init(CC);
    command_append(command, "-Iinclude");
    command_append(command, main_file);
    command_append(command, "src/allocator.c");
    command_append(command, "src/compilation_unit.c");
    command_append(command, "src/lexer.c");
    command_append(command, "src/string_builder.c");
    command_append(command, "src/string_slice.c");
    command_append(command, "src/util.c");
    command_set_output_file(command, output);
    command_execute(command);
}

void run_code() {
    char * output = "c-";
    printf("Compiling code...\n");
    compile_code("src/main.c", output);
    printf("Code compiled!\n");
    command_t * command = command_init("./c-");
    command_append(command, "code-examples/1.x");
    printf("Executing ./c-...\n");
    command_execute(command);
    printf("Code ran\n");
}

void test_code() {

}

int main(int argc, char ** argv) {
    cb_rebuild_on_change(__FILE__, argv);
    parse_arguments(argc, argv);
    if(has_argument_at_intex("run", 1)) run_code();
    else if(has_argument_at_intex("test", 1)) test_code();
    return 0;
}
