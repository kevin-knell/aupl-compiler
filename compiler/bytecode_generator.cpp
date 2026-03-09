#include "bytecode_generator.hpp"
#include <cstddef>
#include <iostream>
#include "symbol_table.hpp"
#include "statement.hpp"
#include "color.hpp"
#include <iomanip>

#define SET_STRING(ins, size) strings[static_cast<uint8_t>(vm::Instruction::ins)] = #ins;

std::string strings[256] = { "" };
    
void init_strings() {
    OPCODES(SET_STRING, _)
}

size_t cmp::generate_bytecode_layout(SymbolTable& symbol_table) {
	size_t bytecode_size = 0;

    for (auto [cn, cls] : symbol_table.classes) {
		if (cls->native_class_bind) continue;
        std::cout << cn << std::endl;

        for (auto [fn, f] : cls->functions) {
        	std::function<void(cmp::ScopePtr)> accumulate_scope_sizes = [&](cmp::ScopePtr scope) {
				scope->starting_address = bytecode_size;

				std::cout << scope->get_full_name() << ": " << std::hex << (int)(bytecode_size) << std::dec << std::endl;

				for (auto stmt : scope->body) {
					cmp::BytecodeGenerationInfo bgi(symbol_table, cls, f, scope);
					bgi.bytecode_size = bytecode_size;
					std::cout << "\t" << stmt->to_string() << ": " << std::hex << bytecode_size << std::dec << std::endl;
					bytecode_size += stmt->get_bytecode_size(bgi);
				}
            	
				std::cout << std::endl;

				bytecode_size += 1; // HALT
                
				for (auto& lower : scope->lower_scopes) {
					if (auto lower_scope = lower.lock())
						accumulate_scope_sizes(lower_scope);
					else
						std::cout << "Warning: lower scope is expired" << std::endl;
                }
            };
            accumulate_scope_sizes(f->scope);
        }
    }

	return bytecode_size;
}

cmp::BytecodeProductInfo cmp::generate_bytecode(cmp::SymbolTable &symbol_table, size_t bytecode_size) {
	init_strings();

	std::vector<uint8_t> bytecode;
    bytecode.reserve(bytecode_size);
    size_t main_start = 0;
    bool has_main = false;

    for (auto [cn, cls] : symbol_table.classes) {
		if (cls->native_class_bind) continue;
        std::cout << cn << std::endl;

        for (auto [fn, f] : cls->functions) {
            if (fn == "main") {
                main_start = bytecode.size();
                has_main = true;
            }

            std::cout << "\n" << f->to_string() << std::endl;

			std::function<void(cmp::ScopePtr)> generate_bytecode = [&](cmp::ScopePtr scope) {
				std::cout << scope->get_full_name() << " " << (int)scope->starting_address << " {" << std::endl;

				for (auto stmt : scope->body) {
					cmp::BytecodeGenerationInfo bgi(symbol_table, cls, f, scope);
				
					std::vector<uint8_t> bytecode_new = stmt->generate_bytecode(bgi);
					bytecode.insert(bytecode.end(), bytecode_new.begin(), bytecode_new.end());

					if (stmt->get_kind() != cmp::Statement::LABEL && bytecode_new.size() != stmt->get_bytecode_size(bgi)) {
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

				std::cout << "}" << std::endl;

				if (fn == "main") bytecode.push_back(static_cast<uint8_t>(vm::Instruction::HALT));
				else bytecode.push_back(static_cast<uint8_t>(vm::Instruction::RET));
                
				for (auto& lower : scope->lower_scopes) {
					if (auto lower_scope = lower.lock())
						generate_bytecode(lower_scope);
					else
						std::cout << "Warning: lower scope is expired" << std::endl;
                }
            };

			generate_bytecode(f->scope);
        }
    }

	return BytecodeProductInfo{
		.bytecode = bytecode,
		.main_start = main_start,
		.has_main = has_main
	};
}
