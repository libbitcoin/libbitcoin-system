.. _tut-intro:

***************
Introduction
***************

libbitcoin is a bitcoin library targeted towards high end use. The library
places a heavy focus around asychronicity. This enables a big scope for future
scalability as each component has its own thread pool. By increasing the number
of threads for that component the library is able to scale outwards across CPU
cores. This will be vital in the future as the demands of the bitcoin network
grow.

Another core design principle is libbitcoin is not a framework, but a toolkit.
Frameworks hinder development during the latter stages of a development cycle,
enforce one style of coding and do not work well with other frameworks. By
contrast, we have gone to great pains to make libbitcoin function as an
independent set of mutual components with no dependencies between them.

