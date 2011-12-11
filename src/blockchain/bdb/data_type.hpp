#ifndef LIBBITCOIN_STORAGE_BERKELEYDB_DATA_TYPE_STORAGE_H
#define LIBBITCOIN_STORAGE_BERKELEYDB_DATA_TYPE_STORAGE_H

#include DB_CXX_HEADER

#include <bitcoin/types.hpp>

namespace libbitcoin {

class data_type
{
public:
    void set(uint32_t value)
    {
        data_buffer_ = uncast_type(value);
        prepare();
    }

    void set(const data_chunk& data)
    {
        extend_data(data_buffer_, data);
        prepare();
    }

    void set(const hash_digest& data)
    {
        extend_data(data_buffer_, data);
        prepare();
    }

    void set(const std::string& data)
    {
        extend_data(data_buffer_, data);
        prepare();
    }

    Dbt* get()
    {
        return &dbt_;
    }
private:
    void prepare()
    {
        dbt_.set_data(&data_buffer_[0]);
        dbt_.set_size(data_buffer_.size());
        dbt_.set_flags(DB_DBT_USERMEM);
    }

    data_chunk data_buffer_;
    Dbt dbt_;
};

typedef std::shared_ptr<data_type> data_type_ptr;

} // libbitcoin

#endif

