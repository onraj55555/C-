type -> "i8" | "i16" | "i32" | "i64" | "u8" | "u16" | "u32", "u64" | "bool" | "isize" | "usize"
complex_type -> type | id // TODO: add pointers
function_signature_return_type -> type | "void" | id
function_signature_parameter -> id ":" complex_type
function_signature_parameter_list -> e | function_signature_parameter function_signature_parameter_list_tail
function_signature_parameter_list_tail -> e "," function_signature_parameter function_signature_parameter_list_tail
function_signature -> "fn" id "(" ... ")" "->" function_signature_return_type
function_definition -> function_signature ";"
function_declaration -> function_signature "{" body "}"
