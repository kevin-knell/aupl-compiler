#include <iostream>
#include <string>
#include <vector>
#include <assert.h>
#include <iomanip>
#include <fstream>

#ifdef COMPILER
#include "file_manager.hpp"
#include "tokenizer.hpp"
#include "symbol_builder.hpp"
#include "symbol_table.hpp"
#include "class_parser.hpp"
#include "statement.hpp"
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


void print_help() {
	std::cout <<
		"Usage: aupl [options] folder..." << std::endl <<
		"-h, --help             Display this message" << std::endl <<
		"-v, --version          Display version" << std::endl <<
		"-o [file]              Output file" << std::endl;
}


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

	// output
	std::string output_path;
	
	for (int i = 1; i < argc; ++i) {
		std::string arg = argv[i];

		if (arg == "-o") {
			if (i + 1 >= argc) {
				std::cerr << "no arg for -o!" << std::endl;
				return 1;
			}

			output_path = argv[i + 1];
			break;
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

    cmp::RegisterFormatConverter register_format_converter(symbol_table);
	register_format_converter.convert_to_register_format();

#ifdef OPTIMIZE
    std::cout << "\noptimizing code" << std::endl;

    std::vector<cmp::Optimizer*> optimizers;
    optimizers.push_back((cmp::Optimizer*)new cmp::ConstFoldingOptimizer());
    optimizers.push_back((cmp::Optimizer*)new cmp::EraseUnusedVariableOptimizer());

    for (int i = 0; i < 1; ++i) {
        for (auto p : optimizers) {
            p->optimize(symbol_table);
        }
    }
#endif

    std::cout << "\ngenerating scope structures" << std::endl;
	symbol_table.generate_scope_structures();

    std::cout << "\ngenerating bytecode layout" << std::endl;
	auto size_gen = cmp::BytecodeGenerator<true>(symbol_table);
    size_t bytecode_size = size_gen.generate_bytecode(symbol_table, 0);

    std::cout << "\ngenerating bytecode" << std::endl;
	auto code_gen = cmp::BytecodeGenerator<false>(symbol_table);
    auto bpi = code_gen.generate_bytecode(symbol_table, bytecode_size);

    if (!bpi.has_main) {
        std::cout << "no main function!" << std::endl;
        return 1;
    }

    if (bpi.bytecode.size() != bytecode_size) {
        std::cout << std::dec << "bytecode size is wrong:" << bpi.bytecode.size() << " / " << bytecode_size << std::endl;
        return 1;
    }

	// save to file
	std::ofstream output_file(output_path, std::ios::binary | std::ios::trunc);

	size_t code_size = bytecode_size;
	size_t const_size = symbol_table.const_memory.size();
	size_t main_start = bpi.main_start;

	output_file.write(reinterpret_cast<char*>(&code_size), sizeof(code_size));
	output_file.write(reinterpret_cast<char*>(&const_size), sizeof(const_size));
	output_file.write(reinterpret_cast<char*>(&main_start), sizeof(main_start));

	output_file.write(
		reinterpret_cast<char*>(bpi.bytecode.data()),
		sizeof(uint8_t) * code_size
	);

	for (auto& v : symbol_table.const_memory) {
		output_file.write(reinterpret_cast<char*>(&v.u8), sizeof(uint8_t));
	}

	output_file.close();
	
    return 0;
}
#endif

#ifdef VM_ONLY
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
#endif