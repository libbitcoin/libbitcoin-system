#include "data_type.hpp"

namespace libbitcoin {

// readable_data_type

void readable_data_type::set(uint32_t value)
{
    data_buffer_ = uncast_type(value);
    prepare();
}

void readable_data_type::set(const data_chunk& data)
{
    extend_data(data_buffer_, data);
    prepare();
}

void readable_data_type::set(const hash_digest& data)
{
    extend_data(data_buffer_, data);
    prepare();
}

void readable_data_type::set(const std::string& data)
{
    extend_data(data_buffer_, data);
    prepare();
}

Dbt* readable_data_type::get()
{
    return &dbt_;
}
const Dbt* readable_data_type::get() const
{
    return &dbt_;
}

void readable_data_type::prepare()
{
    dbt_.set_data(&data_buffer_[0]);
    dbt_.set_ulen(data_buffer_.size());
    dbt_.set_size(data_buffer_.size());
    dbt_.set_flags(DB_DBT_USERMEM);
}

// writable_data_type

writable_data_type::writable_data_type()
{
    dbt_.set_flags(DB_DBT_MALLOC);
}
writable_data_type::~writable_data_type()
{
    free(dbt_.get_data());
}

data_chunk writable_data_type::data() const
{
    std::string raw_depth(reinterpret_cast<const char*>(
        dbt_.get_data()), dbt_.get_size());
    return data_chunk(raw_depth.begin(), raw_depth.end());
}

bool writable_data_type::empty()
{
    return dbt_.get_data() == nullptr;
}

Dbt* writable_data_type::get()
{
    return &dbt_;
}
const Dbt* writable_data_type::get() const
{
    return &dbt_;
}

// empty_data_type

empty_data_type::empty_data_type()
{
    dbt_.set_flags(DB_DBT_MALLOC|DB_DBT_PARTIAL);
    dbt_.set_doff(0);
    dbt_.set_dlen(0);
}
empty_data_type::~empty_data_type()
{
    free(dbt_.get_data());
}

Dbt* empty_data_type::get()
{
    return &dbt_;
}
const Dbt* empty_data_type::get() const
{
    return &dbt_;
}

} // libbitcoin

