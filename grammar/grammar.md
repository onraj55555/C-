# Types, includes all basic types and custom struct types
simple_type -> "i8" | "i16" | "i32" | "i64" | "isize" | "u8" | "u16" | "u32" | "u64" | "usize" | "bool" | "void"
base_type -> simple_type | id
complex_type -> base_type pointer_suffix
pointer_suffix -> "*" pointer_suffix | e

# Expressions
variable_declaration_expression -> "let" id (":" complex_type | e) "=" expression
function_call_expression -> id "(" function_call_expression_argument_list ")"
function_call_expression_argument_list -> expression function_call_expression_argument_list_tail | e
function_call_expression_argument_list_tail -> "," expression function_call_expression_argument_list_tail | e
expression -> assignment_expression
assignment_expression -> variable_declaration_expression | lvalue "=" assignment_expression | additive_expression
additive_expression -> term term_prime
term_prime -> (+ | -) term term_prime | e
term -> factor factor_prime
factor_prime -> (* | / | %) factor factor_prime
factor -> primary_expression
primary_expression -> literal | id | function_call_expression | "(" expression ")"
postfix_expression -> primary_expression post_fix_expression_tail
post_fix_expression_tail -> "(" function_call_expression_argument_list ")" post_fix_expression_tail | e

# Misc
lvalue -> id