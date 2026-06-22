#include <iostream>
#include <string>
#include <vector>
#include <assert.h>
#include <iomanip>
#include <fstream>

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
#include "text_color.hpp"

#include "bytecode_generator.hpp"
#include "cpp_generator.hpp"
#include "type_from_cpp.hpp"
#include "compiler_args.hpp"

#include "vm.hpp"
#include "instructions.hpp"
#include "class_db.hpp"
#include "execute.hpp"
#include "class_registrator.hpp"

void print_help() {
	std::cout <<
		"Usage: aupl [options] folder..." << std::endl <<
		"-h, --help				Display this message" << std::endl <<
		"-v, --version			Display version" << std::endl <<
		"--gen-no-bc			Generate no AUPL bytecode" << std::endl <<
		"--gen-cpp				Generate C++ code" << std::endl <<
		"--gen-js				Generate JavaScript code" << std::endl <<
		"--gen-gbc				Generate ???" << std::endl <<
		"-o [file]				Output file" << std::endl;
}

static std::vector<std::string>::iterator contains_arg(std::vector<std::string>& args, std::vector<std::string> searched) {
	for (auto s : searched) {
		auto it = std::find(args.begin(), args.end(), s);
		if (it != args.end()) {
			return it;
		}
	}

	return args.end();
}


int main(int argc, char** argv) {
	std::vector<std::string> args;

	for (int i = 1; i < argc; ++i) {
		std::string arg = argv[i];
		args.push_back(arg);
	}

	// print help
	if (contains_arg(args, {"--help", "-h"}) != args.end()) {
		print_help();
		return 0;
	}

	// print version
	if (contains_arg(args, {"--version", "-v"}) != args.end()) {
		std::cout << "Version: 0.1" << std::endl;
		return 0;
	}

	// generate no bytecode
	if (contains_arg(args, {"--gen-no-bc"}) != args.end()) {
		cmp::compiler_args().generate_bytecode = false;
	}

	// generate cpp
	if (contains_arg(args, {"--gen-cpp"}) != args.end()) {
		cmp::compiler_args().generate_cpp = true;
	}

	// output
	std::string output_path;
	{
		auto it = contains_arg(args, {"-o"});
		
		if (it != args.end()) {
			it++;
			
			if (it == args.end()) {
				std::cerr << "no arg for -o!" << std::endl;
				return 1;
			}

			output_path = *it;
		}
	}

	cmp::NameAnalyzer::na_debug_print = false;
	cmp::NameAnalyzer::na_debug_print_verbose = false;

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
        std::string content = cmp::get_text_from_file(file);
		
		cmp::parse_file(file, content, symbol_table);
    }

	// print parsed classes & functions
	for (auto [cn, cls] : symbol_table.classes) {
		if (cls->native_class_bind) {
			continue;
		}

		std::cout << cn << std::endl;
		
		for (auto [fn, f] : cls->functions) {
			std::cout << f->to_string() << std::endl;
		}
	}

	if (std::any_of(
			symbol_table.errors.begin(),
			symbol_table.errors.end(),
			[](cmp::Error err) {
				return err.level == cmp::Error::CRITICAL
					|| err.level == cmp::Error::ERROR;
			}
		)
	) {
		std::cerr << "found error during parsing" << std::endl;
		return 1;
	}

    cmp::NameAnalyzer name_analyzer(symbol_table);
    name_analyzer.resolve_variables();

    // TODO: short circuit if

    cmp::RegisterFormatConverter register_format_converter(symbol_table);
	register_format_converter.convert_to_register_format();

	if (std::any_of(
			symbol_table.errors.begin(),
			symbol_table.errors.end(),
			[](cmp::Error err) {
				return err.level == cmp::Error::CRITICAL
					|| err.level == cmp::Error::ERROR;
			}
		)
	) {
		std::cerr << "found error during name analysis" << std::endl;
		return 1;
	}

	// optimize
    std::vector<cmp::Optimizer*> optimizers;
    optimizers.push_back(static_cast<cmp::Optimizer*>(new cmp::ConstFoldingOptimizer()));
    optimizers.push_back(static_cast<cmp::Optimizer*>(new cmp::EraseUnusedVariableOptimizer()));

    //for (int i = 0; i < 1; ++i) {
    //    for (auto p : optimizers) {
    //        p->optimize(symbol_table);
    //    }
    //}
	
	// scope structure / memory layout
	symbol_table.generate_scope_structures();

	// print final classes & functions
	for (auto [cn, cls] : symbol_table.classes) {
		if (cls->native_class_bind) {
			continue;
		}

		std::cout << cn << std::endl;
		
		for (auto [fn, f] : cls->functions) {
			std::cout << f->to_string() << std::endl;
		}
	}

	if (cmp::compiler_args().generate_bytecode) {
		auto size_gen = cmp::BytecodeGenerator<true>(symbol_table);
		size_t bytecode_size = size_gen.generate_bytecode();
		
		auto code_gen = cmp::BytecodeGenerator<false>(symbol_table);
		auto bpi = code_gen.generate_bytecode();

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
		size_t const_size = bpi.const_memory.size();
		size_t main_start = bpi.main_start;

		output_file.write(reinterpret_cast<char*>(&code_size), sizeof(code_size));
		output_file.write(reinterpret_cast<char*>(&const_size), sizeof(const_size));
		output_file.write(reinterpret_cast<char*>(&main_start), sizeof(main_start));

		output_file.write(
			reinterpret_cast<char*>(bpi.bytecode.data()),
			sizeof(uint8_t) * code_size
		);

		for (auto& v : bpi.const_memory) {
			output_file.write(reinterpret_cast<char*>(&v.u8), sizeof(uint8_t));
		}

		output_file.close();
	}

	if (cmp::compiler_args().generate_cpp) {
		// generate C++
		std::ofstream hpp_file(output_path + "/cpp/output.hpp", std::ios::trunc);
		std::ofstream cpp_file(output_path + "/cpp/output.cpp", std::ios::trunc);

		cmp::CppCodeGenerator cpp_generator(symbol_table);
		cpp_generator.generate_cpp_code(hpp_file, cpp_file);

		std::cout << "written" << std::endl;

		hpp_file.close();
		cpp_file.close();
	}
	
    return 0;
}
