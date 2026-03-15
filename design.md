# AUPL
Another Unnecessary Programming Language

# Language Specifications

## Project Structure
- every ```.aupl``` file is a class or similar language element.

## Syntax
### Class Header
A class **Class Header** consists of one of the following 2 options:

1. implicitly extend ```Object```:
```
class ClassName
```

2. explicitly extend another class:
```
class ClassName : ParentName
```

#### Abstract Classes
```
abstract class ClassName
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
int sum(a, b) = a + b
```

Functions may be abstract:
```
class Animal
void do_noise() = abstract
```

Functions of parent classes may be overridden. It does not matter wether the parent function was abstract or not. (syntax might change):
```
class Dog : Animal
override void do_noise() {
	print("bark!")
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

### Types
#### Basic Types
The following basic types are integrated into the language:
```
void
bool
int		(64 bit)
float	(64 bit)

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

# Modules
## Compiler
The compiler parses ```.aupl``` files and outputs **bytecode** or another desired form of result.

## VM
The Virtual Machine (VM) execudes **bytecode** that was previously put out by the compiler.