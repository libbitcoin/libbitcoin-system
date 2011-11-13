#ifndef LIBBITCOIN_STORAGE_BERKELEYDB_DETAIL_H
#define LIBBITCOIN_STORAGE_BERKELEYDB_DETAIL_H

template<typename DbType>
class bdb_guard
{
public:
    bdb_guard()
     : object_(nullptr)
    {
    }
    ~bdb_guard()
    {
        if (object_ != nullptr)
            object_->close(0);
    }

    bdb_guard(const bdb_guard&) = delete;
    void operator=(const bdb_guard&) = delete;

    void set(DbType* object)
    {
        object_ = object;
    }

    DbType* operator->() const
    {
        return object_;
    }
    DbType* get() const
    {
        return object_;
    }
private:
    DbType* object_;
};

#endif

