%typemap("javapackage") authority, authority *, authority & "orb.libbitcoin.bitcoin.config"
%typemap("javapackage") checkpoint, checkpoint *, checkpoint & "orb.libbitcoin.bitcoin.config"
%typemap("javapackage") header, header *, header & "orb.libbitcoin.bitcoin.config"
%typemap("javapackage") input, input *, input & "orb.libbitcoin.bitcoin.config"
%typemap("javapackage") transaction, transaction *, transaction & "orb.libbitcoin.bitcoin.config"

%typemap("javapackage") ConfigHeaderVector, ConfigHeaderVector *, ConfigHeaderVector & "orb.libbitcoin.bitcoin.config"
%typemap("javapackage") ConfigInputVector, ConfigInputVector *, ConfigInputVector & "orb.libbitcoin.bitcoin.config"
%typemap("javapackage") ConfigTransactionVector, ConfigTransactionVector *, ConfigTransactionVector & "orb.libbitcoin.bitcoin.config"
