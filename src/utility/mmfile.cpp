#include <bitcoin/utility/mmfile.hpp>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>

namespace libbitcoin {

mmfile::mmfile(const std::string& filename)
{
    file_handle_ = open64(filename.c_str(),
        O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (file_handle_ == -1)
        return;
    struct stat64 sbuf;
    if (fstat64(file_handle_, &sbuf) == -1)
        return;
    size_ = (long long)sbuf.st_size;
    // You can static_cast void* pointers.
    data_ = static_cast<uint8_t*>(mmap64(
        0, size_, PROT_READ | PROT_WRITE, MAP_SHARED, file_handle_, 0));
    if (data_ == MAP_FAILED)
        data_ = nullptr;
    // mmap opens its own file handle.
    close(file_handle_);
}
mmfile::~mmfile()
{
}

uint8_t* mmfile::data()
{
    return data_;
}
const uint8_t* mmfile::data() const
{
    return data_;
}
size_t mmfile::size() const
{
    return size_;
}

} // namespace libbitcoin

