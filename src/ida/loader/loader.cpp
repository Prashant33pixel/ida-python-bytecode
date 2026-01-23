// Python Bytecode Loader Module for IDA Pro
// Handles .pyc file detection, parsing, and segment creation

#include <ida.hpp>
#include <idp.hpp>
#include <loader.hpp>
#include <diskio.hpp>
#include <bytes.hpp>
#include <name.hpp>
#include <nalt.hpp>
#include <segment.hpp>
#include <entry.hpp>
#include <auto.hpp>
#include <typeinf.hpp>

#include "../../core/common/types.hpp"
#include "../../core/common/platform.hpp"
#include "../../core/config/constants.hpp"
#include "../../core/version/version.hpp"
#include "../../core/version/magic.hpp"
#include "../../marshal/types/type_codes.hpp"
#include "../../marshal/code_object/code_object.hpp"
#include "../../format/header/header.hpp"

namespace pyc {
namespace loader {

// ============================================================================
// Global Version State (set during loading)
// ============================================================================

static uint8_t g_py_major = 0;
static uint8_t g_py_minor = 0;

// ============================================================================
// Helper Functions
// ============================================================================

static uint32_t read_le32(linput_t* li) {
    uint8_t buf[4];
    if (qlread(li, buf, 4) != 4) return 0;
    return buf[0] | (buf[1] << 8) | (buf[2] << 16) | (buf[3] << 24);
}

static uint8_t read_byte(linput_t* li) {
    uint8_t b;
    if (qlread(li, &b, 1) != 1) return 0;
    return b;
}

// ============================================================================
// Segment Creation Helper
// ============================================================================

static bool create_pyc_segment(ea_t start, ea_t end, const char* name, 
                               const char* sclass, uchar perm) {
    segment_t seg;
    seg.start_ea = start;
    seg.end_ea = end;
    seg.sel = allocate_selector((start >> 4) & 0xFFFF);
    seg.bitness = 1;  // 32-bit addressing
    seg.align = saRelByte;
    seg.comb = scPub;
    seg.perm = perm;
    
    if (!add_segm_ex(&seg, name, sclass, ADDSEG_NOSREG | ADDSEG_OR_DIE))
        return false;
    
    return true;
}

// ============================================================================
// Code Object Info Structure
// ============================================================================

struct pyc_code_info_t {
    qstring name;           // Function/code name
    qstring qualname;       // Qualified name (3.11+)
    qstring filename;       // Source filename
    uint32_t argcount;
    uint32_t posonlyargcount;
    uint32_t kwonlyargcount;
    uint32_t nlocals;
    uint32_t stacksize;
    uint32_t flags;
    uint32_t firstlineno;
    qvector<uint8_t> code;
    qvector<qstring> consts;
    qvector<qstring> names;
    qvector<qstring> varnames;
    qvector<uint8_t> linetable;
    ea_t code_ea;           // Where code is loaded
    int depth;              // Nesting depth (0 for module)
    
    // Nested code objects found in consts
    qvector<pyc_code_info_t> nested_codes;
};

// ============================================================================
// Forward Declarations
// ============================================================================

static bool skip_marshal_object(linput_t* li);
static bool read_marshal_as_string(linput_t* li, qstring* out, 
                                   qvector<pyc_code_info_t>* nested_codes, int depth);
static bool parse_code_object_inner(linput_t* li, pyc_code_info_t* info, int depth);

// ============================================================================
// Skip Marshal Object (version-aware)
// ============================================================================

static bool skip_string(linput_t* li, uint8_t type) {
    uint32_t len;
    if (type == marshal::TYPE_SHORT_ASCII || type == marshal::TYPE_SHORT_ASCII_INTERNED) {
        len = read_byte(li);
    } else {
        len = read_le32(li);
    }
    return qlseek(li, len, SEEK_CUR) != -1;
}

static bool skip_code_object(linput_t* li) {
    // Skip code object fields based on Python version
    bool is_py3 = g_py_major >= 3;
    bool has_posonlyargcount = is_py3 && g_py_minor >= 8;
    bool has_qualname = is_py3 && g_py_minor >= 11;
    bool has_exceptiontable = is_py3 && g_py_minor >= 11;
    bool nlocals_separate = !(is_py3 && g_py_minor >= 11);
    
    // Skip integer fields
    qlseek(li, 4, SEEK_CUR);  // argcount
    if (has_posonlyargcount)
        qlseek(li, 4, SEEK_CUR);  // posonlyargcount
    qlseek(li, 4, SEEK_CUR);  // kwonlyargcount
    if (nlocals_separate)
        qlseek(li, 4, SEEK_CUR);  // nlocals
    qlseek(li, 4, SEEK_CUR);  // stacksize
    qlseek(li, 4, SEEK_CUR);  // flags
    
    // Skip code (bytes)
    if (!skip_marshal_object(li)) return false;
    
    // Skip consts (tuple) - recursive, may contain code objects
    if (!skip_marshal_object(li)) return false;
    
    // Skip names (tuple)
    if (!skip_marshal_object(li)) return false;
    
    // Python 3.11+ changed order
    if (is_py3 && g_py_minor >= 11) {
        // localsplusnames
        if (!skip_marshal_object(li)) return false;
        // localspluskinds
        if (!skip_marshal_object(li)) return false;
    } else {
        // varnames
        if (!skip_marshal_object(li)) return false;
        // freevars
        if (!skip_marshal_object(li)) return false;
        // cellvars
        if (!skip_marshal_object(li)) return false;
    }
    
    // filename
    if (!skip_marshal_object(li)) return false;
    
    // name
    if (!skip_marshal_object(li)) return false;
    
    // qualname (3.11+)
    if (has_qualname) {
        if (!skip_marshal_object(li)) return false;
    }
    
    // firstlineno - raw int in 3.11+, marshal object in earlier versions
    if (is_py3 && g_py_minor >= 11) {
        qlseek(li, 4, SEEK_CUR);  // Raw 4-byte int
    } else {
        if (!skip_marshal_object(li)) return false;
    }
    
    // linetable/lnotab
    if (!skip_marshal_object(li)) return false;
    
    // exceptiontable (3.11+)
    if (has_exceptiontable) {
        if (!skip_marshal_object(li)) return false;
    }
    
    return true;
}

static bool skip_marshal_object(linput_t* li) {
    uint8_t type;
    if (qlread(li, &type, 1) != 1) return false;
    type &= ~marshal::FLAG_REF;
    
    switch (type) {
        case marshal::TYPE_NULL:
        case marshal::TYPE_NONE:
        case marshal::TYPE_STOPITER:
        case marshal::TYPE_ELLIPSIS:
        case marshal::TYPE_FALSE:
        case marshal::TYPE_TRUE:
            return true;
            
        case marshal::TYPE_INT:
            return qlseek(li, 4, SEEK_CUR) != -1;
            
        case marshal::TYPE_INT64:
        case marshal::TYPE_BINARY_FLOAT:
            return qlseek(li, 8, SEEK_CUR) != -1;
            
        case marshal::TYPE_BINARY_COMPLEX:
            return qlseek(li, 16, SEEK_CUR) != -1;
            
        case marshal::TYPE_LONG: {
            int32_t n = (int32_t)read_le32(li);
            if (n < 0) n = -n;
            return qlseek(li, n * 2, SEEK_CUR) != -1;
        }
            
        case marshal::TYPE_STRING:
        case marshal::TYPE_INTERNED:
        case marshal::TYPE_ASCII:
        case marshal::TYPE_ASCII_INTERNED:
        case marshal::TYPE_UNICODE:
            return skip_string(li, type);
            
        case marshal::TYPE_SHORT_ASCII:
        case marshal::TYPE_SHORT_ASCII_INTERNED:
            return skip_string(li, type);
            
        case marshal::TYPE_TUPLE:
        case marshal::TYPE_LIST:
        case marshal::TYPE_SET:
        case marshal::TYPE_FROZENSET: {
            uint32_t n = read_le32(li);
            for (uint32_t i = 0; i < n; i++)
                if (!skip_marshal_object(li)) return false;
            return true;
        }
            
        case marshal::TYPE_SMALL_TUPLE: {
            uint8_t n = read_byte(li);
            for (uint8_t i = 0; i < n; i++)
                if (!skip_marshal_object(li)) return false;
            return true;
        }
            
        case marshal::TYPE_DICT: {
            while (true) {
                int64_t pos = qltell(li);
                uint8_t key_type;
                if (qlread(li, &key_type, 1) != 1) return false;
                if ((key_type & ~marshal::FLAG_REF) == marshal::TYPE_NULL) return true;
                qlseek(li, pos);
                if (!skip_marshal_object(li)) return false;
                if (!skip_marshal_object(li)) return false;
            }
        }
            
        case marshal::TYPE_CODE:
            return skip_code_object(li);
            
        case marshal::TYPE_REF:
        case marshal::TYPE_STRINGREF:
            return qlseek(li, 4, SEEK_CUR) != -1;
            
        case marshal::TYPE_FLOAT: {
            uint8_t len = read_byte(li);
            return qlseek(li, len, SEEK_CUR) != -1;
        }
            
        case marshal::TYPE_COMPLEX: {
            uint8_t rlen = read_byte(li);
            if (qlseek(li, rlen, SEEK_CUR) == -1) return false;
            uint8_t ilen = read_byte(li);
            return qlseek(li, ilen, SEEK_CUR) != -1;
        }
            
        default:
            msg("PYC: Unknown marshal type 0x%02X at offset %lld\n", type, (long long)qltell(li)-1);
            return false;
    }
}

// ============================================================================
// Read Marshal String
// ============================================================================

static bool read_marshal_string(linput_t* li, qstring* out) {
    uint8_t type;
    if (qlread(li, &type, 1) != 1) return false;
    type &= ~marshal::FLAG_REF;
    
    uint32_t len;
    switch (type) {
        case marshal::TYPE_SHORT_ASCII:
        case marshal::TYPE_SHORT_ASCII_INTERNED:
            len = read_byte(li);
            break;
        case marshal::TYPE_ASCII:
        case marshal::TYPE_ASCII_INTERNED:
        case marshal::TYPE_STRING:
        case marshal::TYPE_INTERNED:
        case marshal::TYPE_UNICODE:
            len = read_le32(li);
            break;
        case marshal::TYPE_NONE:
            *out = "None";
            return true;
        case marshal::TYPE_REF:
            read_le32(li);  // Skip ref index
            out->sprnt("<ref>");
            return true;
        case marshal::TYPE_STRINGREF:
            // Reference to an interned string (Python 2.x)
            read_le32(li);  // Skip ref index
            out->sprnt("<ref>");
            return true;
        default:
            return false;
    }
    
    out->resize(len);
    if (len > 0 && qlread(li, out->begin(), len) != len)
        return false;
    
    return true;
}

// ============================================================================
// Read Marshal Bytes
// ============================================================================

static bool read_marshal_bytes(linput_t* li, qvector<uint8_t>* out) {
    uint8_t type;
    if (qlread(li, &type, 1) != 1) return false;
    type &= ~marshal::FLAG_REF;
    
    // Handle reference to previously seen bytes object
    if (type == marshal::TYPE_REF) {
        qlseek(li, 4, SEEK_CUR);  // Skip ref index
        out->clear();
        return true;
    }
    
    uint32_t len;
    if (type == marshal::TYPE_SHORT_ASCII || type == marshal::TYPE_SHORT_ASCII_INTERNED) {
        len = read_byte(li);
    } else if (type == marshal::TYPE_STRING || type == marshal::TYPE_ASCII || 
               type == marshal::TYPE_ASCII_INTERNED || type == marshal::TYPE_INTERNED) {
        len = read_le32(li);
    } else {
        return false;
    }
    
    out->resize(len);
    if (len > 0 && qlread(li, out->begin(), len) != len)
        return false;
    
    return true;
}

// ============================================================================
// Read Tuple Header
// ============================================================================

static bool read_tuple_header(linput_t* li, uint32_t* count) {
    uint8_t type;
    if (qlread(li, &type, 1) != 1) return false;
    type &= ~marshal::FLAG_REF;
    
    if (type == marshal::TYPE_SMALL_TUPLE) {
        *count = read_byte(li);
        return true;
    } else if (type == marshal::TYPE_TUPLE) {
        *count = read_le32(li);
        return true;
    }
    return false;
}

// ============================================================================
// Read Marshal as String Representation
// ============================================================================

static bool read_marshal_as_string(linput_t* li, qstring* out, 
                                   qvector<pyc_code_info_t>* nested_codes, int depth) {
    int64_t start_pos = qltell(li);
    uint8_t type;
    if (qlread(li, &type, 1) != 1) return false;
    type &= ~marshal::FLAG_REF;
    
    switch (type) {
        case marshal::TYPE_NONE:
            *out = "None";
            return true;
        case marshal::TYPE_TRUE:
            *out = "True";
            return true;
        case marshal::TYPE_FALSE:
            *out = "False";
            return true;
        case marshal::TYPE_ELLIPSIS:
            *out = "...";
            return true;
        case marshal::TYPE_INT: {
            int32_t val = (int32_t)read_le32(li);
            out->sprnt("%d", val);
            return true;
        }
        case marshal::TYPE_INT64: {
            int64_t val;
            if (qlread(li, &val, 8) != 8) return false;
            out->sprnt("%lld", (long long)val);
            return true;
        }
        case marshal::TYPE_BINARY_FLOAT: {
            double val;
            if (qlread(li, &val, 8) != 8) return false;
            out->sprnt("%g", val);
            return true;
        }
        case marshal::TYPE_STRING:
        case marshal::TYPE_INTERNED:
        case marshal::TYPE_ASCII:
        case marshal::TYPE_ASCII_INTERNED:
        case marshal::TYPE_UNICODE:
        case marshal::TYPE_SHORT_ASCII:
        case marshal::TYPE_SHORT_ASCII_INTERNED: {
            qlseek(li, start_pos);  // Rewind
            qstring str;
            if (!read_marshal_string(li, &str)) return false;
            // Escape and quote the string
            out->sprnt("'%s'", str.c_str());
            return true;
        }
        case marshal::TYPE_TUPLE:
        case marshal::TYPE_SMALL_TUPLE: {
            qlseek(li, start_pos);  // Rewind
            uint32_t count;
            if (!read_tuple_header(li, &count)) return false;
            *out = "(";
            for (uint32_t i = 0; i < count; i++) {
                if (i > 0) out->append(", ");
                qstring item;
                if (!read_marshal_as_string(li, &item, nested_codes, depth)) {
                    item = "?";
                }
                out->append(item);
                if (out->length() > 60) {
                    out->append("...");
                    // Skip remaining items
                    for (uint32_t j = i + 1; j < count; j++)
                        skip_marshal_object(li);
                    break;
                }
            }
            out->append(")");
            return true;
        }
        case marshal::TYPE_CODE: {
            // Parse the nested code object and store it
            pyc_code_info_t nested;
            if (parse_code_object_inner(li, &nested, depth + 1)) {
                out->sprnt("<code '%s'>", nested.name.c_str());
                if (nested_codes)
                    nested_codes->push_back(nested);
            } else {
                *out = "<code ?>";
            }
            return true;
        }
        case marshal::TYPE_REF: {
            uint32_t ref = read_le32(li);
            out->sprnt("<ref:%u>", ref);
            return true;
        }
        case marshal::TYPE_LONG: {
            int32_t n = (int32_t)read_le32(li);
            bool neg = n < 0;
            if (neg) n = -n;
            qlseek(li, n * 2, SEEK_CUR);  // Skip the digits
            out->sprnt("<long%s>", neg ? "-" : "+");
            return true;
        }
        default: {
            out->sprnt("<type:0x%02X>", type);
            qlseek(li, start_pos);  // Rewind
            skip_marshal_object(li);  // Skip it
            return true;
        }
    }
}

// ============================================================================
// Parse String Tuple
// ============================================================================

static bool parse_string_tuple(linput_t* li, qvector<qstring>* out) {
    // Check type first - might be a ref to a previously seen tuple
    uint8_t type;
    int64_t pos = qltell(li);
    if (qlread(li, &type, 1) != 1) return false;
    type &= ~marshal::FLAG_REF;
    
    if (type == marshal::TYPE_REF) {
        // Reference to a previously seen object - skip the 4-byte index
        // We can't resolve refs without tracking all objects, so return empty
        qlseek(li, 4, SEEK_CUR);
        out->clear();
        return true;
    }
    
    // Rewind and read as tuple
    qlseek(li, pos);
    
    uint32_t count;
    if (!read_tuple_header(li, &count))
        return false;
    
    out->resize(count);
    for (uint32_t i = 0; i < count; i++) {
        if (!read_marshal_string(li, &(*out)[i])) {
            // Not a string - skip and use placeholder
            (*out)[i].sprnt("<item_%u>", i);
        }
    }
    return true;
}

// ============================================================================
// Parse Code Object
// ============================================================================

static bool parse_code_object_inner(linput_t* li, pyc_code_info_t* info, int depth) {
    info->depth = depth;
    
    bool is_py3 = g_py_major >= 3;
    bool has_kwonlyargcount = is_py3;  // Python 3.0+ only
    bool has_posonlyargcount = is_py3 && g_py_minor >= 8;
    bool has_qualname = is_py3 && g_py_minor >= 11;
    bool has_linetable = is_py3 && g_py_minor >= 10;
    bool has_exceptiontable = is_py3 && g_py_minor >= 11;
    bool nlocals_separate = !(is_py3 && g_py_minor >= 11);
    
    // Read code object fields
    info->argcount = read_le32(li);
    
    if (has_posonlyargcount)
        info->posonlyargcount = read_le32(li);
    else
        info->posonlyargcount = 0;
    
    if (has_kwonlyargcount)
        info->kwonlyargcount = read_le32(li);
    else
        info->kwonlyargcount = 0;
    
    if (nlocals_separate)
        info->nlocals = read_le32(li);
    else
        info->nlocals = 0;
    
    info->stacksize = read_le32(li);
    info->flags = read_le32(li);
    
    // Read code bytes
    if (!read_marshal_bytes(li, &info->code)) {
        msg("PYC: Failed to read code bytes at depth %d\n", depth);
        return false;
    }
    
    // Read consts tuple - may contain nested code objects
    uint32_t const_count;
    if (!read_tuple_header(li, &const_count)) {
        msg("PYC: Failed to read consts tuple header at depth %d\n", depth);
        return false;
    }
    
    info->consts.resize(const_count);
    for (uint32_t i = 0; i < const_count; i++) {
        if (!read_marshal_as_string(li, &info->consts[i], &info->nested_codes, depth)) {
            info->consts[i].sprnt("<const_%u>", i);
        }
    }
    
    // Read names tuple
    if (!parse_string_tuple(li, &info->names)) {
        msg("PYC: Failed to read names tuple at depth %d\n", depth);
        return false;
    }
    
    // Python 3.11+ changed order
    if (is_py3 && g_py_minor >= 11) {
        // localsplusnames
        if (!parse_string_tuple(li, &info->varnames)) {
            msg("PYC: Failed to read localsplusnames at depth %d\n", depth);
            return false;
        }
        
        // localspluskinds (bytes object) - skip
        qvector<uint8_t> kinds;
        read_marshal_bytes(li, &kinds);
    } else {
        // Pre-3.11: varnames, freevars, cellvars
        qvector<qstring> freevars, cellvars;
        if (!parse_string_tuple(li, &info->varnames)) {
            info->varnames.clear();
        }
        if (!parse_string_tuple(li, &freevars)) {
            // Optional
        }
        if (!parse_string_tuple(li, &cellvars)) {
            // Optional
        }
    }
    
    // Read filename
    if (!read_marshal_string(li, &info->filename)) {
        info->filename = "<unknown>";
    }
    
    // Read name
    if (!read_marshal_string(li, &info->name)) {
        info->name = "<code>";
    }
    
    // Read qualname (3.11+)
    if (has_qualname) {
        if (!read_marshal_string(li, &info->qualname))
            info->qualname = info->name;
    } else {
        info->qualname = info->name;
    }
    
    // Read firstlineno - raw 4-byte int in all Python versions
    info->firstlineno = read_le32(li);
    
    // Read linetable/lnotab
    read_marshal_bytes(li, &info->linetable);
    
    // Read exception table (3.11+)
    if (has_exceptiontable) {
        qvector<uint8_t> exctable;
        read_marshal_bytes(li, &exctable);
    }
    
    return true;
}

// ============================================================================
// Load All Code Objects into IDA
// ============================================================================

static ea_t g_next_code_addr = 0x10000;

static void load_code_object(pyc_code_info_t& code, const qstring& prefix) {
    // Create segment name
    qstring seg_name;
    if (code.name == "<module>")
        seg_name = ".code";
    else if (prefix.empty())
        seg_name.sprnt(".code_%s", code.name.c_str());
    else
        seg_name.sprnt(".code_%s.%s", prefix.c_str(), code.name.c_str());
    
    // Calculate addresses
    ea_t code_base = g_next_code_addr;
    ea_t code_end = code_base + code.code.size();
    g_next_code_addr = (code_end + 0x100) & ~0xFF;  // Align to 256 bytes
    
    // Create code segment
    if (!create_pyc_segment(code_base, code_end, seg_name.c_str(), "CODE",
                           SEGPERM_READ | SEGPERM_EXEC)) {
        msg("PYC: Failed to create segment %s\n", seg_name.c_str());
        return;
    }
    
    // Load code bytes
    mem2base(code.code.begin(), code_base, code_end, -1);
    code.code_ea = code_base;
    
    // Store code object metadata in netnode
    qstring co_node_name;
    co_node_name.sprnt("%s%llX", config::CODE_NODE_PREFIX, (uint64_t)code_base);
    netnode co_node;
    co_node.create(co_node_name.c_str());
    co_node.altset(config::CO_ARGCOUNT, code.argcount);
    co_node.altset(config::CO_KWONLYARGCOUNT, code.kwonlyargcount);
    co_node.altset(config::CO_NLOCALS, code.nlocals);
    co_node.altset(config::CO_STACKSIZE, code.stacksize);
    co_node.altset(config::CO_FLAGS, code.flags);
    co_node.altset(config::CO_FIRSTLINENO, code.firstlineno);
    co_node.altset(config::CO_CODE_SIZE, (uint32_t)code.code.size());
    
    // Store names as null-separated blob
    qstring names_blob;
    for (const auto& name : code.names) {
        names_blob.append(name);
        names_blob.append('\0');
    }
    co_node.supset(config::CO_NAMES_BLOB, names_blob.c_str(), names_blob.length());
    
    // Store varnames
    qstring varnames_blob;
    for (const auto& name : code.varnames) {
        varnames_blob.append(name);
        varnames_blob.append('\0');
    }
    co_node.supset(config::CO_VARNAMES_BLOB, varnames_blob.c_str(), varnames_blob.length());
    
    // Store consts representations
    qstring consts_blob;
    for (const auto& c : code.consts) {
        consts_blob.append(c);
        consts_blob.append('\0');
    }
    co_node.supset(config::CO_CONSTS_BLOB, consts_blob.c_str(), consts_blob.length());
    
    // Create entry point / function name
    qstring func_name;
    if (prefix.empty())
        func_name = code.name;
    else
        func_name.sprnt("%s.%s", prefix.c_str(), code.name.c_str());
    
    // Clean up name for IDA (replace <> with _)
    func_name.replace("<", "_");
    func_name.replace(">", "_");
    
    if (code.depth == 0) {
        add_entry(0, code_base, func_name.c_str(), true);
    } else {
        force_name(code_base, func_name.c_str());
    }
    
    // Queue for auto-analysis
    auto_make_proc(code_base);
    
    msg("PYC: Loaded %s at %llX (%zu bytes, %zu consts, %zu names)\n",
        func_name.c_str(), (uint64_t)code_base, 
        code.code.size(), code.consts.size(), code.names.size());
    
    // Recursively load nested code objects
    qstring new_prefix;
    if (prefix.empty()) {
        if (code.name != "<module>")
            new_prefix = code.name;
    } else {
        new_prefix.sprnt("%s.%s", prefix.c_str(), code.name.c_str());
    }
    
    for (auto& nested : code.nested_codes) {
        load_code_object(nested, new_prefix);
    }
}

// ============================================================================
// File Detection (accept_file)
// ============================================================================

// Check if file is a raw marshal code object (no pyc header)
static bool is_raw_marshal_code(linput_t* li) {
    qlseek(li, 0);
    uint8_t first_byte;
    if (qlread(li, &first_byte, 1) != 1)
        return false;
    
    // TYPE_CODE = 0xe3 ('c'), may have FLAG_REF (0x80)
    uint8_t type = first_byte & ~marshal::FLAG_REF;
    return type == marshal::TYPE_CODE;
}

static int idaapi accept_pyc_file(
    qstring* fileformatname,
    qstring* processor,
    linput_t* li,
    const char* /*filename*/)
{
    // Check minimum file size
    if (qlsize(li) < 8)
        return 0;
    
    // Read magic number
    qlseek(li, 0);
    uint32_t magic = read_le32(li);
    
    // Check CRLF marker in magic
    if ((magic & 0xFFFF0000) != 0x0A0D0000 && (magic >> 16) != 0x0D0A) {
        // Not a standard pyc header - check for raw marshal
        if (is_raw_marshal_code(li)) {
            fileformatname->sprnt("Python Compiled Bytecode (Raw Marshal, assuming 3.13)");
            *processor = "pyc";
            return 1;  // Lower priority than proper pyc files
        }
        return 0;
    }
    
    // Extract version magic
    uint16_t ver_magic = magic & 0xFFFF;
    
    // Look up in magic table
    const version::magic_entry* entry = version::lookup_magic(ver_magic);
    if (!entry)
        return 0;
    
    // Verify file size is at least header size
    if (qlsize(li) < entry->header_size)
        return 0;
    
    // File accepted
    fileformatname->sprnt("Python Compiled Bytecode (%s)", entry->name);
    *processor = "pyc";
    
    return 1 | ACCEPT_FIRST;
}

// ============================================================================
// Main Loading Function (load_file)
// ============================================================================

static void idaapi load_pyc_file(linput_t* li, ushort /*neflags*/, const char* fileformatname)
{
    // Set processor type
    set_processor_type("pyc", SETPROC_LOADER);
    
    // Read and parse magic
    qlseek(li, 0);
    uint32_t magic = read_le32(li);
    uint16_t ver_magic = magic & 0xFFFF;
    
    const version::magic_entry* ver = version::lookup_magic(ver_magic);
    bool is_raw_marshal = false;
    
    // Default version info for raw marshal (Python 3.13)
    static const version::magic_entry raw_marshal_version = {
        0, 3, 13, 0, 2, true, "Python 3.13 (Raw Marshal)"
    };
    
    if (!ver) {
        // Check if it's a raw marshal file
        if (is_raw_marshal_code(li)) {
            ver = &raw_marshal_version;
            is_raw_marshal = true;
            msg("PYC: Loading raw marshal file (assuming Python 3.13)\n");
        } else {
            loader_failure("Unknown Python version magic: 0x%04X", ver_magic);
            return;
        }
    } else {
        msg("PYC: Loading %s file (magic 0x%04X)\n", ver->name, ver_magic);
    }
    
    // Set global version for marshal parsing
    g_py_major = ver->major;
    g_py_minor = ver->minor;
    
    // Position to start of code object
    if (is_raw_marshal) {
        qlseek(li, 0);  // Raw marshal starts at beginning
    } else {
        qlseek(li, ver->header_size);  // Skip pyc header
    }
    
    // Read type code - should be TYPE_CODE
    uint8_t type;
    if (qlread(li, &type, 1) != 1) {
        loader_failure("Failed to read marshal type");
        return;
    }
    
    bool has_ref = (type & marshal::FLAG_REF) != 0;
    type &= ~marshal::FLAG_REF;
    
    if (type != marshal::TYPE_CODE) {
        loader_failure("Expected code object at root, got type 0x%02X", type);
        return;
    }
    
    // Parse the root code object
    pyc_code_info_t root_code;
    if (!parse_code_object_inner(li, &root_code, 0)) {
        loader_failure("Failed to parse root code object");
        return;
    }
    
    msg("PYC: Parsed module '%s' from '%s'\n",
        root_code.name.c_str(), root_code.filename.c_str());
    
    // Store version info in netnode
    netnode pyc_node;
    pyc_node.create(config::NODE_NAME);
    pyc_node.altset(config::NODE_VERSION_MAJOR, ver->major);
    pyc_node.altset(config::NODE_VERSION_MINOR, ver->minor);
    pyc_node.altset(config::NODE_VERSION_FAMILY, static_cast<uint32_t>(version::get_family(ver->major, ver->minor)));
    pyc_node.altset(config::NODE_HAS_CACHES, ver->has_caches ? 1 : 0);
    pyc_node.altset(config::NODE_WORD_SIZE, ver->word_size);
    
    // Reset address counter and load all code objects
    g_next_code_addr = 0x10000;
    load_code_object(root_code, qstring());
    
    msg("PYC: Successfully loaded %s module\n", ver->name);
}

} // namespace loader
} // namespace pyc

// ============================================================================
// Loader Descriptor Export
// ============================================================================

extern "C" __attribute__((visibility("default"))) loader_t LDSC = {
    IDP_INTERFACE_VERSION,
    0,                                  // Flags
    pyc::loader::accept_pyc_file,       // accept_file
    pyc::loader::load_pyc_file,         // load_file
    nullptr,                            // save_file
    nullptr,                            // move_segm
    nullptr,                            // init_loader_options
};
