#ifndef LIBBITCOIN_SHA256_H
#define LIBBITCOIN_SHA256_H
namespace libbitcoin {

class SHA256
{
private:
    bool finalized = false;
public:
    SHA256();
    update(const void *data, unsigned long len);
    update(std::string data);
    
    std::string final();
};

}
#endif
