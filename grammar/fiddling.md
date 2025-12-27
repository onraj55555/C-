type -> "i8" | "i16" | "i32" | "i64" | "u8" | "u16" | "u32", "u64" | "bool" | "isize" | "usize"
complex_type -> type | id // TODO: add pointers
function_signature_return_type -> type | "void" | id
function_signature_parameter -> id ":" complex_type
function_signature_parameter_list -> e | function_signature_parameter function_signature_parameter_list_tail
function_signature_parameter_list_tail -> e | "," function_signature_parameter function_signature_parameter_list_tail
function_signature -> "fn" id "(" ... ")" "->" function_signature_return_type
//function_definition -> function_signature ";"
function_declaration -> function_signature "{" body "}"
variable_initialisation -> "let" id ":" complex_type "=" expression
variable_assignment -> id "=" expression
function_call -> id "(" function_call_parameter_list ")"
function_call_parameter_list -> e | expression function_call_parameter_list_tail
function_call_parameter_list_tail -> e | "," expression function_call_parameter_list_tail
