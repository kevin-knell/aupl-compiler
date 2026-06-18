# Compiler
## Parsing
- add RegEx expressions
- add macros
- add annotations

## Symbols
- make all `Expression` & `Statement` classes only creatable as `shared_ptr` (see other symbol classes)
- (?) make all symbols derive from one base class and make a uniform AST out of them

## Code Generation
- ### Bytecode
- ### C++
- ### JavaScript
	- use Node Composition to create websites like react
- ### (?) GLSL
	- make it possible to create .AUSL files that are used to create shaders

# VM
## Instruction Set

## Execution
- (?) direct-threaded loops

## Native Code Binding
- write script that parses library .hpp files and generates .hpp files that can be used to bind them
- One Script to rule them all, One Script to find them,
	One Script to bring them all, and in the darkness bind them

# AUPLib
- make lib completely separate from VM

## Algorithms & Datastructures
- lists
- maps
- graphs
- trees
- search
- sort

## Math
- Vector & Matrix classes
- quaternion

## OS
- ### Filesystem
	- add `Directory`
	- creating temporary files
- ### Multithreading

## Regex
- add RegEx
- do not use C++ std lib RegEx, because it cannot use named groups