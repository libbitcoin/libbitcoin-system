Unsafe endian assumption in crc32c.h:

// This assumes little-endian integers, as is the case on Intel processors that
// the assembler code here is for.
static uint32_t append_adler_table(uint32_t crci, buffer input, size_t length)
