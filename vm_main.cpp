#include <iostream>
#include <string>
#include <vector>
#include <assert.h>
#include <iomanip>
#include <fstream>

#include "vm.hpp"
#include "instructions.hpp"
#include "class_db.hpp"
#include "execute.hpp"
#include "class_registrator.hpp"

void create_vm(const vm::ClassDB& db, const std::string& path) {
	vm::VirtualMachine vm(db);

	{
		std::ifstream input_file(path, std::ios::binary);

		size_t code_size;
		size_t const_memory_size;
		size_t main_start;

		input_file.read(reinterpret_cast<char*>(&code_size), sizeof(code_size));
		input_file.read(reinterpret_cast<char*>(&const_memory_size), sizeof(const_memory_size));
		input_file.read(reinterpret_cast<char*>(&main_start), sizeof(main_start));

		vm.code = new vm::Instruction[code_size];
		input_file.read(reinterpret_cast<char*>(vm.code), sizeof(vm::Instruction) * code_size);

		vm.const_memory = new vm::Value[const_memory_size];
		input_file.read(reinterpret_cast<char*>(vm.const_memory), sizeof(vm::Value) * const_memory_size);

		vm.main_start = main_start;

		input_file.close();
	}

    vm::run_vm(vm);
}

int main(int argc, char** argv) {
	if (argc != 2) {
		std::cerr << "Only 1 arg for input file expected!" << std::endl;
		return 1;
	}

    vm::ClassDB db;
	vm::register_classes(db);
	
	create_vm(db, argv[argc - 1]);

	return 0;
}