%typemap("javapackage") OneByte, OneByte *, OneByte & "orb.libbitcoin.bitcoin.utility"
%typemap("javapackage") DataSlice, DataSlice *, DataSlice & "orb.libbitcoin.bitcoin.utility"

%typemap("javapackage") binary, binary *, binary & "orb.libbitcoin.bitcoin.utility"

%typemap("javapackage") data_chunk, data_chunk *, data_chunk & "orb.libbitcoin.bitcoin.utility"
%typemap("javapackage") data_stack, data_stack *, data_stack & "orb.libbitcoin.bitcoin.utility"

%typemap("javapackage") string_list, string_list *, string_list & "orb.libbitcoin.bitcoin"
