#include "compiler_args.hpp"

namespace cmp {

CompilerArgs &compiler_args(){
	static CompilerArgs singleton;
	return singleton;
}

}