# Separate sections in file
```
types:
all type definitions

functions:
all functions
```

# Returning big types between the same language
```
let x = Foo::new(...); // Foo::new() -> Foo with Foo being bigger than a register
// Translates to
let x = Foo{}; // Empty initialised, perhaps every field is initialised to default;
x.new(...); // Initialises the fields
```

# Standard types
Core only specifies basic numeric types like ints, floats and maybe booleans
Std lib expands this by providing strings types which are structs of a u8 pointer and a length, ..., maybe this can be in the core

# Operators and interfaces
Operators can be overloaded if a certain function implementation is provided (like Rust with the Ord trait)
Interfaces can be made and structs should implement them if they want polymorphism to work (useful for allocators)

# Modules
Modules to prevent namespace pollution?
``` std/string.c!
mod string;

struct string {
  data: u8 *,
  len: u64
}

impl string {
  fn new() -> string {
    return string {data: 0, len: 0};
  }
  
  ...
}
```

# Argument passing
I DO NOT KNOW IF I WANT THIS BUT COULD BE FAST

Function arguments which are bigger than the word size get automatically passed as a pointer

```
struct big_struct {
    // ...
}

fn f(bs: big_struct) -> void {

}

let bs = big_struct{...};
f(bs); // becomes f(&bs) since bs is not a pointer
let bsp = &bs;
f(bsp); // Not allowed since this means bsp allows for modification but the function signature tells us that the original bs will not be modified
```
