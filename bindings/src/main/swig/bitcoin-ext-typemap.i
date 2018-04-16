%typemap("javapackage") uint8Array, uint8Array *, uint8Array & "orb.libbitcoin.bitcoin"
%typemap("javapackage") uint16Array, uint16Array *, uint16Array & "orb.libbitcoin.bitcoin"
%typemap("javapackage") uint32Array, uint32Array *, uint32Array & "orb.libbitcoin.bitcoin"
%typemap("javapackage") uint64Array, uint64Array *, uint64Array & "orb.libbitcoin.bitcoin"
%typemap("javapackage") int32Array, int32Array *, int32Array & "orb.libbitcoin.bitcoin"
%typemap("javapackage") int64Array, int64Array *, int64Array & "orb.libbitcoin.bitcoin"

%typemap("javapackage") ULongLong, ULongLong *, ULongLong & "orb.libbitcoin.bitcoin"
%typemap("javapackage") Bool, Bool *, Bool & "orb.libbitcoin.bitcoin"
//%typemap("javapackage") Int32, Int32 *, Int32 & "orb.libbitcoin.bitcoin"

%typemap("javapackage") ucharVector, ucharVector *, ucharVector & "orb.libbitcoin.bitcoin"
%typemap("javapackage") ucharVectorVector, ucharVectorVector *, ucharVectorVector & "orb.libbitcoin.bitcoin"
%typemap("javapackage") ucharArray, ucharArray *, ucharArray & "orb.libbitcoin.bitcoin"

%typemap("javapackage") error, error *, error & "orb.libbitcoin.bitcoin"
