%module message
%{
#include <bitcoin/bitcoin.hpp>
%}


%import "../../../../include/bitcoin/bitcoin/compat.h"
%import "../../../../include/bitcoin/bitcoin/compat.hpp"
%import "../../../../include/bitcoin/bitcoin/define.hpp"

%import <bitcoin-ext.i>
%import <bitcoin-ext-typemap.i>
%import <bitcoin-chain-ext.i>
%import <bitcoin-chain-ext-typemap.i>
%import <bitcoin-utility-ext.i>
%import <bitcoin-utility-ext-typemap.i>

%ignore libbitcoin::message::address::address(network_address::list&& addresses);
%ignore libbitcoin::message::address::address(address&& other);

%ignore libbitcoin::message::address::addresses();
%ignore libbitcoin::message::address::set_addresses(network_address::list&& value);

%rename(assign) operator=(address&& other);
%ignore operator=(const address&);
%rename(eq) operator==(const address& other) const;
%rename(ne) operator!=(const address& other) const;
%ignore libbitcoin::message::address::factory(uint32_t version, std::istream& stream);
%ignore libbitcoin::message::address::factory(uint32_t version, reader& source);
%ignore libbitcoin::message::address::from_data(uint32_t version, std::istream& stream);
%ignore libbitcoin::message::address::from_data(uint32_t version, reader& source);
%ignore libbitcoin::message::address::to_data(uint32_t version, std::ostream& stream) const;
%ignore libbitcoin::message::address::to_data(uint32_t version, writer& sink) const;
%include "../include/bitcoin/bitcoin/message/address.hpp"

%ignore libbitcoin::message::alert::alert(data_chunk&& payload, data_chunk&& signature);
%ignore libbitcoin::message::alert::alert(alert&& other);

%ignore libbitcoin::message::alert::payload();
%ignore libbitcoin::message::alert::set_payload(data_chunk&& value);

%ignore libbitcoin::message::alert::signature();
%ignore libbitcoin::message::alert::set_signature(data_chunk&& value);

%rename(assign) operator=(alert&& other);
%ignore operator=(const alert&);
%rename(eq) operator==(const alert& other) const;
%rename(ne) operator!=(const alert& other) const;
%ignore libbitcoin::message::alert::factory(uint32_t version, std::istream& stream);
%ignore libbitcoin::message::alert::factory(uint32_t version, reader& source);
%ignore libbitcoin::message::alert::from_data(uint32_t version, std::istream& stream);
%ignore libbitcoin::message::alert::from_data(uint32_t version, reader& source);
%ignore libbitcoin::message::alert::to_data(uint32_t version, std::ostream& stream) const;
%ignore libbitcoin::message::alert::to_data(uint32_t version, writer& sink) const;
%include "../include/bitcoin/bitcoin/message/alert.hpp"

%ignore libbitcoin::message::alert_payload::alert_payload(uint32_t version, uint64_t relay_until, uint64_t expiration,
        uint32_t id, uint32_t cancel, std::vector<uint32_t>&& set_cancel,
        uint32_t min_version, uint32_t max_version,
        std::vector<std::string>&& set_sub_version, uint32_t priority,
        std::string&& comment, std::string&& status_bar,
        std::string&& reserved);
%ignore libbitcoin::message::alert_payload::alert_payload(alert_payload&& other);
%ignore libbitcoin::message::alert_payload::set_cancel();
%ignore libbitcoin::message::alert_payload::set_set_cancel(std::vector<uint32_t>&& value);

%ignore libbitcoin::message::alert_payload::set_sub_version();
%ignore libbitcoin::message::alert_payload::set_set_sub_version(std::vector<std::string>&& value);
%ignore libbitcoin::message::alert_payload::comment();
%ignore libbitcoin::message::alert_payload::set_comment(std::string&& value);
%ignore libbitcoin::message::alert_payload::status_bar();
%ignore libbitcoin::message::alert_payload::set_status_bar(std::string&& value);
%ignore libbitcoin::message::alert_payload::reserved();
%ignore libbitcoin::message::alert_payload::set_reserved(std::string&& value);
    
%rename(assign) operator=(alert_payload&& other);
%ignore operator=(const alert_payload&);
%rename(eq) operator==(const alert_payload& other) const;
%rename(ne) operator!=(const alert_payload& other) const;
%ignore libbitcoin::message::alert_payload::factory(uint32_t version, std::istream& stream);
%ignore libbitcoin::message::alert_payload::factory(uint32_t version, reader& source);
%ignore libbitcoin::message::alert_payload::from_data(uint32_t version, std::istream& stream);
%ignore libbitcoin::message::alert_payload::from_data(uint32_t version, reader& source);
%ignore libbitcoin::message::alert_payload::to_data(uint32_t version, std::ostream& stream) const;
%ignore libbitcoin::message::alert_payload::to_data(uint32_t version, writer& sink) const;
%include "../include/bitcoin/bitcoin/message/alert_payload.hpp"

%ignore libbitcoin::message::block::block(block&& other);
%ignore libbitcoin::message::block::block(chain::block&& other);
%ignore libbitcoin::message::block::block(chain::header&& header, chain::transaction::list&& transactions);

%ignore libbitcoin::message::block::factory(uint32_t version, std::istream& stream);
%ignore libbitcoin::message::block::factory(uint32_t version, reader& source);
%ignore libbitcoin::message::block::from_data(uint32_t version, std::istream& stream);
%ignore libbitcoin::message::block::from_data(uint32_t version, reader& source);
%ignore libbitcoin::message::block::to_data(uint32_t version, std::ostream& stream) const;
%ignore libbitcoin::message::block::to_data(uint32_t version, writer& sink) const;

%rename(assign) operator=(chain::block&& other);
%rename(assign) operator=(block&& other);
%ignore operator=(const block&);

%rename(eq) operator==(const chain::block& other) const;
%rename(ne) operator!=(const chain::block& other) const;

%rename(eq) operator==(const block& other) const;
%rename(ne) operator!=(const block& other) const;

%include "../include/bitcoin/bitcoin/message/block.hpp"

%ignore libbitcoin::message::block_transactions::block_transactions(hash_digest&& block_hash, chain::transaction::list&& transactions);
%ignore libbitcoin::message::block_transactions::block_transactions(block_transactions&& other);

%ignore libbitcoin::message::block_transactions::block_hash();
%ignore libbitcoin::message::block_transactions::set_block_hash(hash_digest&& value);

%ignore libbitcoin::message::block_transactions::transactions();
%ignore libbitcoin::message::block_transactions::set_transactions(chain::transaction::list&& other);

%ignore libbitcoin::message::block_transactions::factory(uint32_t version, std::istream& stream);
%ignore libbitcoin::message::block_transactions::factory(uint32_t version, reader& source);
%ignore libbitcoin::message::block_transactions::from_data(uint32_t version, std::istream& stream);
%ignore libbitcoin::message::block_transactions::from_data(uint32_t version, reader& source);
%ignore libbitcoin::message::block_transactions::to_data(uint32_t version, std::ostream& stream) const;
%ignore libbitcoin::message::block_transactions::to_data(uint32_t version, writer& sink) const;
%rename(assign) operator=(block_transactions&& other);
%ignore operator=(const block_transactions&);

%rename(eq) operator==(const block_transactions& other) const;
%rename(ne) operator!=(const block_transactions& other) const;
%include "../include/bitcoin/bitcoin/message/block_transactions.hpp"

%ignore libbitcoin::message::compact_block::compact_block(chain::header&& header, uint64_t nonce, short_id_list&& short_ids, prefilled_transaction::list&& transactions);
%ignore libbitcoin::message::compact_block::compact_block(compact_block&& other);

%ignore libbitcoin::message::compact_block::header();
%ignore libbitcoin::message::compact_block::set_header(chain::header&& value);
%ignore libbitcoin::message::compact_block::short_ids();
%ignore libbitcoin::message::compact_block::set_short_ids(short_id_list&& value);
%ignore libbitcoin::message::compact_block::transactions();
%ignore libbitcoin::message::compact_block::set_transactions(prefilled_transaction::list&& value);
    
%ignore libbitcoin::message::compact_block::factory(uint32_t version, std::istream& stream);
%ignore libbitcoin::message::compact_block::factory(uint32_t version, reader& source);
%ignore libbitcoin::message::compact_block::from_data(uint32_t version, std::istream& stream);
%ignore libbitcoin::message::compact_block::from_data(uint32_t version, reader& source);
%ignore libbitcoin::message::compact_block::to_data(uint32_t version, std::ostream& stream) const;
%ignore libbitcoin::message::compact_block::to_data(uint32_t version, writer& sink) const;
%rename(assign) operator=(compact_block&& other);
%ignore operator=(const compact_block&);

%rename(eq) operator==(const compact_block& other) const;
%rename(ne) operator!=(const compact_block& other) const;

%include "../include/bitcoin/bitcoin/message/compact_block.hpp"

%ignore libbitcoin::message::fee_filter::fee_filter(uint64_t minimum);
%ignore libbitcoin::message::fee_filter::fee_filter(fee_filter&& other);

%ignore libbitcoin::message::fee_filter::factory(uint32_t version, std::istream& stream);
%ignore libbitcoin::message::fee_filter::factory(uint32_t version, reader& source);
%ignore libbitcoin::message::fee_filter::from_data(uint32_t version, std::istream& stream);
%ignore libbitcoin::message::fee_filter::from_data(uint32_t version, reader& source);
%ignore libbitcoin::message::fee_filter::to_data(uint32_t version, std::ostream& stream) const;
%ignore libbitcoin::message::fee_filter::to_data(uint32_t version, writer& sink) const;
%rename(assign) operator=(fee_filter&& other);
%ignore operator=(const fee_filter&);

%rename(eq) operator==(const fee_filter& other) const;
%rename(ne) operator!=(const fee_filter& other) const;
%include "../include/bitcoin/bitcoin/message/fee_filter.hpp"

%ignore libbitcoin::message::filter_add::filter_add(data_chunk&& data);
%ignore libbitcoin::message::filter_add::filter_add(filter_add&& other);
%ignore libbitcoin::message::filter_add::data();
%ignore libbitcoin::message::filter_add::set_data(data_chunk&& value);

%ignore libbitcoin::message::filter_add::factory(uint32_t version, std::istream& stream);
%ignore libbitcoin::message::filter_add::factory(uint32_t version, reader& source);

%ignore libbitcoin::message::filter_add::from_data(uint32_t version, std::istream& stream);
%ignore libbitcoin::message::filter_add::from_data(uint32_t version, reader& source);
%ignore libbitcoin::message::filter_add::to_data(uint32_t version, std::ostream& stream) const;
%ignore libbitcoin::message::filter_add::to_data(uint32_t version, writer& sink) const;
%rename(assign) operator=(filter_add&& other);
%ignore operator=(const filter_add&);

%rename(eq) operator==(const filter_add& other) const;
%rename(ne) operator!=(const filter_add& other) const;
%include "../include/bitcoin/bitcoin/message/filter_add.hpp"

%ignore libbitcoin::message::filter_clear::filter_clear(filter_clear&& other);

%ignore libbitcoin::message::filter_clear::factory(uint32_t version, std::istream& stream);
%ignore libbitcoin::message::filter_clear::factory(uint32_t version, reader& source);
%ignore libbitcoin::message::filter_clear::from_data(uint32_t version, std::istream& stream);
%ignore libbitcoin::message::filter_clear::from_data(uint32_t version, reader& source);
%ignore libbitcoin::message::filter_clear::to_data(uint32_t version, std::ostream& stream) const;
%ignore libbitcoin::message::filter_clear::to_data(uint32_t version, writer& sink) const;
%include "../include/bitcoin/bitcoin/message/filter_clear.hpp"

%ignore libbitcoin::message::filter_load::filter_load(data_chunk&& filter, uint32_t hash_functions, uint32_t tweak, uint8_t flags);
%ignore libbitcoin::message::filter_load::filter_load(filter_load&& other);
%ignore libbitcoin::message::filter_load::filter();
%ignore libbitcoin::message::filter_load::set_filter(data_chunk&& value);

%ignore libbitcoin::message::filter_load::factory(uint32_t version, std::istream& stream);
%ignore libbitcoin::message::filter_load::factory(uint32_t version, reader& source);
%ignore libbitcoin::message::filter_load::from_data(uint32_t version, std::istream& stream);
%ignore libbitcoin::message::filter_load::from_data(uint32_t version, reader& source);
%ignore libbitcoin::message::filter_load::to_data(uint32_t version, std::ostream& stream) const;
%ignore libbitcoin::message::filter_load::to_data(uint32_t version, writer& sink) const;
%rename(assign) operator=(filter_load&& other);
%ignore operator=(const filter_load&);

%rename(eq) operator==(const filter_load& other) const;
%rename(ne) operator!=(const filter_load& other) const;
%include "../include/bitcoin/bitcoin/message/filter_load.hpp"

%ignore libbitcoin::message::get_address::factory(uint32_t version, std::istream& stream);
%ignore libbitcoin::message::get_address::factory(uint32_t version, reader& source);
%ignore libbitcoin::message::get_address::from_data(uint32_t version, std::istream& stream);
%ignore libbitcoin::message::get_address::from_data(uint32_t version, reader& source);
%ignore libbitcoin::message::get_address::to_data(uint32_t version, std::ostream& stream) const;
%ignore libbitcoin::message::get_address::to_data(uint32_t version, writer& sink) const;
%include "../include/bitcoin/bitcoin/message/get_address.hpp"

%ignore libbitcoin::message::get_block_transactions::get_block_transactions(hash_digest&& block_hash, std::vector<uint64_t>&& indexes);
%ignore libbitcoin::message::get_block_transactions::get_block_transactions(get_block_transactions&& other);
        
%ignore libbitcoin::message::get_block_transactions::block_hash();
%ignore libbitcoin::message::get_block_transactions::set_block_hash(hash_digest&& value);
%ignore libbitcoin::message::get_block_transactions::indexes();
%ignore libbitcoin::message::get_block_transactions::set_indexes(std::vector<uint64_t>&& values);

%ignore libbitcoin::message::get_block_transactions::factory(uint32_t version, std::istream& stream);
%ignore libbitcoin::message::get_block_transactions::factory(uint32_t version, reader& source);
%ignore libbitcoin::message::get_block_transactions::from_data(uint32_t version, std::istream& stream);
%ignore libbitcoin::message::get_block_transactions::from_data(uint32_t version, reader& source);
%ignore libbitcoin::message::get_block_transactions::to_data(uint32_t version, std::ostream& stream) const;
%ignore libbitcoin::message::get_block_transactions::to_data(uint32_t version, writer& sink) const;
%rename(assign) operator=(get_block_transactions&& other);
%ignore operator=(const get_block_transactions&);

%rename(eq) operator==(const get_block_transactions& other) const;
%rename(ne) operator!=(const get_block_transactions& other) const;

%include "../include/bitcoin/bitcoin/message/get_block_transactions.hpp"

%ignore libbitcoin::message::get_blocks::get_blocks(hash_list&& start, hash_digest&& stop);
%ignore libbitcoin::message::get_blocks::get_blocks(get_blocks&& other);
%ignore libbitcoin::message::get_blocks::start_hashes();
%ignore libbitcoin::message::get_blocks::set_start_hashes(hash_list&& value);
%ignore libbitcoin::message::get_blocks::stop_hash();
%ignore libbitcoin::message::get_blocks::set_stop_hash(hash_digest&& value);

%ignore libbitcoin::message::get_blocks::factory(uint32_t version, std::istream& stream);
%ignore libbitcoin::message::get_blocks::factory(uint32_t version, reader& source);
%ignore libbitcoin::message::get_blocks::from_data(uint32_t version, std::istream& stream);
%ignore libbitcoin::message::get_blocks::from_data(uint32_t version, reader& source);
%ignore libbitcoin::message::get_blocks::to_data(uint32_t version, std::ostream& stream) const;
%ignore libbitcoin::message::get_blocks::to_data(uint32_t version, writer& sink) const;
%rename(assign) operator=(get_blocks&& other);
%ignore operator=(const get_blocks&);

%rename(eq) operator==(const get_blocks& other) const;
%rename(ne) operator!=(const get_blocks& other) const;

%include "../include/bitcoin/bitcoin/message/get_blocks.hpp"

%ignore libbitcoin::message::inventory::inventory(inventory_vector::list&& values);
%ignore libbitcoin::message::inventory::inventory(inventory&& other);
%ignore libbitcoin::message::inventory::inventories();
%ignore libbitcoin::message::inventory::set_inventories(inventory_vector::list&& value);

%ignore libbitcoin::message::inventory::factory(uint32_t version, std::istream& stream);
%ignore libbitcoin::message::inventory::factory(uint32_t version, reader& source);
%ignore libbitcoin::message::inventory::from_data(uint32_t version, std::istream& stream);
%ignore libbitcoin::message::inventory::from_data(uint32_t version, reader& source);
%ignore libbitcoin::message::inventory::to_data(uint32_t version, std::ostream& stream) const;
%ignore libbitcoin::message::inventory::to_data(uint32_t version, writer& sink) const;
%rename(assign) operator=(inventory&& other);
%ignore operator=(const inventory&);

%rename(eq) operator==(const inventory& other) const;
%rename(ne) operator!=(const inventory& other) const;
%include "../include/bitcoin/bitcoin/message/inventory.hpp"

%ignore libbitcoin::message::get_data::get_data(inventory_vector::list&& list);
%ignore libbitcoin::message::get_data::get_data(get_data&& other);

%ignore libbitcoin::message::get_data::factory(uint32_t version, std::istream& stream);
%ignore libbitcoin::message::get_data::factory(uint32_t version, reader& source);
%ignore libbitcoin::message::get_data::from_data(uint32_t version, std::istream& stream) override;
%ignore libbitcoin::message::get_data::from_data(uint32_t version, reader& source) override;
%rename(assign) operator=(get_data&& other);
%ignore operator=(const get_data&);

%rename(eq) operator==(const get_data& other) const;
%rename(ne) operator!=(const get_data& other) const;

%include "../include/bitcoin/bitcoin/message/get_data.hpp"

%ignore libbitcoin::message::get_headers::get_headers(hash_list&& start, hash_digest&& stop);
%ignore libbitcoin::message::get_headers::get_headers(get_headers&& other);

%ignore libbitcoin::message::get_headers::factory(uint32_t version, std::istream& stream);
%ignore libbitcoin::message::get_headers::factory(uint32_t version, reader& source);
%ignore libbitcoin::message::get_headers::from_data(uint32_t version, std::istream& stream) override;
%ignore libbitcoin::message::get_headers::from_data(uint32_t version, reader& source) override;

%rename(assign) operator=(get_headers&& other);
%ignore operator=(const get_headers&);

%rename(eq) operator==(const get_headers& other) const;
%rename(ne) operator!=(const get_headers& other) const;
%include "../include/bitcoin/bitcoin/message/get_headers.hpp"

%ignore libbitcoin::message::header::header(chain::header&& other);
%ignore libbitcoin::message::header::header(header&& other);

%ignore libbitcoin::message::header::factory(uint32_t version, std::istream& stream);
%ignore libbitcoin::message::header::factory(uint32_t version, reader& source);
%ignore libbitcoin::message::header::from_data(uint32_t version, std::istream& stream);
%ignore libbitcoin::message::header::from_data(uint32_t version, reader& source);
%ignore libbitcoin::message::header::to_data(uint32_t version, std::ostream& stream) const;
%ignore libbitcoin::message::header::to_data(uint32_t version, writer& sink) const;

%rename(assign) operator=(chain::header&& other);

%rename(assign) operator=(header&& other);
%ignore operator=(const header&) /*= delete*/;

%rename(eq) operator==(const chain::header& other) const;
%rename(ne) operator!=(const chain::header& other) const;

%rename(eq) operator==(const header& other) const;
%rename(ne) operator!=(const header& other) const;
%include "../include/bitcoin/bitcoin/message/header.hpp"

%ignore libbitcoin::message::headers::headers(header::list&& values);
%ignore libbitcoin::message::headers::headers(headers&& other);
%ignore libbitcoin::message::headers::elements();
%ignore libbitcoin::message::headers::set_elements(header::list&& values);

%ignore libbitcoin::message::headers::factory(uint32_t version, std::istream& stream);
%ignore libbitcoin::message::headers::factory(uint32_t version, reader& source);
%ignore libbitcoin::message::headers::from_data(uint32_t version, std::istream& stream);
%ignore libbitcoin::message::headers::from_data(uint32_t version, reader& source);
%ignore libbitcoin::message::headers::to_data(uint32_t version, std::ostream& stream) const;
%ignore libbitcoin::message::headers::to_data(uint32_t version, writer& sink) const;
%rename(assign) operator=(headers&& other);
%ignore operator=(const headers&);

%rename(eq) operator==(const headers& other) const;
%rename(ne) operator!=(const headers& other) const;
%include "../include/bitcoin/bitcoin/message/headers.hpp"

%ignore libbitcoin::message::heading::heading(heading&& other);
%ignore libbitcoin::message::heading::command();
%ignore libbitcoin::message::heading::set_command(std::string&& value);

%ignore libbitcoin::message::heading::factory(std::istream& stream);
%ignore libbitcoin::message::heading::factory(reader& source);
%ignore libbitcoin::message::heading::from_data(std::istream& stream);
%ignore libbitcoin::message::heading::from_data(reader& source);
%ignore libbitcoin::message::heading::to_data(std::ostream& stream) const;
%ignore libbitcoin::message::heading::to_data(writer& sink) const;
%rename(assign) operator=(heading&& other);
%ignore operator=(const heading&);

%rename(eq) operator==(const heading& other) const;
%rename(ne) operator!=(const heading& other) const;
%include "../include/bitcoin/bitcoin/message/heading.hpp"

%ignore libbitcoin::message::inventory_vector::inventory_vector(type_id type, hash_digest&& hash);
%ignore libbitcoin::message::inventory_vector::inventory_vector(inventory_vector&& other);
%ignore libbitcoin::message::inventory_vector::hash();
%ignore libbitcoin::message::inventory_vector::set_hash(hash_digest&& value);

%ignore libbitcoin::message::inventory_vector::factory(uint32_t version, std::istream& stream);
%ignore libbitcoin::message::inventory_vector::factory(uint32_t version, reader& source);
%ignore libbitcoin::message::inventory_vector::from_data(uint32_t version, std::istream& stream);
%ignore libbitcoin::message::inventory_vector::from_data(uint32_t version, reader& source);
%ignore libbitcoin::message::inventory_vector::to_data(uint32_t version, std::ostream& stream) const;
%ignore libbitcoin::message::inventory_vector::to_data(uint32_t version, writer& sink) const;
%rename(assign) operator=(inventory_vector&& other);
%ignore operator=(const inventory_vector& other);

%rename(eq) operator==(const inventory_vector& other) const;
%rename(ne) operator!=(const inventory_vector& other) const;
%include "../include/bitcoin/bitcoin/message/inventory_vector.hpp"

%ignore libbitcoin::message::memory_pool::memory_pool(memory_pool&& other);

%ignore libbitcoin::message::memory_pool::factory(uint32_t version, std::istream& stream);
%ignore libbitcoin::message::memory_pool::factory(uint32_t version, reader& source);
%ignore libbitcoin::message::memory_pool::from_data(uint32_t version, std::istream& stream);
%ignore libbitcoin::message::memory_pool::from_data(uint32_t version, reader& source);
%ignore libbitcoin::message::memory_pool::to_data(uint32_t version, std::ostream& stream) const;
%ignore libbitcoin::message::memory_pool::to_data(uint32_t version, writer& sink) const;

%include "../include/bitcoin/bitcoin/message/memory_pool.hpp"

%ignore libbitcoin::message::merkle_block::merkle_block(chain::header&& header, size_t total_transactions, hash_list&& hashes, data_chunk&& flags);
%ignore libbitcoin::message::merkle_block::merkle_block(merkle_block&& other);
%ignore libbitcoin::message::merkle_block::header();
%ignore libbitcoin::message::merkle_block::set_header(chain::header&& value);
%ignore libbitcoin::message::merkle_block::hashes();
%ignore libbitcoin::message::merkle_block::set_hashes(hash_list&& value);
%ignore libbitcoin::message::merkle_block::flags();
%ignore libbitcoin::message::merkle_block::set_flags(data_chunk&& value);
    
%ignore libbitcoin::message::merkle_block::factory(uint32_t version, std::istream& stream);
%ignore libbitcoin::message::merkle_block::factory(uint32_t version, reader& source);
%ignore libbitcoin::message::merkle_block::from_data(uint32_t version, std::istream& stream);
%ignore libbitcoin::message::merkle_block::from_data(uint32_t version, reader& source);
%ignore libbitcoin::message::merkle_block::to_data(uint32_t version, std::ostream& stream) const;
%ignore libbitcoin::message::merkle_block::to_data(uint32_t version, writer& sink) const;
%rename(assign) operator=(merkle_block&& other);
%ignore operator=(const merkle_block&);

%rename(eq) operator==(const merkle_block& other) const;
%rename(ne) operator!=(const merkle_block& other) const;

%include "../include/bitcoin/bitcoin/message/merkle_block.hpp"
%include "../include/bitcoin/bitcoin/message/messages.hpp"

%ignore libbitcoin::message::network_address::network_address(uint32_t timestamp, uint64_t services, ip_address&& ip, uint16_t port);
%ignore libbitcoin::message::network_address::network_address(network_address&& other);
%ignore libbitcoin::message::network_address::ip();
%ignore libbitcoin::message::network_address::set_ip(ip_address&& value);
%ignore libbitcoin::message::network_address::factory(uint32_t version, std::istream& stream,         bool with_timestamp);
%ignore libbitcoin::message::network_address::factory(uint32_t version, reader& source,        bool with_timestamp);
%ignore libbitcoin::message::network_address::from_data(uint32_t version, std::istream& stream,        bool with_timestamp);
%ignore libbitcoin::message::network_address::from_data(uint32_t version, reader& source, bool with_timestamp);
%ignore libbitcoin::message::network_address::to_data(uint32_t version, std::ostream& stream,        bool with_timestamp) const;
%ignore libbitcoin::message::network_address::to_data(uint32_t version, writer& sink, bool with_timestamp) const;
%rename(assign) operator=(network_address&& other);
%ignore operator=(const network_address& other);

%rename(eq) operator==(const network_address& other) const;
%rename(ne) operator!=(const network_address& other) const;

%include "../include/bitcoin/bitcoin/message/network_address.hpp"

%ignore libbitcoin::message::not_found::not_found(inventory_vector::list&& values);
%ignore libbitcoin::message::not_found::not_found(not_found&& other);
%ignore libbitcoin::message::not_found::factory(uint32_t version, std::istream& stream);
%ignore libbitcoin::message::not_found::factory(uint32_t version, reader& source);
%ignore libbitcoin::message::not_found::from_data(uint32_t version, std::istream& stream) override;
%ignore libbitcoin::message::not_found::from_data(uint32_t version, reader& source) override;

%rename(assign) operator=(not_found&& other);
%ignore operator=(const not_found&);

%rename(eq) operator==(const not_found& other) const;
%rename(ne) operator!=(const not_found& other) const;

%include "../include/bitcoin/bitcoin/message/not_found.hpp"

%ignore libbitcoin::message::ping::factory(uint32_t version, std::istream& stream);
%ignore libbitcoin::message::ping::factory(uint32_t version, reader& source);
%ignore libbitcoin::message::ping::from_data(uint32_t version, std::istream& stream);
%ignore libbitcoin::message::ping::from_data(uint32_t version, reader& source);

%ignore libbitcoin::message::ping::to_data(uint32_t version, std::ostream& stream) const;
%ignore libbitcoin::message::ping::to_data(uint32_t version, writer& sink) const;
%rename(assign) operator=(ping&& other);
%ignore operator=(const ping&);

%rename(eq) operator==(const ping& other) const;
%rename(ne) operator!=(const ping& other) const;

%include "../include/bitcoin/bitcoin/message/ping.hpp"

%ignore libbitcoin::message::pong::factory(uint32_t version, std::istream& stream);
%ignore libbitcoin::message::pong::factory(uint32_t version, reader& source);
%ignore libbitcoin::message::pong::from_data(uint32_t version, std::istream& stream);
%ignore libbitcoin::message::pong::from_data(uint32_t version, reader& source);

%ignore libbitcoin::message::pong::to_data(uint32_t version, std::ostream& stream) const;
%ignore libbitcoin::message::pong::to_data(uint32_t version, writer& sink) const;
%rename(assign) operator=(pong&& other);
%ignore operator=(const pong&);

%rename(eq) operator==(const pong& other) const;
%rename(ne) operator!=(const pong& other) const;
%include "../include/bitcoin/bitcoin/message/pong.hpp"

%ignore libbitcoin::message::prefilled_transaction::prefilled_transaction(uint64_t index, chain::transaction&& tx);
%ignore libbitcoin::message::prefilled_transaction::prefilled_transaction(prefilled_transaction&& other);
%ignore libbitcoin::message::prefilled_transaction::transaction();
%ignore libbitcoin::message::prefilled_transaction::set_transaction(chain::transaction&& tx);

%ignore libbitcoin::message::prefilled_transaction::factory(uint32_t version, std::istream& stream);
%ignore libbitcoin::message::prefilled_transaction::factory(uint32_t version, reader& source);
%ignore libbitcoin::message::prefilled_transaction::from_data(uint32_t version, std::istream& stream);
%ignore libbitcoin::message::prefilled_transaction::from_data(uint32_t version, reader& source);
%ignore libbitcoin::message::prefilled_transaction::to_data(uint32_t version, std::ostream& stream) const;
%ignore libbitcoin::message::prefilled_transaction::to_data(uint32_t version, writer& sink) const;

%rename(assign) operator=(prefilled_transaction&& other);
%ignore operator=(const prefilled_transaction& other);

%rename(eq) operator==(const prefilled_transaction& other) const;
%rename(ne) operator!=(const prefilled_transaction& other) const;
%include "../include/bitcoin/bitcoin/message/prefilled_transaction.hpp"

%ignore libbitcoin::message::reject::reject(reject&& other);
%ignore libbitcoin::message::reject::message();
%ignore libbitcoin::message::reject::reason();
%ignore libbitcoin::message::reject::data();
%ignore libbitcoin::message::reject::set_data(hash_digest&& value);
%ignore libbitcoin::message::reject::factory(uint32_t version, std::istream& stream);
%ignore libbitcoin::message::reject::factory(uint32_t version, reader& source);
%ignore libbitcoin::message::reject::from_data(uint32_t version, std::istream& stream);
%ignore libbitcoin::message::reject::from_data(uint32_t version, reader& source);
%ignore libbitcoin::message::reject::to_data(uint32_t version, std::ostream& stream) const;
%ignore libbitcoin::message::reject::to_data(uint32_t version, writer& sink) const;
%rename(assign) operator=(reject&& other);
%ignore operator=(const reject&);

%rename(eq) operator==(const reject& other) const;
%rename(ne) operator!=(const reject& other) const;
%include "../include/bitcoin/bitcoin/message/reject.hpp"

%ignore libbitcoin::message::send_compact::send_compact(send_compact&& other);
%ignore libbitcoin::message::send_compact::factory(uint32_t version, std::istream& stream);
%ignore libbitcoin::message::send_compact::factory(uint32_t version, reader& source);
%ignore libbitcoin::message::send_compact::from_data(uint32_t version, std::istream& stream);
%ignore libbitcoin::message::send_compact::from_data(uint32_t version, reader& source);
%ignore libbitcoin::message::send_compact::to_data(uint32_t version, std::ostream& stream) const;
%ignore libbitcoin::message::send_compact::to_data(uint32_t version, writer& sink) const;
%rename(assign) operator=(send_compact&& other);
%ignore operator=(const send_compact&);

%rename(eq) operator==(const send_compact& other) const;
%rename(ne) operator!=(const send_compact& other) const;
%include "../include/bitcoin/bitcoin/message/send_compact.hpp"

%ignore libbitcoin::message::send_headers::send_headers(send_headers&& other);
%ignore libbitcoin::message::send_headers::factory(uint32_t version, std::istream& stream);
%ignore libbitcoin::message::send_headers::factory(uint32_t version, reader& source);
%ignore libbitcoin::message::send_headers::from_data(uint32_t version, std::istream& stream);
%ignore libbitcoin::message::send_headers::from_data(uint32_t version, reader& source);
%ignore libbitcoin::message::send_headers::to_data(uint32_t version, std::ostream& stream) const;
%ignore libbitcoin::message::send_headers::to_data(uint32_t version, writer& sink) const;
%include "../include/bitcoin/bitcoin/message/send_headers.hpp"

%ignore libbitcoin::message::transaction::transaction(transaction&& other);
%ignore libbitcoin::message::transaction::transaction(chain::transaction&& other);
%ignore libbitcoin::message::transaction::transaction(uint32_t version, uint32_t locktime,
        chain::input::list&& inputs, chain::output::list&& outputs);
%ignore libbitcoin::message::transaction::factory(uint32_t version, std::istream& stream);
%ignore libbitcoin::message::transaction::factory(uint32_t version, reader& source);
%ignore libbitcoin::message::transaction::from_data(uint32_t version, std::istream& stream);
%ignore libbitcoin::message::transaction::from_data(uint32_t version, reader& source);
%ignore libbitcoin::message::transaction::to_data(uint32_t version, std::ostream& stream) const;
%ignore libbitcoin::message::transaction::to_data(uint32_t version, writer& sink) const;

%rename(assign) operator=(chain::transaction&& other);

%rename(assign) operator=(transaction&& other);
%ignore operator=(const transaction&);

%rename(eq) operator==(const chain::transaction& other) const;
%rename(ne) operator!=(const chain::transaction& other) const;

%rename(eq) operator==(const transaction& other) const;
%rename(ne) operator!=(const transaction& other) const;
%include "../include/bitcoin/bitcoin/message/transaction.hpp"

%ignore libbitcoin::message::verack::factory(uint32_t version, std::istream& stream);
%ignore libbitcoin::message::verack::factory(uint32_t version, reader& source);
%ignore libbitcoin::message::verack::from_data(uint32_t version, std::istream& stream);
%ignore libbitcoin::message::verack::from_data(uint32_t version, reader& source);
%ignore libbitcoin::message::verack::to_data(uint32_t version, std::ostream& stream) const;
%ignore libbitcoin::message::verack::to_data(uint32_t version, writer& sink) const;

%include "../include/bitcoin/bitcoin/message/verack.hpp"

%ignore libbitcoin::message::version::version(version&& other);
%ignore libbitcoin::message::version::version(uint32_t value, uint64_t services, uint64_t timestamp,
    network_address&& address_receiver, network_address&& address_sender,
    uint64_t nonce, std::string&& user_agent, uint32_t start_height,
    bool relay);
%ignore libbitcoin::message::version::address_receiver() const;
%ignore libbitcoin::message::version::address_sender() const;
%ignore libbitcoin::message::version::user_agent() const;
%ignore libbitcoin::message::version::factory(uint32_t version, std::istream& stream);
%ignore libbitcoin::message::version::factory(uint32_t version, reader& source);
%ignore libbitcoin::message::version::from_data(uint32_t version, std::istream& stream);
%ignore libbitcoin::message::version::from_data(uint32_t version, reader& source);
%ignore libbitcoin::message::version::to_data(uint32_t version, std::ostream& stream) const;
%ignore libbitcoin::message::version::to_data(uint32_t version, writer& sink) const;

%rename(assign) operator=(version&& other);
%ignore operator=(const version&);

%rename(eq) operator==(const version& other) const;
%rename(ne) operator!=(const version& other) const;

%include "../include/bitcoin/bitcoin/message/version.hpp"
