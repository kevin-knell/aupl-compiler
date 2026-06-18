#pragma once

namespace cmp {

struct CompilerArgs {
	bool generate_bytecode = true;
	bool generate_cpp = false;
	bool generate_js = false;
	bool generate_gbc = false;

	bool bcg_debug_print = false;
	bool bcg_debug_print_verbose = false;
};

CompilerArgs& compiler_args();

} // namespace cmp
