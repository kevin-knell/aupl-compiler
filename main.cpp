#include <iostream>
#include <string>
#include <vector>
#include <assert.h>
#include <chrono>

#ifdef COMPILER
#include "file_manager.hpp"
#include "tokenizer.hpp"
#include "symbol_builder.hpp"
#include "symbol_table.hpp"
#include "class_parser.hpp"
#include "statement.hpp"
#include "class.hpp"
#include "name_analyzer.hpp"
#include "expression.hpp"
#include "register_format_converter.hpp"
#include "const_folding_optimizer.hpp"
#include "erase_unused_variable_optimizer.hpp"
#include "color.hpp"
#include "bytecode_generator.hpp"
#endif

#include "vm.hpp"
#include "instructions.hpp"
#include "class_db.hpp"
#include "execute.hpp"
#include "class_registrator.hpp"
#include <iomanip>


void print_help() {
	std::cout <<
		"Usage: aupl [options] folder..." << std::endl <<
		"-h, --help             Display this message" << std::endl <<
		"-v, --version          Display version" << std::endl;
}


#ifdef COMPILER
int main(int argc, char** argv) {
	// print help
	for (int i = 1; i < argc; ++i) {
		std::string arg = argv[i];

		if (arg == "--help" || arg == "-h") {
			print_help();
			return 0;
		}
	}

	// print version
	for (int i = 1; i < argc; ++i) {
		std::string arg = argv[i];

		if (arg == "--version" || arg == "-v") {
			std::cout << "Version: 0.1" << std::endl;
			return 0;
		}
	}

	// compile

    // create class db
    vm::ClassDB db;
	vm::register_classes(db);

	cmp::SymbolTable symbol_table(db);

    // parse user source code
    std::string folder_path = argc == 1 ? "." : argv[argc - 1]; // Replace with your folder path
    auto files = cmp::get_all_files_in_folder(folder_path);

	if (files.empty()) {
		std::cerr << "no files in folder!" << std::endl;
		return 1;
	}

    for (const auto& file : files) {
        std::cout << "Tokenizing file: " << file << std::endl;
        std::string content = cmp::get_text_from_file(file);
		
		cmp::parse_file(content, symbol_table);
    }

    std::cout << "\nsemantic analysis" << std::endl;

    cmp::NameAnalyzer name_analyzer(symbol_table);
    name_analyzer.resolve_variables();

    std::cout << "\nconverting to short circuit if" << std::endl;

    // TODO

    std::cout << "\nconverting to register format" << std::endl;

    cmp::RegisterFormatConverter::convert_to_register_format(symbol_table);

    std::cout << "\noptimizing code" << std::endl;

    std::vector<cmp::Optimizer*> optimizers;
    optimizers.push_back((cmp::Optimizer*)new cmp::ConstFoldingOptimizer());
    optimizers.push_back((cmp::Optimizer*)new cmp::EraseUnusedVariableOptimizer());

    for (int i = 0; i < 1; ++i) {
        for (auto p : optimizers) {
            p->optimize(symbol_table);
        }
    }

    std::cout << "\ngenerating scope structures" << std::endl;
	symbol_table.generate_scope_structures();

    std::cout << "\ngenerating bytecode layout" << std::endl;
    size_t bytecode_size = cmp::generate_bytecode_layout(symbol_table);

    std::cout << "\ngenerating bytecode" << std::endl;
	auto bpi = cmp::generate_bytecode(symbol_table, bytecode_size);

    if (!bpi.has_main) {
        std::cout << "no main function!" << std::endl;
        return 1;
    }

    if (bpi.bytecode.size() != bytecode_size) {
        std::cout << std::dec << "bytecode size is wrong:" << bpi.bytecode.size() << " / " << bytecode_size << std::endl;
        return 1;
    }

	// TODO: save to file
    
    std::cout << "\ncreating vm" << std::endl;

	// TODO: load from file

    vm::VirtualMachine vm(db);

    vm.code = reinterpret_cast<vm::Instruction*>(bpi.bytecode.data());
	vm.const_memory = new vm::Value[symbol_table.const_memory.size()];

	vm.main_start = bpi.main_start;

	for (size_t i = 0; i < symbol_table.const_memory.size(); i++) {
		vm.const_memory[i] = symbol_table.const_memory[i];
	}
    
    std::cout << "\nprocessing" << std::dec << std::endl;

    auto start = std::chrono::high_resolution_clock::now();

    vm::run_vm(vm);

    auto end = std::chrono::high_resolution_clock::now();

    // Calculate the duration in microseconds
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
	auto duration_msec = (double)duration / 1000.0;

    std::cout << "vm::run_vm execution time: " << duration_msec << " miliseconds" << std::endl;
    
    std::cout << "\nprocess finished" << std::endl;
	
    return 0;
}
#endif

#ifdef VM_ONLY
int main() {
	std::cout << "start vm only" << std::endl;

    vm::ClassDB db;
	vm::register_classes(db);

	/*

	std::cout << "\ncreating vm" << std::endl;
    vm::VirtualMachine vm;
    vm.db = &db;
    vm.code = reinterpret_cast<vm::Instruction*>(bytecode.data());
    vm.main_start = main_start;
	vm.const_memory = new vm::Value[symbol_table.const_memory.size()];

	for (size_t i = 0; i < symbol_table.const_memory.size(); i++) {
		vm.const_memory[i] = symbol_table.const_memory[i];
	}
    
    std::cout << "\nprocessing" << std::dec << std::endl;

    auto start = std::chrono::high_resolution_clock::now();

    vm::run_vm(vm);

    auto end = std::chrono::high_resolution_clock::now();

    // Calculate the duration in microseconds
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
	auto duration_msec = (double)duration / 1000.0;

    std::cout << "vm::run_vm execution time: " << duration_msec << " miliseconds" << std::endl;
    
    std::cout << "\nprocess finished" << std::endl;
	*/

	return 0;
}
#endif