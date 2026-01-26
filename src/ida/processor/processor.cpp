// Python Bytecode Processor Module for IDA Pro
// Handles instruction decoding, emulation, and disassembly output

#include <ida.hpp>
#include <idp.hpp>
#include <auto.hpp>
#include <bytes.hpp>
#include <lines.hpp>
#include <nalt.hpp>
#include <name.hpp>
#include <offset.hpp>
#include <segregs.hpp>
#include <frame.hpp>
#include <xref.hpp>

#include "../../core/common/types.hpp"
#include "../../core/common/platform.hpp"
#include "../../core/config/constants.hpp"
#include "../../core/version/version.hpp"
#include "../../bytecode/instruction/definition.hpp"
#include "../../bytecode/instruction/flags.hpp"
#include "../../bytecode/instruction/operand_types.hpp"
#include "../../bytecode/opcodes/table.hpp"
#include "../../bytecode/operators/compare.hpp"
#include "../../bytecode/operators/binary.hpp"
#include "../../bytecode/instruction/intrinsics.hpp"
#include "../../ida/common/registers.hpp"
#include "../../ida/common/specflags.hpp"

namespace pyc {
namespace processor {

// ============================================================================
// Register Names
// ============================================================================

static const char* const RegNames[] = {
    "sp",   // Stack pointer
    "pc",   // Program counter
    "cs",   // Code segment (virtual)
    "ds",   // Data segment (virtual)
};

// ============================================================================
// EXTENDED_ARG opcode constant
// ============================================================================

constexpr uint8_t EXTENDED_ARG = 144;

// ============================================================================
// Processor Module Class
// ============================================================================

struct pyc_t : public procmod_t {
    // Version info
    uint8_t version_major = 3;
    uint8_t version_minor = 14;
    version::family version_family = version::family::py314_plus;
    bool has_caches = true;
    uint8_t word_size = 2;
    
    // Cached code object info
    netnode code_node;
    bool code_node_valid = false;
    qvector<qstring> cached_names;
    qvector<qstring> cached_varnames;
    qvector<qstring> cached_consts;
    ea_t cached_code_base = BADADDR;
    
    // Event handler
    virtual ssize_t idaapi on_event(ssize_t msgid, va_list va) override;
    
    // Analysis functions
    int ana(insn_t* insn);
    int emu(const insn_t* insn);
    void out_insn(outctx_t& ctx);
    bool out_opnd(outctx_t& ctx, const op_t& op);
    
    // State management
    void load_from_netnode();
    void load_code_info(ea_t ea);
    
    // Name resolution
    bool get_name_string(ea_t ea, uint32_t idx, qstring* out);
    bool get_varname_string(ea_t ea, uint32_t idx, qstring* out);
    bool get_const_string(ea_t ea, uint32_t idx, qstring* out);
};

// ============================================================================
// State Management
// ============================================================================

void pyc_t::load_from_netnode() {
    netnode pyc_node(config::NODE_NAME, 0, false);
    if (pyc_node != BADNODE) {
        version_major = (uint8_t)pyc_node.altval(config::NODE_VERSION_MAJOR);
        version_minor = (uint8_t)pyc_node.altval(config::NODE_VERSION_MINOR);
        version_family = (version::family)pyc_node.altval(config::NODE_VERSION_FAMILY);
        has_caches = pyc_node.altval(config::NODE_HAS_CACHES) != 0;
        word_size = (uint8_t)pyc_node.altval(config::NODE_WORD_SIZE);
        
        if (version_family == version::family::unknown)
            version_family = version::get_family(version_major, version_minor);
        
        bytecode::opcodes::set_version_family(version_family);
    }
}

void pyc_t::load_code_info(ea_t ea) {
    // Find which code segment this address belongs to
    segment_t* seg = getseg(ea);
    if (!seg) return;
    
    ea_t code_base = seg->start_ea;
    
    // Check if already cached
    if (code_node_valid && cached_code_base == code_base)
        return;
    
    // Build netnode name for this code object
    qstring co_node_name;
    co_node_name.sprnt("%s%llX", config::CODE_NODE_PREFIX, (uint64_t)code_base);
    
    netnode co_node(co_node_name.c_str(), 0, false);
    if (co_node == BADNODE) {
        code_node_valid = false;
        return;
    }
    
    code_node = co_node;
    cached_code_base = code_base;
    
    // Load names blob
    cached_names.clear();
    qstring names_blob;
    size_t names_len = co_node.supstr(&names_blob, config::CO_NAMES_BLOB);
    if (names_len > 0) {
        const char* p = names_blob.c_str();
        const char* end = p + names_blob.length();
        while (p < end) {
            size_t len = strlen(p);
            cached_names.push_back(qstring(p, len));
            p += len + 1;
        }
    }
    
    // Load varnames blob
    cached_varnames.clear();
    qstring varnames_blob;
    size_t varnames_len = co_node.supstr(&varnames_blob, config::CO_VARNAMES_BLOB);
    if (varnames_len > 0) {
        const char* p = varnames_blob.c_str();
        const char* end = p + varnames_blob.length();
        while (p < end) {
            size_t len = strlen(p);
            cached_varnames.push_back(qstring(p, len));
            p += len + 1;
        }
    }
    
    // Load consts blob
    cached_consts.clear();
    qstring consts_blob;
    size_t consts_len = co_node.supstr(&consts_blob, config::CO_CONSTS_BLOB);
    if (consts_len > 0) {
        const char* p = consts_blob.c_str();
        const char* end = p + consts_blob.length();
        while (p < end) {
            size_t len = strlen(p);
            cached_consts.push_back(qstring(p, len));
            p += len + 1;
        }
    }
    
    code_node_valid = true;
}

bool pyc_t::get_name_string(ea_t ea, uint32_t idx, qstring* out) {
    load_code_info(ea);
    // For LOAD_GLOBAL in 3.11+, actual index is arg >> 1
    if (version_family == version::family::py311_313 || 
        version_family == version::family::py314_plus)
        idx = idx >> 1;
    
    if (idx < cached_names.size()) {
        *out = cached_names[idx];
        return true;
    }
    return false;
}

bool pyc_t::get_varname_string(ea_t ea, uint32_t idx, qstring* out) {
    load_code_info(ea);
    if (idx < cached_varnames.size()) {
        *out = cached_varnames[idx];
        return true;
    }
    return false;
}

bool pyc_t::get_const_string(ea_t ea, uint32_t idx, qstring* out) {
    load_code_info(ea);
    if (idx < cached_consts.size()) {
        *out = cached_consts[idx];
        return true;
    }
    return false;
}

// ============================================================================
// Analyzer (ana)
// ============================================================================

int pyc_t::ana(insn_t* insn) {
    ea_t ea = insn->ea;
    
    // Handle EXTENDED_ARG chaining
    uint32_t extended_arg = 0;
    ea_t start_ea = ea;
    
    if (version_family >= version::family::py36_310) {
        // Wordcode: check for EXTENDED_ARG prefix
        while (get_byte(ea) == EXTENDED_ARG) {
            extended_arg = (extended_arg | get_byte(ea + 1)) << 8;
            ea += 2;
        }
    }
    
    uint8_t opcode = get_byte(ea);
    
    const bytecode::insn_def* def = bytecode::opcodes::get_current_insn_def(opcode);
    
    insn->itype = opcode;
    insn->Op1.type = o_void;
    insn->Op2.type = o_void;
    insn->Op3.type = o_void;
    
    // Calculate instruction size
    int base_size;
    uint32_t arg = 0;
    
    if (version_family <= version::family::py30_35) {
        // Pre-wordcode: variable size
        if (opcode >= 90) {  // HAVE_ARGUMENT
            arg = get_byte(ea + 1) | (get_byte(ea + 2) << 8);
            arg = extended_arg | arg;
            base_size = 3;
        } else {
            base_size = 1;
        }
    } else {
        // Wordcode: always 2 bytes
        arg = get_byte(ea + 1);
        arg = extended_arg | arg;
        base_size = 2;
    }
    
    insn->size = (ea - start_ea) + base_size;
    
    // Add cache entries for 3.11+
    if (has_caches && def && def->cache_entries > 0) {
        insn->size += def->cache_entries * 2;
    }
    
    // Set operand based on instruction type
    if (def && def->op_type != bytecode::operand_type::none) {
        insn->Op1.dtype = dt_dword;
        insn->Op1.offb = (ea - start_ea) + 1;
        insn->Op1.value = arg;
        
        switch (def->op_type) {
            case bytecode::operand_type::const_idx:
                insn->Op1.type = o_imm;
                insn->Op1.specflag1 = ida::SPEC_CONST;
                break;
                
            case bytecode::operand_type::name_idx:
                insn->Op1.type = o_imm;
                insn->Op1.specflag1 = ida::SPEC_NAME;
                break;
                
            case bytecode::operand_type::local_idx:
                insn->Op1.type = o_imm;
                insn->Op1.specflag1 = ida::SPEC_LOCAL;
                break;
                
            case bytecode::operand_type::free_idx:
                insn->Op1.type = o_imm;
                insn->Op1.specflag1 = ida::SPEC_FREE;
                break;
                
            case bytecode::operand_type::compare:
                insn->Op1.type = o_imm;
                insn->Op1.specflag1 = ida::SPEC_CMP;
                break;
                
            case bytecode::operand_type::jrel: {
                // Relative forward jump
                ea_t target;
                if (version_family >= version::family::py36_310) {
                    // Wordcode: arg is instruction count, not byte offset
                    // For 3.11+, offset is from after cache entries
                    ea_t after_caches = ea + base_size;
                    if (has_caches && def)
                        after_caches += def->cache_entries * 2;
                    target = after_caches + arg * 2;
                } else {
                    target = ea + base_size + arg;
                }
                insn->Op1.type = o_near;
                insn->Op1.addr = target;
                break;
            }
                
            case bytecode::operand_type::jback: {
                // Relative backward jump
                ea_t after_caches = ea + base_size;
                if (has_caches && def)
                    after_caches += def->cache_entries * 2;
                insn->Op1.type = o_near;
                insn->Op1.addr = after_caches - arg * 2;
                break;
            }
                
            case bytecode::operand_type::jabs: {
                // Absolute jump (pre-3.10 uses byte offset)
                ea_t code_base = getseg(ea) ? getseg(ea)->start_ea : 0;
                insn->Op1.type = o_near;
                if (version_family < version::family::py36_310) {
                    insn->Op1.addr = code_base + arg;
                } else {
                    // 3.10+ uses instruction offset
                    insn->Op1.addr = code_base + arg * 2;
                }
                break;
            }
                
            case bytecode::operand_type::intrinsic1:
            case bytecode::operand_type::intrinsic2:
            case bytecode::operand_type::byte:
            case bytecode::operand_type::hasarg:
            default:
                insn->Op1.type = o_imm;
                break;
        }
    }
    
    return insn->size;
}

// ============================================================================
// Emulator (emu)
// ============================================================================

int pyc_t::emu(const insn_t* insn) {
    const bytecode::insn_def* def = bytecode::opcodes::get_current_insn_def(insn->itype);
    
    bool flow = true;
    
    if (def) {
        // Stop flow for returns and unconditional jumps
        if (def->flags & (bytecode::flag_ret | bytecode::flag_stop))
            flow = false;
        
        // Stop flow for unconditional jumps
        if ((def->flags & bytecode::flag_jump) && !(def->flags & bytecode::flag_cond))
            flow = false;
    }
    
    // Create cross-references for jumps
    if (insn->Op1.type == o_near) {
        ea_t target = insn->Op1.addr;
        
        if (is_mapped(target)) {
            if (def && (def->flags & bytecode::flag_call)) {
                add_cref(insn->ea, target, fl_CN);
            } else if (def && (def->flags & bytecode::flag_jump)) {
                add_cref(insn->ea, target, fl_JN);
            }
        }
    }
    
    // Flow to next instruction
    if (flow) {
        add_cref(insn->ea, insn->ea + insn->size, fl_F);
    }
    
    return 1;
}

// ============================================================================
// Output
// ============================================================================

void pyc_t::out_insn(outctx_t& ctx) {
    // Output mnemonic
    ctx.out_mnemonic();
    
    // Output operand if present
    if (ctx.insn.Op1.type != o_void) {
        ctx.out_char(' ');
        ctx.out_one_operand(0);
    }
    
    ctx.flush_outbuf();
}

bool pyc_t::out_opnd(outctx_t& ctx, const op_t& op) {
    const bytecode::insn_def* def = bytecode::opcodes::get_current_insn_def(ctx.insn.itype);
    
    switch (op.type) {
        case o_void:
            return false;
            
        case o_imm: {
            switch (op.specflag1) {
                case ida::SPEC_CONST: {
                    // Resolve constant
                    qstring const_str;
                    if (get_const_string(ctx.insn.ea, (uint32_t)op.value, &const_str)) {
                        // Truncate if too long
                        if (const_str.length() > 40) {
                            const_str.resize(37);
                            const_str.append("...");
                        }
                        ctx.out_line(const_str.c_str(), COLOR_STRING);
                    } else {
                        ctx.out_long((uint32_t)op.value, 10);
                    }
                    break;
                }
                    
                case ida::SPEC_NAME: {
                    // Resolve name
                    qstring name;
                    if (get_name_string(ctx.insn.ea, (uint32_t)op.value, &name)) {
                        ctx.out_line(name.c_str(), COLOR_IMPNAME);
                    } else {
                        ctx.out_long((uint32_t)op.value, 10);
                    }
                    break;
                }
                    
                case ida::SPEC_LOCAL: {
                    // Resolve local variable name
                    qstring varname;
                    if (get_varname_string(ctx.insn.ea, (uint32_t)op.value, &varname)) {
                        ctx.out_line(varname.c_str(), COLOR_LOCNAME);
                    } else {
                        ctx.out_char('$');
                        ctx.out_long((uint32_t)op.value, 10);
                    }
                    break;
                }
                    
                case ida::SPEC_FREE: {
                    // Free variable
                    ctx.out_keyword("free_");
                    ctx.out_long((uint32_t)op.value, 10);
                    break;
                }
                    
                case ida::SPEC_CMP: {
                    // Comparison operator
                    // Python 3.12+ encodes comparison in bits 5-7
                    bool is_312_plus = (version_major > 3) || 
                                       (version_major == 3 && version_minor >= 12);
                    const char* cmp_name = bytecode::operators::get_cmp_op_name((uint8_t)op.value, is_312_plus);
                    ctx.out_line(cmp_name, COLOR_SYMBOL);
                    break;
                }
                    
                case ida::SPEC_BINOP: {
                    // Binary operator
                    const char* op_name = bytecode::operators::get_binary_op_name((uint8_t)op.value);
                    ctx.out_line(op_name, COLOR_SYMBOL);
                    break;
                }
                    
                default:
                    // Check for special instructions
                    if (def) {
                        if (def->op_type == bytecode::operand_type::intrinsic1) {
                            const char* name = bytecode::get_intrinsic_1_name((uint8_t)op.value);
                            ctx.out_line(name, COLOR_MACRO);
                            break;
                        }
                        if (def->op_type == bytecode::operand_type::intrinsic2) {
                            const char* name = bytecode::get_intrinsic_2_name((uint8_t)op.value);
                            ctx.out_line(name, COLOR_MACRO);
                            break;
                        }
                    }
                    // Default: output as decimal
                    ctx.out_long((uint32_t)op.value, 10);
                    break;
            }
            break;
        }
            
        case o_near: {
            // Jump target
            if (!ctx.out_name_expr(op, op.addr, BADADDR)) {
                ctx.out_tagon(COLOR_ERROR);
                ctx.out_btoa((uint32_t)op.addr, 16);
                ctx.out_tagoff(COLOR_ERROR);
            }
            break;
        }
            
        default:
            return false;
    }
    
    return true;
}

// ============================================================================
// Event Handler
// ============================================================================

ssize_t idaapi pyc_t::on_event(ssize_t msgid, va_list va) {
    switch (msgid) {
        case processor_t::ev_init: {
            // Initialize instruction tables with default version
            bytecode::opcodes::set_version_family(version::family::py314_plus);
            return 1;
        }
            
        case processor_t::ev_term:
            return 1;
            
        case processor_t::ev_newfile:
        case processor_t::ev_oldfile:
            load_from_netnode();
            return 1;
            
        case processor_t::ev_ana_insn: {
            insn_t* insn = va_arg(va, insn_t*);
            return ana(insn);
        }
            
        case processor_t::ev_emu_insn: {
            const insn_t* insn = va_arg(va, const insn_t*);
            return emu(insn);
        }
            
        case processor_t::ev_out_insn: {
            outctx_t* ctx = va_arg(va, outctx_t*);
            out_insn(*ctx);
            return 1;
        }
            
        case processor_t::ev_out_operand: {
            outctx_t* ctx = va_arg(va, outctx_t*);
            const op_t* op = va_arg(va, const op_t*);
            return out_opnd(*ctx, *op) ? 1 : -1;
        }
            
        case processor_t::ev_out_mnem: {
            outctx_t* ctx = va_arg(va, outctx_t*);
            const bytecode::insn_def* def = bytecode::opcodes::get_current_insn_def(ctx->insn.itype);
            if (def && def->mnemonic) {
                ctx->out_line(def->mnemonic, COLOR_INSN);
            } else {
                char buf[16];
                qsnprintf(buf, sizeof(buf), "op_%02X", ctx->insn.itype);
                ctx->out_line(buf, COLOR_INSN);
            }
            return 1;
        }
            
        case processor_t::ev_is_call_insn: {
            const insn_t* insn = va_arg(va, const insn_t*);
            const bytecode::insn_def* def = bytecode::opcodes::get_current_insn_def(insn->itype);
            return (def && (def->flags & bytecode::flag_call)) ? 1 : -1;
        }
            
        case processor_t::ev_is_ret_insn: {
            const insn_t* insn = va_arg(va, const insn_t*);
            bool strict = va_argi(va, bool);
            const bytecode::insn_def* def = bytecode::opcodes::get_current_insn_def(insn->itype);
            return (def && (def->flags & bytecode::flag_ret)) ? 1 : -1;
        }
            
        case processor_t::ev_is_basic_block_end: {
            const insn_t* insn = va_arg(va, const insn_t*);
            bool call_insn_stops = va_argi(va, bool);
            const bytecode::insn_def* def = bytecode::opcodes::get_current_insn_def(insn->itype);
            if (def) {
                if (def->flags & (bytecode::flag_ret | bytecode::flag_stop))
                    return 1;
                if ((def->flags & bytecode::flag_jump) && !(def->flags & bytecode::flag_cond))
                    return 1;
            }
            return -1;
        }
            
        case processor_t::ev_may_be_func: {
            // Any location could potentially be a function start
            return 100;  // Probability
        }
            
        default:
            return 0;
    }
}

// ============================================================================
// Notify Callback (procmod factory)
// ============================================================================

static ssize_t idaapi notify(void*, int msgid, va_list) {
    if (msgid == processor_t::ev_get_procmod)
        return size_t(new pyc_t);
    return 0;
}

// ============================================================================
// Processor Definitions
// ============================================================================

// Short and long names
static const char* const shnames[] = { "pyc", nullptr };
static const char* const lnames[] = { "Python Compiled Bytecode", nullptr };

// Assembler definition
static const asm_t pycasm = {
    AS_COLON | AS_N2CHR | ASH_HEXF3 | AS_UNEQU,
    0,
    "Python Assembler",
    0,
    nullptr,    // header
    ".org",     // origin
    ".end",     // end
    ";",        // comment
    '"',        // string delimiter
    '\'',       // char delimiter
    "\"'",      // special chars
    ".ascii",   // ascii string directive
    ".byte",    // byte directive
    ".word",    // word directive
    ".dword",   // dword directive
    nullptr,    // qword directive
    nullptr,    // oword directive
    nullptr,    // float directive
    nullptr,    // double directive
    nullptr,    // tbyte directive
    nullptr,    // packreal directive
    nullptr,    // array directive
    ".ds %s",   // reserve space directive
    ".equ",     // equate directive
    nullptr,    // assume directive
    nullptr,    // checkarg_dispatch
    nullptr,    // register dispatch
    nullptr,    // end of line
    nullptr,    // origin depends on proc
    0,          // cmnt2
    nullptr,    // Low 8 bits of segment
    nullptr,    // High 8 bits of segment
    nullptr,    // ds directive for uninitialized data
};

static const asm_t* const asms[] = { &pycasm, nullptr };

// Return instruction sequence
static const uchar ret_code[] = { 83 };  // RETURN_VALUE
static const bytes_t ret_codes[] = {
    { sizeof(ret_code), ret_code },
    { 0, nullptr }
};

// Instruction array (filled at init)
static instruc_t Instructions[256];

} // namespace processor
} // namespace pyc

// ============================================================================
// Processor Descriptor Export
// ============================================================================

extern "C" __attribute__((visibility("default"))) processor_t LPH = {
    IDP_INTERFACE_VERSION,              // version
    pyc::PLFM_PYC,                       // id (from core/common/types.hpp)
    PRN_HEX                             // flags
        | PR_RNAMESOK
        | PR_NO_SEGMOVE,
    0,                                  // flags2
    8,                                  // cnbits (8 bits per byte in code)
    8,                                  // dnbits (8 bits per byte in data)
    
    pyc::processor::shnames,            // short processor names
    pyc::processor::lnames,             // long processor names
    pyc::processor::asms,               // assemblers
    
    pyc::processor::notify,             // notify callback
    
    pyc::processor::RegNames,           // register names
    qnumber(pyc::processor::RegNames),  // number of registers
    
    pyc::ida::REG_CS,                   // first segment register
    pyc::ida::REG_DS,                   // last segment register
    0,                                  // number of CS assumes
    pyc::ida::REG_CS,                   // default CS register
    pyc::ida::REG_DS,                   // default DS register
    
    nullptr,                            // codestart arrays
    pyc::processor::ret_codes,          // retcodes
    
    0,                                  // first instruction
    255,                                // last instruction
    pyc::processor::Instructions,       // instruction array
    
    0,                                  // sizeof(googles)
    { 0, },                             // googles (unused)
    
    83,                                 // icode_return (RETURN_VALUE)
    nullptr,                            // unused slot
};
