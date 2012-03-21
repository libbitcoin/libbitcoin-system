#ifndef LIBBITCOIN_STORAGE_BERKELEYDB_DATA_TYPE_STORAGE_H
#define LIBBITCOIN_STORAGE_BERKELEYDB_DATA_TYPE_STORAGE_H

#include DB_CXX_HEADER

#include <bitcoin/types.hpp>

namespace libbitcoin {

class readable_data_type
{
public:
    void set(uint32_t value);
    void set(const data_chunk& data);
    void set(const hash_digest& data);
    void set(const short_hash& data);
    void set(const std::string& data);

    Dbt* get();
    const Dbt* get() const;

private:
    void prepare();

    data_chunk data_buffer_;
    Dbt dbt_;
};

class writable_data_type
{
public:
    writable_data_type();
    ~writable_data_type();

    data_chunk data() const;

    bool empty();

    Dbt* get();
    const Dbt* get() const;

private:
    Dbt dbt_;
};

typedef std::shared_ptr<writable_data_type> writable_data_type_ptr;

class empty_data_type
{
public:
    empty_data_type();
    ~empty_data_type();

    Dbt* get();
    const Dbt* get() const;

private:
    Dbt dbt_;
};

} // namespace libbitcoin

#endif

