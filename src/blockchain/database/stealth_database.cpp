#include <bitcoin/blockchain/database/stealth_database.hpp>

#ifndef _MSC_VER
#include <sys/mman.h>
#endif
#include <bitcoin/utility/assert.hpp>
#include <bitcoin/utility/serializer.hpp>

namespace libbitcoin {

constexpr uint32_t metadata_size = 12;
// We save the entry offset every 100 blocks.
constexpr uint32_t block_page_interval = 100;
constexpr uint32_t entry_row_size = 4 + 33 + 21 + 32;

stealth_database::stealth_database(mmfile& file)
  : file_(file)
{
    BITCOIN_ASSERT(file_.data() != nullptr);
    // metadata:
    //   [ version:4 ]
    //   [ max_header_rows:4 ]
    //   [ entries_count:4 ]
    // header:
    //   [ ...               ]
    //   [ [ entry_index:4 ] ]
    //   [ ...               ]
    // entries:
    //   [ ...                                                            ]
    //   [ [ prefix_bitfield:4 ][ ephemkey:33 ][ address:21 ][ tx_id:32 ] ]
    //   [ ...                                                            ]
    BITCOIN_ASSERT(file_.size() > metadata_size);
    // Read off metadata.
    auto deserial = make_deserializer(
        file_.data(), file_.data() + metadata_size);
    version_ = deserial.read_4_bytes();
    max_header_rows_ = deserial.read_4_bytes();
    entries_count_ = deserial.read_4_bytes();
    BITCOIN_ASSERT(version_ == 1);
    // Calculate sector offsets.
    header_sector_ = metadata_size;
    entries_sector_ = header_sector_ + max_header_rows_ * 4;
    reset();
}

void stealth_database::store(write_function write)
{
    uint64_t next_offset = calculate_entry_offset(
        entries_count_ + entries_written_count_);
    uint8_t* iter = file_.data() + next_offset;
    write(iter);
    ++entries_written_count_;
}

void stealth_database::sync(uint32_t block_height)
{
    // Add start index for entries at a block_height.
    // This way we can skip entries and start reading.
    if (block_height % block_page_interval == 0)
        add_header_entry_index(block_height);
    modify_entries_count();
    // Ready for next batch of writes.
    reset();
}
void stealth_database::add_header_entry_index(uint32_t block_height)
{
    uint32_t interval = block_height / block_page_interval;
    uint64_t offset = header_sector_ + interval * 4;
    uint8_t* iter = file_.data() + offset;
    auto serial = make_serializer(iter);
    serial.write_4_bytes(entries_count_);
}
void stealth_database::modify_entries_count()
{
    entries_count_ += entries_written_count_;
    uint8_t* item = file_.data() + 8;
    auto serial = make_serializer(item);
    serial.write_4_bytes(entries_count_);
}
void stealth_database::reset()
{
    entries_written_count_ = 0;
    advise_kernel();
}

void stealth_database::scan(read_function read, uint32_t from_height)
{
    uint32_t start_index = read_start_entry_index(from_height);
    for (uint32_t index = start_index; index < entries_count_; ++index)
        read_entry(index, read);
}
uint32_t stealth_database::read_start_entry_index(uint32_t from_height)
{
    uint32_t interval = from_height / block_page_interval;
    uint64_t offset = header_sector_ + interval * 4;
    uint8_t* iter = file_.data() + offset;
    auto deserial = make_deserializer(iter, iter + 4);
    return deserial.read_4_bytes();
}
void stealth_database::read_entry(uint32_t entry_index, read_function read)
{
    uint64_t offset = calculate_entry_offset(entry_index);
    BITCOIN_ASSERT(file_.size() >= offset + entry_row_size);
    uint8_t* iter = file_.data() + offset;
    read(iter);
}

uint64_t stealth_database::calculate_entry_offset(uint32_t index)
{
    return entries_sector_ + index * entry_row_size;
}
void stealth_database::advise_kernel()
{
    uint64_t start_offset = calculate_entry_offset(0);
    uint64_t end_offset = calculate_entry_offset(entries_count_);
    uint64_t entries_size = end_offset - start_offset;

#ifndef _MSC_VER
    // Not yet MSVC portable (maybe windows).
    madvise(file_.data() + start_offset, entries_size, MADV_SEQUENTIAL);
#endif
}

} // namespace libbitcoin