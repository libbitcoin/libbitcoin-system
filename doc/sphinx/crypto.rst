.. _tut-crypto:

***********************
Crypto and Private Keys
***********************

Vanilla Private Keys
====================

Bitcoin uses a form of encryption called elliptic curve cryptography.
libbitcoin represents this type of private key using the
:class:`elliptic_curve_key` class.
::

    // Generate new private key and echo to STDOUT.
    bool new_keypair()
    {
        elliptic_curve_key ec;
        if (!ec.new_key_pair())
            return false;
        private_data raw_private_key = ec.private_key();
        std::cout << std::string(raw_private_key.begin(), raw_private_key.end());
        return true;
    }

To load the private_key, we can use the :func:`set_private_key` member function.

.. cpp:function:: bool elliptic_curve_key::set_public_key(const data_chunk& pubkey)
.. cpp:function:: data_chunk elliptic_curve_key::public_key() const

.. cpp:function:: bool elliptic_curve_key::set_private_key(const private_data& privkey)
.. cpp:function:: private_data elliptic_curve_key::private_key() const

Using private key functionality when a public key is set is undefined, and
most operations will likely fail.

:ref:`examples_priv` is an example program for working with Bitcoin private keys.
The example shows generating private keys, showing the address and
signing/verifying of a hash digest.

Show Your Bitcoin Address
-------------------------

A Bitcoin address is a specially encoded hash of the public key. First the
public key is hashed with :func:`generate_ripemd_hash` by calling RIPEMD twice.
Then a checksum is generated with :func:`generate_sha256_checksum` and appended.
A version byte is prepended, usually :type:`payment_address::pubkey_version`.
Finally the whole byte array is encoded in base58 using :func:`encode_base58`.

Bitcoin addresses are between 27-34 alphanumeric characters. The
:class:`payment_address` class represents a Bitcoin address.
::

    // Takes a raw private key, loads it using the elliptic_curve_key class,
    // and displays the Bitcoin address for it.
    bool display_address(const std::string raw_private_key)
    {
        elliptic_curve_key ec;
        if (!ec.set_private_key(
                private_data(raw_private_key.begin(), raw_private_key.end())))
            return false;
        payment_address address;
        if (!set_public_key(address, ec.public_key()))
            return false;
        log_info() << address.encoded();
        return true;
    }

:class:`payment_address` is able to tell us more information about a
Bitcoin address such as its payment type.
::

    // We can also use address.set_encoded(addr) and check the bool return value.
    payment_address address("1MuqPCL7Uven1pAPzfXPVpErZq1R8G3yHs");
    // payment_type enum is defined in script.hpp
    assert(address.type() == payment_type::pubkey_hash);

Deterministic Wallets
=====================

A deterministic wallet is a wallet where private and public keys are all derived
from a starting seed value. Take ``hash(seed + n)`` where ``n`` starts from 1
and increment as additional keys are needed. Use this value to generate the nth
private key. This is a deterministic wallet.

A deterministic wallet can be backed up by copying the starting seed
value to a secure location, and this only needs to be done once. If the wallet
ever gets lost, all private and public keys can be regenerated from the initial
seed.

Also, multiple devices could host the same wallet based off of the same seed and
automatically stay in sync with eachother. Non-critical information such as
address books would need to be stored and copied between wallets. 

libbitcoin implements the same compatible deterministic wallet algorithm as `Electrum <http://electrum.org/>`_.
Wallet seeds from the Bitcoin client Electrum are usable in libbitcoin.
Deterministic wallets are always initialized with a seed whether newly
generated or set.
::

    deterministic_wallet wallet;
    // Create new seed.
    wallet.new_seed();
    log_info() << "new seed: " << wallet.seed();

::

    deterministic_wallet wallet;
    // Set seed.
    if (!wallet.set_seed("a219213f9b12422aa206d988e3e49607"))
        log_error() << "Error setting seed.";

Calling :func:`deterministic_wallet::generate_public_key` with an integer n generates
the nth public key. To iterate through the public keys in a wallet, repeatedly
call :func:`deterministic_wallet::generate_public_key` starting at 1 and incrementing.

Deterministic wallets usually use a *gap limit* number to decide when to stop
incrementing n. The *gap limit* is how many sequential addresses must have a
0 balance before it stops generating Bitcoin addresses.
::

    // Get an address from wallet...
    data_chunk pubkey = wallet.generate_public_key(2);
    payment_address addr;
    if (!set_public_key(addr, pubkey))
        log_error() << "Error setting public key.";
    assert(addr.encoded() == "1E4vM9q25xsyDwWwdqHUWnwshdWC9PykmL");

Computing the corresponding private key involves a similar logic. Instead of
reading the private key directly, a value called the *secret parameter* is
created by the deterministic wallet. This is put into the elliptic curve
formula and used to create the private key.
::

    // ... Get the corresponding private key.
    // Extract the secret parameter.
    secret_parameter secret = wallet.generate_secret(2);
    assert(pretty_hex(secret) == "33cc7e35fbb78d17d207e53d0fe950d1db571be889b3ff87aec653e501759264");
    // The secret parameter is used to compute the private key
    // by the elliptic curve formula.
    elliptic_curve_key privkey;
    if (!privkey.set_secret(secret))
        log_error() << "Error set private key.";
    // Wallet generated public key should match corresponding public key
    // in the private key.
    assert(privkey.public_key() == pubkey);

To see all this working together, view :ref:`examples_determ`.

Master Public Key
-----------------

The master public key is an interesting concept. A :class:`deterministic_wallet`
can be initialized with a master public key that allows generating all the
public keys with :func:`deterministic_wallet::generate_public_key`, but not
the corresponding private keys (through the secret parameter).

Imagine a small business owner who wants their staff to have access to deposit
addresses in their wallet to accept payments from customers, but not the
ability to access all the funds. Waiters in a restaurant can accept Bitcoin
payments which only the shop owner can spend.

Another use-case is a website keeping their Bitcoins offline. They can accept
payments into their offline wallet. Without access to their seed (which is kept
offline), nobody can spend their Bitcoins.
::

    data_chunk mpk = wallet.master_public_key();
    assert(pretty_hex(mpk) == "d996c1a50ca4a57a9dface614338a1d837cb339e08361cfaf66ffd7da8e21786a7142a014056439d579654d7bb58dd5724b93372b5efae62e76783300f2b6cb5");

A master key can only generate public keys but never the private keys.
Calls to :func:`deterministic_wallet::generate_secret` will always return
a zero-initialized :type:`hash_digest`, defined as the constant *null_hash*.
::

    deterministic_wallet wallet2;
    wallet2.set_master_public_key(mpk);
    assert(wallet2.generate_public_key(2) == pubkey);
    // Trying to generate the secret parameter will always return null_hash.
    assert(wallet2.generate_secret(2) == null_hash);

Different Key Formats
=====================

Hex-Encoded Secret
------------------

The *secret parameter* is a value used by the elliptic curve formula to
compute the private key.
::

    secret_parameter secret = hash_from_pretty<secret_parameter>("33cc7e35fbb78d17d207e53d0fe950d1db571be889b3ff87aec653e501759264");
    // The secret parameter is used to compute the private key
    // by the elliptic curve formula.
    elliptic_curve_key privkey;
    if (!privkey.set_secret(secret))
        log_error() << "Error set private key.";

::

    // Display the secret parameter.
    std::cout << privkey.secret() << std::endl;

Wallet Import Format
--------------------

Wallet Import Format (WIF) is a way to encode the secret parameter to make
copying the private key easier.

.. cpp:function:: std::string secret_to_wif(const secret_parameter& secret)

   Convert a secret parameter to the wallet import format.
   Returns an empty string on error.
   ::

    std::string wif = secret_to_wif(secret);
    if (wif.empty())
        // Error...

.. cpp:function:: secret_parameter wif_to_secret(const std::string& wif)

   Convert wallet import format key to secret parameter.
   Returns a nulled secret on error.
   ::

    secret_parameter secret = wif_to_secret(
        "5HueCGU8rMjxEXxiPuD5BDku4MkFqeZyd4dZ1jvhTVqvbTLvyTJ");
    if (secret == null_hash)
        // Error...

Casascius Minikey
-----------------

Casascius coins encode private keys in a format known as Casascius minikey.
:func:`minikey_to_secret` converts a Casascius minikey to a secret parameter.

.. cpp:function:: secret_parameter minikey_to_secret(const std::string& minikey)

   Convert Cascasius minikey to secret parameter.
   Returns a nulled secret on error.
   ::

    secret_parameter secret =
        minikey_to_secret("S6c56bnXQiBjk9mqSYE7ykVQ7NzrRy");
    if (secret == null_hash)
        // Error...

