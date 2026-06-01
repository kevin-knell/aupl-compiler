# AUPL
Another Unnecessary Programming Language

# Language Specifications

## Project Structure
- every ```.aupl``` file is a class or similar language element.

## Syntax
### File Header
#### Class Files
the file header of a class consists of one of the following 2 options:

1. implicitly extend ```Object```:
```
class ClassName
```

2. explicitly extend another class:
```
class ClassName : ParentName
```

#### Abstract Classes
Abstract classes inherit in the same way, but cannot be created
```
abstract class ClassName
```
```
abstract class ClassName : Base
```

### Class Elements
#### Member Variables
```
Type name
Type name = expression
```

#### Functions
A function is declared similar to **Java** or **C/C++**:
```
Type name(Type arg, Type arg...) {
	...
}
```

A function may directly return a value:
```
static int sum(a, b) = a + b
```

Functions may be abstract:
```
class Animal
void do_noise() = abstract
```

Functions of parent classes may be overridden. It does not matter wether the parent function was abstract or not. (syntax might change):
```
class Dog : Animal
override do_noise() {
	print("bark!")
}
```

#### Inner Classes
```
class MyInnerClass {
	...
}
```

#### Singletons
Singletons are classes that are automatically created as a static member.
```
singleton MySingleton {
	...
}
```
...is similar to:
```
class MySingletonClass { ... }
static MySingletonClass instance
```

singleton values can be accessed by using the name of the singleton:
```
singleton MySingleton {
	int value = 5
}

void do_smth() {
	print(MySingleton.value)
}
```

Singletons can inherit from classes:
```
class Base {}
singleton Derived : Base {}
```

#### Singletons instead of Static Members
Instead of:
```
static int x
static int y
static int z
```
...Users are encouraged to use:
```
private class BaseData {
	int x
	int y
	int z
}
singleton Data : BaseData {}
```

#### Implementations
```
class MyObject

int value

impl Comparable {
	int main(MyObject obj) = compare(obj)

	int compare(MyObject obj) {
		return obj.value - owner.value
	}
}
```
calling:
```
MyObject a
MyObject b
a.Comparable(b)
a.Comparable.main(b)
a.Comparable.compare(b)
```
using as instance:
```
MyObject a
MyObject b
Comparable c = a.Comparable
c(b)
c.main(b)
c.compare(b)
```
this Program compares 2 classes:
```
generic<T>
int compare(Comparable<T> a, Comparable<T> b) {
	return a(b)
}

static void main() {
	Dog dog_1(5)
	Dog dog_2(6)

	Cat cat_1(1)
	Cat cat_2(2)

	print(compare(dog_1.Comparable, dog_2.Comparable))
	print(compare(cat_1.Comparable, cat_2.Comparable))
}
```

### Statements
#### Variable Declaration & Assignment
Variables are declared and reassigned like in **Java** or **C/C++**:
```
int x
int y = 5
y = 3
```

Object Instances can be declared like in **C/C++**:
```
MyClass foo
MyClass bar = MyClass(...)
MyClass baz(...)
```

#### If / Else
```
if my_condition {
	...
}
```

```
if my_condition {
	...
} else {
	...
}
```

```
if my_condition {
	...
} else if second_condition {
	...
} else {
	...
}
```

#### While
```
while my_condition {
	...
}
```
```
while my_condition {
	...
	if second_condition {
		continue
	}
	...
	if third_condition {
		break
	}
	...
}
```

#### For
A for loop may iterate through integers:
```
for i in 5 {
	print(i)
}
```

> output:
> ```
> 0
> 1
> 2
> 3
> 4
> ```

A for loop may iterate through an array:
```
String[] messages = {"foo", "bar", "baz"}
for s in messages {
	print(s)
}
```

> output:
> ```
> foo
> bar
> baz
> ```

A for loop may use ```continue``` & ```break```:
```
for i in 5 {
	...
	if second_condition {
		continue
	}
	...
	if third_condition {
		break
	}
	...
}
```

#### Return
In a ```void``` function, ```return``` may be used without an expression to preemptively exit a function.
```
void my_function() {
	...
	if my_condition {
		return
	}
	...
}
```
A non-```void``` function must return a value.
```
int my_function() {
	return 5
}
```
A short function can use ```=``` instead of ```{...}``` & ```return```.
```
int my_function() = 5
```

### Expressions

#### Literals
```
int x = 5
float y = 5.0
bool b = true
String text = "Hello World"
Regex pattern = /\w+/
```

#### Variables
```
int x = 5
int y = x
```

#### Function Calls
```
my_func(5, 3, 7)
```

Maybe there will be the option to omit parenthesis for calls with exactly 1 argument
```
do_something()

print "without parens"
print("with parens")

func_with_3_args(x, y, z)
```
They can only be omitted as a single statement or when surrounded by parenthesis / other separators:
```
do_something(3, my_func(5), 8)
do_something(3, my_func 5, 8)
```

#### Unary Operations
```
-
not
```

#### Binary Operations
```
+ - * / %
and or
== != < > <= >=
```
Elvis Operator
```
MyObject obj = get_obj() ?: fallback
```

#### Ternary Operations
```
bool result = condition ? on_true : on_false
```

#### Tuples & Initializer Lists
```
(int, int) my_tuple = (2, 2)
int[4] array = { 5, 6, 2, 3 }
```
right not there is no destinction between tuples and initializer lists except for syntax
```
(int, float) tuple_from_init_list = { 5, 6.0 }
int[4] array_from_tuple = ( 5, 6, 2, 3 )
```

### Types
#### Basic Types
The following basic types are integrated into the language:
```
void
bool
int (64 bit)
float (64 bit)

These specific Native Types:
String
Shared
List

specific size types:
	i8		i16		i32		i64		| signed integers
	u8		u16		u32		u64		| unsigned integers
	f32		f64						| floating point
```
```String```, ```Shared```,... are not just registered into the ```ClassDB``` like ```vec2``` or ```Console``` are; They are required by the compiler for **instancing String Literals** and for **managing Object instances**.

#### Arrays / Lists
In **AUPL**, an ```Array``` is presented as a specific case of a ```List```.
While an ```Array``` has a fixed size, a an ```List``` may resize.
The user only has to care wether the size should be ```const```.
An ```Array``` / ```List``` may be declared in the following ways:
```
int[] list1
int[5] list2				// initial capacity = 5
int[my_value] list3			// initial capacity = my_variable

int[const] array1 = {1, 2, 3}		// size inferred
int[const 5] array2
int[const my_value]					// dynamically allocated
```

#### Tuples
Tuples may contain any Types of Objects:
```
(int, int)					// same as int[const 2]
(int, float, String)
(int, (int, int), int)		// nested
```

#### Generics
```
class MyList<T>
```

```
generic<T>
T my_function(T a, T b) = a + b
```

#### Type Expressions
when using ```auto``` in a variable declaration, the type is inferred.
```
auto a = 5
```

```typeof(...)``` can be used to infer the type of an expression. The result must be unambiguous at compiler time.
```
typeof(5) a = 5
```

```typename``` can be used to declare a type as if it was a variable.
```
typename T = int
T a = 5
```
it can also use ```typeof```.
```
typename T = typeof(5)
T a = 5
```

# Modules
## AUPL

## VM
The Virtual Machine (VM) execudes **bytecode** that was previously put out by the compiler.

### Instruction Set

## Compiler
The compiler parses ```.aupl``` files and outputs **bytecode** or another desired form of result.
