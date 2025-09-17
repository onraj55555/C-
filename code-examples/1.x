#import("std/allocator");

enum result<T, E> {
    ok(T),
    err(E)
}

struct foo {
    a: i32,
    b: i32
}

impl foo {
    fn new(a: i32, b: i32) -> foo {
        return foo {a:a, b:b};
    }
    
    fn init(self: *foo, a: i32, b: i32) -> void {
        self.a = a;
        self.b = b;
    }
    
    fn new_alloc(a: i32, b: i32, all: allocator) -> foo* {
        let foo = all.alloc(foo, 1);
        foo->a = a;
        foo->b = b;
        return foo;
    }
    
    fn add(self: *foo) -> i32 {
        return self.a + self.b;
    }
}

fn main() -> i32 {
    let a = allocator{};
    a.init();
    
    let f1 : foo = foo::new(1, 2);
    return f1->add();
}