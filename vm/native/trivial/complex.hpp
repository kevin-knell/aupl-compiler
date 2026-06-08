#include <complex>
#include "native.hpp"

namespace vm {
    class ClassDB;
} // namespace vm

namespace auplib {

class complex {
public:
    static void register_to_db(vm::ClassDB& db);

    static const double ONE;
    static const double I;
    
    std::complex<double> value;

    complex() = default;

    double re() const;
    double im() const;
};

MARK_TRIVIAL(complex)

}