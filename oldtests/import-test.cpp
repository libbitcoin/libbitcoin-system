#include <boost/lexical_cast.hpp>
#include <bitcoin/bitcoin.hpp>
using namespace bc;

using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;

void blockchain_started(const std::error_code& ec);
void resume_copy(const std::error_code& ec, size_t last_depth,
    blockchain* chain_1, blockchain* chain_2);
void copy_block(const std::error_code& ec, const block_type& blk,
    size_t depth, blockchain* chain_1, blockchain* chain_2);
void handle_import(const std::error_code& ec,
    size_t depth, const hash_digest& hash,
    blockchain* chain_1, blockchain* chain_2);

void blockchain_started(const std::error_code& ec)
{
    if (ec)
        log_error() << "Blockchain init error: " << ec.message();
    else
        log_info() << "Blockchain initialized!";
}

size_t end_depth = 0;

void begin_import(const std::error_code& ec, size_t last_depth,
    blockchain* chain_1, blockchain* chain_2)
{
    if (ec)
    {
        log_error() << "Fetching stop depth: " << last_depth;
        return;
    }
    end_depth = last_depth;
    log_info() << "Stopping at block #" << end_depth;
    chain_2->fetch_last_depth(
        std::bind(resume_copy, _1, _2, chain_1, chain_2));
}

void resume_copy(const std::error_code& ec, size_t last_depth,
    blockchain* chain_1, blockchain* chain_2)
{
    size_t resume_depth = last_depth + 1;
    if (ec == error::not_found)
        resume_depth = 0;
    else if (ec)
    {
        log_error() << "Error fetch last depth from DEST: " << ec.message();
        return;
    }
    fetch_block(*chain_1, resume_depth,
        std::bind(copy_block, _1, _2, resume_depth, chain_1, chain_2));
}

void copy_block(const std::error_code& ec, const block_type& blk,
    size_t depth, blockchain* chain_1, blockchain* chain_2)
{
    if (ec)
        log_error() << "Fetch error: " << ec.message();
    log_info() << "Fetched block.";
    log_info() << "Importing...";
    chain_2->import(blk, depth,
        std::bind(&handle_import, _1,
            depth, hash_block_header(blk), chain_1, chain_2));
}

void handle_import(const std::error_code& ec,
    size_t depth, const hash_digest& hash,
    blockchain* chain_1, blockchain* chain_2)
{
    if (ec)
    {
        log_error() << "Import error: " << ec.message();
        return;
    }
    log_info() << "Imported block #" << depth << " " << hash;
    log_info() << "Fetching...";
    if (depth == end_depth)
    {
        log_info() << "Finished.";
        return;
    }
    fetch_block(*chain_1, depth + 1,
        std::bind(copy_block, _1, _2, depth + 1, chain_1, chain_2));
}

void output_to_file(std::ofstream& file, log_level level,
    const std::string& domain, const std::string& body)
{
    if (body.empty())
        return;
    file << level_repr(level);
    if (!domain.empty())
        file << " [" << domain << "]";
    file << ": " << body << std::endl;
}
void output_cerr_and_file(std::ofstream& file, log_level level,
    const std::string& domain, const std::string& body)
{
    if (body.empty())
        return;
    std::ostringstream output;
    output << level_repr(level);
    if (!domain.empty())
        output << " [" << domain << "]";
    output << ": " << body;
    std::cerr << output.str() << std::endl;
}

int main(int argc, char** argv)
{
    std::ofstream outfile("debug.log"), errfile("error.log");
    log_debug().set_output_function(
        std::bind(output_to_file, std::ref(outfile), _1, _2, _3));
    log_info().set_output_function(
        std::bind(output_to_file, std::ref(outfile), _1, _2, _3));
    log_warning().set_output_function(
        std::bind(output_to_file, std::ref(errfile), _1, _2, _3));
    log_error().set_output_function(
        std::bind(output_cerr_and_file, std::ref(errfile), _1, _2, _3));
    log_fatal().set_output_function(
        std::bind(output_cerr_and_file, std::ref(errfile), _1, _2, _3));

    if (argc != 3)
    {
        log_info() << "Usage: import SOURCE DEST";
        return 1;
    }
    threadpool pool(1);
    bdb_blockchain chain_1(pool);
    leveldb_blockchain chain_2(pool);
    chain_1.start(argv[1], blockchain_started);
    chain_2.start(argv[2], blockchain_started);
    chain_1.fetch_last_depth(
        std::bind(begin_import, _1, _2, &chain_1, &chain_2));
    std::cin.get();
    log_info() << "Exiting...";
    pool.stop();
    pool.join();
    chain_1.stop();
    chain_2.stop();
    return 0;
}

