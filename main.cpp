#include <iostream>
#include <string>
#include <vector>
#include <assert.h>
#include <chrono>

#include "file_manager.hpp"
#include "tokenizer.hpp"
#include "symbol_builder.hpp"
#include "symbol_table.hpp"
#include "statement.hpp"
#include "class.hpp"
#include "vm.hpp"
#include "instructions.hpp"
#include "class_db.hpp"
#include "execute.hpp"
#include "name_analyzer.hpp"
#include "bytecode_generation_info.hpp"
#include "expression.hpp"
#include "register_format_converter.hpp"
#include "const_folding_optimizer.hpp"
#include "erase_unused_variable_optimizer.hpp"
#include "color.hpp"
#include <iomanip>

#define SET_STRING(ins, size) strings[static_cast<uint8_t>(vm::Instruction::ins)] = #ins;

std::string strings[256] = { "" };
    
void init_strings() {
    OPCODES(SET_STRING, _)
}

int main() {
    init_strings();
    
    // create class db
    vm::ClassDB db;

    // parse user source code
    std::string folder_path = "./source"; // Replace with your folder path
    auto files = cmp::get_all_files_in_folder(folder_path);

    cmp::SymbolTable symbol_table;

    for (const auto& file : files) {
        std::cout << "Tokenizing file: " << file << std::endl;
        std::string content = cmp::get_text_from_file(file);
        auto tokens = cmp::tokenize(content);

        for (cmp::Token t : tokens) {
            //std::cout << (int)t.type << " " << t.value << " " << t.pos << std::endl;
        }

        // Parse tokens using SymbolBuilder
        cmp::SymbolBuilder builder(tokens, symbol_table);

        // Attempt to parse class definitions
        try {
            size_t i = 0;
            while (i < tokens.size()) {
                if (tokens[i].value == "class") {
                    builder.parse_class();
                }
                ++i;
            }
        } catch (const std::exception& ex) {
            std::cerr << "Error during parsing: " << ex.what() << std::endl;
        }
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

    for (auto [n, cls] : symbol_table.classes) {
        cls->static_scope->generate_structure();
        std::cout << cls->static_scope->get_full_name() << std::endl;
        std::cout << cls->static_scope->sructure_to_string() << std::endl;
    }

    for (auto [n, cls] : symbol_table.classes) {
        cls->scope->generate_structure();
        std::cout << cls->scope->get_full_name() << std::endl;
        std::cout << cls->scope->sructure_to_string() << std::endl;
    }

    for (auto [n, cls] : symbol_table.classes) {
        for (auto [fn, f] : cls->functions) {
            f->scope->generate_structure();
            std::cout << f->scope->get_full_name() << std::endl;
            std::cout << f->scope->sructure_to_string() << std::endl;
        }
    }

    //return 0;
    
    std::cout << "\ncreating vm" << std::endl;

    vm::VirtualMachine vm;
    vm.db = &db;

    std::cout << "\ngenerating bytecode layout" << std::endl;

    size_t bytecode_size = 0;

    for (auto [cn, cls] : symbol_table.classes) {
        std::cout << cn << std::endl;

        for (auto [fn, f] : cls->functions) {
            cmp::BytecodeGenerationInfo bgi(symbol_table, cls, f, f->scope);
            bytecode_size += f->get_bytecode_size(bgi) + 1;
            std::cout << f->head_to_string() << ": " << (int)bytecode_size << std::endl;
            f->starting_address = bytecode_size;
        }
    }

    std::cout << "\ngenerating bytecode" << std::endl;

    std::vector<uint8_t> bytecode;
    bytecode.reserve(bytecode_size);
    size_t main_start;
	bool has_main = false;

    for (auto [cn, cls] : symbol_table.classes) {
        std::cout << cn << std::endl;

        for (auto [fn, f] : cls->functions) {
            if (fn == "main") {
                main_start = bytecode.size();
				has_main = true;
            }

            std::cout << "\n" << f->to_string() << std::endl;

            for (auto stmt : f->scope->body) {
                cmp::BytecodeGenerationInfo bgi(symbol_table, cls, f, f->scope);
                std::vector<uint8_t> bytecode_new = stmt->generate_bytecode(bgi);
                bytecode.insert(bytecode.end(), bytecode_new.begin(), bytecode_new.end());

                if (bytecode_new.size() != stmt->get_bytecode_size(bgi)) {
                    std::cout << "wrong bytecode size: " << stmt->to_string() << std::endl;
                }

                for (size_t i = 0; i < bytecode_new.size(); ++i) {
					uint8_t opcode = bytecode_new[i];
					auto op_sizes = vm::OP_SIZES[opcode];

					std::cout << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << (int)opcode << " " << C_KEYWORD(strings[opcode]) << " ";

					size_t offset = i + 1;
					for (size_t j = 1; j < op_sizes.size(); ++j) {
						size_t size = op_sizes[j];
						for (size_t k = 0; k < size; ++k) {
							std::cout << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << (int)bytecode_new[offset + k];
						}
						std::cout << " ";
						offset += size;
					}

					i = offset - 1;
					std::cout << std::endl;
				}
            }

            bytecode.push_back(static_cast<uint8_t>(vm::Instruction::HALT));
        }
    }

	if (!has_main) {
		std::cout << "no main function!" << std::endl;
		return 1;
	}

	if (bytecode.size() != bytecode_size) {
        std::cout << "bytecode size is wrong" << std::endl;
        return 1;
    }

    vm.code = reinterpret_cast<vm::Instruction*>(bytecode.data());
    vm.main_start = main_start;
    
    std::cout << "\nprocessing" << std::endl;

    auto start = std::chrono::high_resolution_clock::now();

    vm::run_vm(vm);

    auto end = std::chrono::high_resolution_clock::now();

    // Calculate the duration in microseconds
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

    std::cout << "vm::run_vm execution time: " << duration << " microseconds" << std::endl;
    
    std::cout << "\nprocess finished" << std::endl;
    return 0;


	int x;
	int y;
	int* p;
	int* q;

	x = y;
	p = q;

	p = &x;
	*p = x;
}
