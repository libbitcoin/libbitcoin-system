
%import "bitcoin/bitcoin/config/settings.hpp"
%import "bitcoin/bitcoin/chain/chain_state.hpp"
%import "bitcoin/config_checkpoint.hpp"
%import "bitcoin/config_checkpoint_list.hpp"
%include "bitcoin/chain_chain_state.hpp"

%import "bitcoin/bitcoin/chain/header.hpp"
%include "bitcoin/chain_header.hpp"
%include "bitcoin/chain_header_list.hpp"

%import "bitcoin/bitcoin/chain/transaction.hpp"
%include "bitcoin/chain_transaction_list.hpp"
%include "bitcoin/chain_input_list.hpp"
%include "bitcoin/chain_output_list.hpp"
%include "bitcoin/chain_point_list.hpp"
%include "bitcoin/chain_transaction.hpp"


%import "bitcoin/bitcoin/chain/block.hpp"
%import "bitcoin/unsigned_char_vector.hpp"
%import "bitcoin/uint8_t_vector.hpp"
%import "bitcoin/utility_data_chunk.hpp"
%import "bitcoin/math_hash_digest.hpp"
%import "bitcoin/math_hash_list.hpp"
%ignore cast();
%include "bitcoin/chain_point_indexes.hpp"
%import "bitcoin/bitcoin/chain/block.hpp"
%include "bitcoin/chain_block_indexes.hpp"
%import "bitcoin/error_code.hpp"
%include "bitcoin/chain_block.hpp"
%include "bitcoin/chain_block_list.hpp"

%import "bitcoin/bitcoin/chain/compact.hpp"
%import "bitcoin/math_uint256_t.hpp"
%include "bitcoin/chain_compact.hpp"

%import "bitcoin/bitcoin/chain/input.hpp"
%include "bitcoin/chain_output_point.hpp"
%import "bitcoin/bitcoin/chain/script.hpp"
%import "bitcoin/math_endorsement.hpp"
%import "bitcoin/machine_operation_iterator.hpp"
%include "bitcoin/chain_script_operation_list.hpp"
%import "bitcoin/utility_data_stack.hpp"
%include "bitcoin/chain_script.hpp"
%include "bitcoin/chain_witness.hpp"
%import "bitcoin/wallet_payment_address.hpp"
%import "bitcoin/wallet_payment_address_list.hpp"
%include "bitcoin/chain_input.hpp"
%include "bitcoin/chain_input_list.hpp"

%import "bitcoin/bitcoin/chain/input_point.hpp"
%include "bitcoin/chain_input_point.hpp"

%import "bitcoin/bitcoin/chain/point.hpp"
%include "bitcoin/chain_point.hpp"
%include "bitcoin/chain_point_list.hpp"

%import "bitcoin/bitcoin/chain/output.hpp"
%include "bitcoin/chain_output.hpp"
%include "bitcoin/chain_output_list.hpp"

%import "bitcoin/bitcoin/chain/output_point.hpp"
%include "bitcoin/chain_output_point.hpp"

%import "bitcoin/bitcoin/chain/point_value.hpp"
%include "bitcoin/chain_point_value.hpp"

%import "bitcoin/bitcoin/chain/points_value.hpp"
%include "bitcoin/chain_point_value_list.hpp"
%include "bitcoin/chain_points_value.hpp"

