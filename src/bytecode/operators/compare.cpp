// Python comparison operator implementation

#include "compare.hpp"

namespace pyc {
namespace bytecode {
namespace operators {

static const char* const cmp_ops[] = {
    "<", "<=", "==", "!=", ">", ">="
};

const char* get_cmp_op_name(uint8_t op) {
    // Python 3.12+ comparison operators encode differently
    // The upper 4 bits contain the comparison operation
    uint8_t cmp_idx = op >> 4;
    if (cmp_idx < 6)
        return cmp_ops[cmp_idx];
    return "??";
}

} // namespace operators
} // namespace bytecode
} // namespace pyc
