// Python magic number table implementation
// Contains all known magic numbers for Python 2.0 through 3.14+

#include "magic.hpp"

namespace pyc {
namespace version {

// Complete magic number table (sorted by magic value for binary search potential)
const magic_entry magic_table[] = {
    // Python 2.0-2.2 (very old, rare)
    { 50823, 2, 0,  8, 1, false, "Python 2.0" },
    { 60202, 2, 1,  8, 1, false, "Python 2.1" },
    { 60717, 2, 2,  8, 1, false, "Python 2.2" },
    
    // Python 2.3
    { 62011, 2, 3,  8, 1, false, "Python 2.3" },
    { 62021, 2, 3,  8, 1, false, "Python 2.3" },
    
    // Python 2.4
    { 62041, 2, 4,  8, 1, false, "Python 2.4" },
    { 62051, 2, 4,  8, 1, false, "Python 2.4" },
    { 62061, 2, 4,  8, 1, false, "Python 2.4" },
    
    // Python 2.5
    { 62071, 2, 5,  8, 1, false, "Python 2.5" },
    { 62081, 2, 5,  8, 1, false, "Python 2.5" },
    { 62091, 2, 5,  8, 1, false, "Python 2.5" },
    { 62092, 2, 5,  8, 1, false, "Python 2.5" },
    { 62101, 2, 5,  8, 1, false, "Python 2.5" },
    { 62111, 2, 5,  8, 1, false, "Python 2.5" },
    { 62121, 2, 5,  8, 1, false, "Python 2.5" },
    { 62131, 2, 5,  8, 1, false, "Python 2.5" },
    
    // Python 2.6
    { 62151, 2, 6,  8, 1, false, "Python 2.6" },
    { 62161, 2, 6,  8, 1, false, "Python 2.6" },
    
    // Python 2.7
    { 62171, 2, 7,  8, 1, false, "Python 2.7" },
    { 62181, 2, 7,  8, 1, false, "Python 2.7" },
    { 62191, 2, 7,  8, 1, false, "Python 2.7" },
    { 62201, 2, 7,  8, 1, false, "Python 2.7" },
    { 62211, 2, 7,  8, 1, false, "Python 2.7" },
    
    // Python 3.0
    { 3131,  3, 0,  8, 1, false, "Python 3.0" },
    { 3141,  3, 1,  8, 1, false, "Python 3.1" },
    { 3160,  3, 2,  8, 1, false, "Python 3.2" },
    
    // Python 3.3+ (12-byte header with source size)
    { 3190,  3, 3, 12, 1, false, "Python 3.3" },
    { 3250,  3, 4, 12, 1, false, "Python 3.4" },
    { 3310,  3, 5, 12, 1, false, "Python 3.5" },
    { 3351,  3, 5, 12, 1, false, "Python 3.5" },
    
    // Python 3.6+ (wordcode format - 2 bytes per instruction)
    { 3379,  3, 6, 12, 2, false, "Python 3.6" },
    
    // Python 3.7+ (16-byte header with hash-based invalidation)
    { 3390,  3, 7, 16, 2, false, "Python 3.7" },
    { 3391,  3, 7, 16, 2, false, "Python 3.7" },
    { 3392,  3, 7, 16, 2, false, "Python 3.7" },
    { 3393,  3, 7, 16, 2, false, "Python 3.7" },
    { 3394,  3, 7, 16, 2, false, "Python 3.7" },
    
    // Python 3.8
    { 3400,  3, 8, 16, 2, false, "Python 3.8" },
    { 3401,  3, 8, 16, 2, false, "Python 3.8" },
    { 3410,  3, 8, 16, 2, false, "Python 3.8" },
    { 3411,  3, 8, 16, 2, false, "Python 3.8" },
    { 3412,  3, 8, 16, 2, false, "Python 3.8" },
    { 3413,  3, 8, 16, 2, false, "Python 3.8" },
    
    // Python 3.9
    { 3420,  3, 9, 16, 2, false, "Python 3.9" },
    { 3421,  3, 9, 16, 2, false, "Python 3.9" },
    { 3422,  3, 9, 16, 2, false, "Python 3.9" },
    { 3423,  3, 9, 16, 2, false, "Python 3.9" },
    { 3424,  3, 9, 16, 2, false, "Python 3.9" },
    { 3425,  3, 9, 16, 2, false, "Python 3.9" },
    
    // Python 3.10
    { 3430,  3, 10, 16, 2, false, "Python 3.10" },
    { 3431,  3, 10, 16, 2, false, "Python 3.10" },
    { 3432,  3, 10, 16, 2, false, "Python 3.10" },
    { 3433,  3, 10, 16, 2, false, "Python 3.10" },
    { 3434,  3, 10, 16, 2, false, "Python 3.10" },
    { 3435,  3, 10, 16, 2, false, "Python 3.10" },
    { 3436,  3, 10, 16, 2, false, "Python 3.10" },
    { 3437,  3, 10, 16, 2, false, "Python 3.10" },
    { 3438,  3, 10, 16, 2, false, "Python 3.10" },
    { 3439,  3, 10, 16, 2, false, "Python 3.10" },
    
    // Python 3.11 (inline caches)
    { 3450,  3, 11, 16, 2, true, "Python 3.11" },
    { 3451,  3, 11, 16, 2, true, "Python 3.11" },
    { 3452,  3, 11, 16, 2, true, "Python 3.11" },
    { 3453,  3, 11, 16, 2, true, "Python 3.11" },
    { 3454,  3, 11, 16, 2, true, "Python 3.11" },
    { 3455,  3, 11, 16, 2, true, "Python 3.11" },
    { 3456,  3, 11, 16, 2, true, "Python 3.11" },
    { 3457,  3, 11, 16, 2, true, "Python 3.11" },
    { 3458,  3, 11, 16, 2, true, "Python 3.11" },
    { 3459,  3, 11, 16, 2, true, "Python 3.11" },
    { 3460,  3, 11, 16, 2, true, "Python 3.11" },
    { 3461,  3, 11, 16, 2, true, "Python 3.11" },
    { 3462,  3, 11, 16, 2, true, "Python 3.11" },
    { 3463,  3, 11, 16, 2, true, "Python 3.11" },
    { 3464,  3, 11, 16, 2, true, "Python 3.11" },
    { 3465,  3, 11, 16, 2, true, "Python 3.11" },
    { 3466,  3, 11, 16, 2, true, "Python 3.11" },
    { 3467,  3, 11, 16, 2, true, "Python 3.11" },
    { 3468,  3, 11, 16, 2, true, "Python 3.11" },
    { 3469,  3, 11, 16, 2, true, "Python 3.11" },
    { 3470,  3, 11, 16, 2, true, "Python 3.11" },
    { 3471,  3, 11, 16, 2, true, "Python 3.11" },
    { 3472,  3, 11, 16, 2, true, "Python 3.11" },
    { 3473,  3, 11, 16, 2, true, "Python 3.11" },
    { 3474,  3, 11, 16, 2, true, "Python 3.11" },
    { 3475,  3, 11, 16, 2, true, "Python 3.11" },
    { 3476,  3, 11, 16, 2, true, "Python 3.11" },
    { 3477,  3, 11, 16, 2, true, "Python 3.11" },
    { 3478,  3, 11, 16, 2, true, "Python 3.11" },
    { 3479,  3, 11, 16, 2, true, "Python 3.11" },
    { 3480,  3, 11, 16, 2, true, "Python 3.11" },
    { 3481,  3, 11, 16, 2, true, "Python 3.11" },
    { 3482,  3, 11, 16, 2, true, "Python 3.11" },
    { 3483,  3, 11, 16, 2, true, "Python 3.11" },
    { 3484,  3, 11, 16, 2, true, "Python 3.11" },
    { 3485,  3, 11, 16, 2, true, "Python 3.11" },
    { 3486,  3, 11, 16, 2, true, "Python 3.11" },
    { 3487,  3, 11, 16, 2, true, "Python 3.11" },
    { 3488,  3, 11, 16, 2, true, "Python 3.11" },
    { 3489,  3, 11, 16, 2, true, "Python 3.11" },
    { 3490,  3, 11, 16, 2, true, "Python 3.11" },
    { 3491,  3, 11, 16, 2, true, "Python 3.11" },
    { 3492,  3, 11, 16, 2, true, "Python 3.11" },
    { 3493,  3, 11, 16, 2, true, "Python 3.11" },
    { 3494,  3, 11, 16, 2, true, "Python 3.11" },
    { 3495,  3, 11, 16, 2, true, "Python 3.11" },
    
    // Python 3.12
    { 3500,  3, 12, 16, 2, true, "Python 3.12" },
    { 3501,  3, 12, 16, 2, true, "Python 3.12" },
    { 3502,  3, 12, 16, 2, true, "Python 3.12" },
    { 3503,  3, 12, 16, 2, true, "Python 3.12" },
    { 3504,  3, 12, 16, 2, true, "Python 3.12" },
    { 3505,  3, 12, 16, 2, true, "Python 3.12" },
    { 3506,  3, 12, 16, 2, true, "Python 3.12" },
    { 3507,  3, 12, 16, 2, true, "Python 3.12" },
    { 3508,  3, 12, 16, 2, true, "Python 3.12" },
    { 3509,  3, 12, 16, 2, true, "Python 3.12" },
    { 3510,  3, 12, 16, 2, true, "Python 3.12" },
    { 3511,  3, 12, 16, 2, true, "Python 3.12" },
    { 3512,  3, 12, 16, 2, true, "Python 3.12" },
    { 3513,  3, 12, 16, 2, true, "Python 3.12" },
    { 3514,  3, 12, 16, 2, true, "Python 3.12" },
    { 3515,  3, 12, 16, 2, true, "Python 3.12" },
    { 3516,  3, 12, 16, 2, true, "Python 3.12" },
    { 3517,  3, 12, 16, 2, true, "Python 3.12" },
    { 3518,  3, 12, 16, 2, true, "Python 3.12" },
    { 3519,  3, 12, 16, 2, true, "Python 3.12" },
    { 3520,  3, 12, 16, 2, true, "Python 3.12" },
    { 3521,  3, 12, 16, 2, true, "Python 3.12" },
    { 3522,  3, 12, 16, 2, true, "Python 3.12" },
    { 3523,  3, 12, 16, 2, true, "Python 3.12" },
    { 3524,  3, 12, 16, 2, true, "Python 3.12" },
    { 3525,  3, 12, 16, 2, true, "Python 3.12" },
    { 3526,  3, 12, 16, 2, true, "Python 3.12" },
    { 3527,  3, 12, 16, 2, true, "Python 3.12" },
    { 3528,  3, 12, 16, 2, true, "Python 3.12" },
    { 3529,  3, 12, 16, 2, true, "Python 3.12" },
    { 3530,  3, 12, 16, 2, true, "Python 3.12" },
    { 3531,  3, 12, 16, 2, true, "Python 3.12" },
    
    // Python 3.13
    { 3550,  3, 13, 16, 2, true, "Python 3.13" },
    { 3551,  3, 13, 16, 2, true, "Python 3.13" },
    { 3552,  3, 13, 16, 2, true, "Python 3.13" },
    { 3553,  3, 13, 16, 2, true, "Python 3.13" },
    { 3554,  3, 13, 16, 2, true, "Python 3.13" },
    { 3555,  3, 13, 16, 2, true, "Python 3.13" },
    { 3556,  3, 13, 16, 2, true, "Python 3.13" },
    { 3557,  3, 13, 16, 2, true, "Python 3.13" },
    { 3558,  3, 13, 16, 2, true, "Python 3.13" },
    { 3559,  3, 13, 16, 2, true, "Python 3.13" },
    { 3560,  3, 13, 16, 2, true, "Python 3.13" },
    { 3561,  3, 13, 16, 2, true, "Python 3.13" },
    { 3562,  3, 13, 16, 2, true, "Python 3.13" },
    { 3563,  3, 13, 16, 2, true, "Python 3.13" },
    { 3564,  3, 13, 16, 2, true, "Python 3.13" },
    { 3565,  3, 13, 16, 2, true, "Python 3.13" },
    { 3566,  3, 13, 16, 2, true, "Python 3.13" },
    { 3567,  3, 13, 16, 2, true, "Python 3.13" },
    { 3568,  3, 13, 16, 2, true, "Python 3.13" },
    { 3569,  3, 13, 16, 2, true, "Python 3.13" },
    { 3570,  3, 13, 16, 2, true, "Python 3.13" },
    
    // Python 3.14+
    { 3600,  3, 14, 16, 2, true, "Python 3.14" },
    { 3601,  3, 14, 16, 2, true, "Python 3.14" },
    { 3602,  3, 14, 16, 2, true, "Python 3.14" },
    { 3603,  3, 14, 16, 2, true, "Python 3.14" },
    { 3604,  3, 14, 16, 2, true, "Python 3.14" },
    { 3605,  3, 14, 16, 2, true, "Python 3.14" },
    { 3606,  3, 14, 16, 2, true, "Python 3.14" },
    { 3607,  3, 14, 16, 2, true, "Python 3.14" },
    { 3608,  3, 14, 16, 2, true, "Python 3.14" },
    { 3609,  3, 14, 16, 2, true, "Python 3.14" },
    { 3610,  3, 14, 16, 2, true, "Python 3.14" },
    { 3611,  3, 14, 16, 2, true, "Python 3.14" },
    { 3612,  3, 14, 16, 2, true, "Python 3.14" },
    { 3613,  3, 14, 16, 2, true, "Python 3.14" },
    { 3614,  3, 14, 16, 2, true, "Python 3.14" },
    { 3615,  3, 14, 16, 2, true, "Python 3.14" },
    { 3616,  3, 14, 16, 2, true, "Python 3.14" },
    { 3617,  3, 14, 16, 2, true, "Python 3.14" },
    { 3618,  3, 14, 16, 2, true, "Python 3.14" },
    { 3619,  3, 14, 16, 2, true, "Python 3.14" },
    { 3620,  3, 14, 16, 2, true, "Python 3.14" },
    { 3621,  3, 14, 16, 2, true, "Python 3.14" },
    { 3622,  3, 14, 16, 2, true, "Python 3.14" },
    { 3623,  3, 14, 16, 2, true, "Python 3.14" },
    { 3624,  3, 14, 16, 2, true, "Python 3.14" },
    { 3625,  3, 14, 16, 2, true, "Python 3.14" },
    { 3626,  3, 14, 16, 2, true, "Python 3.14" },
    { 3627,  3, 14, 16, 2, true, "Python 3.14" },
    { 3628,  3, 14, 16, 2, true, "Python 3.14" },
    { 3629,  3, 14, 16, 2, true, "Python 3.14" },
    { 3630,  3, 14, 16, 2, true, "Python 3.14" },
    
    // Sentinel
    { 0, 0, 0, 0, 0, false, nullptr }
};

// Calculate table size (excluding sentinel)
const size_t magic_table_size = (sizeof(magic_table) / sizeof(magic_table[0])) - 1;

// Lookup magic number in table
const magic_entry* lookup_magic(uint16_t magic) {
    for (const auto* entry = magic_table; entry->name != nullptr; ++entry) {
        if (entry->magic == magic)
            return entry;
    }
    return nullptr;
}

// Default version for raw marshal files (Python 3.13)
static const magic_entry default_version = {
    0, 3, 13, 0, 2, true, "Python 3.13 (Raw Marshal)"
};

const magic_entry* get_default_version() {
    return &default_version;
}

} // namespace version
} // namespace pyc
