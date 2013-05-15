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

The approach we took to our threaded design is built not around the data, but
around tasks. On a finer level: operations. libbitcoin is a toolkit library that
uses the proactor design pattern. It implements the proactor pattern through the
use of completion handlers like in boost::asio.

* **Portability**. The library should support a range of commonly used operating systems, and provide consistent behaviour across these operating systems.
* **Scalability**. The library should facilitate the development of network applications that scale to thousands of concurrent connections. The library implementation for each operating system should use the mechanism that best enables this scalability.
* **Efficiency**. The library should support techniques such as scatter-gather I/O, and allow programs to minimise data copying.
* **Model concepts in an intuitive manner**. The library models different subsystems of bitcoin in a clear and intuitive manner. We choose abstractions that allow designing a wide range of applications that rely on bitcoin.
* **Basis for further abstraction**. The library should permit the development of other libraries that provide higher levels of abstraction. For example, implementations of the bitcoin protocol in other networks such as tor.
* **No blocking**. No blocking ever occurs waiting for another thread to complete (except possibly on a low level within boost dispatches- but that is uncommon).
* **UNIX approach**. The library attempts to provide small units of functionality that perform one single task. Our philosophy is to break down higher level functionality into small parts and to simply provide those parts. The cost is inconvenience. The benefit is flexibility.

The Zen of libbitcoin
=====================

| Readability over speed.
| Beauty over convenience.
| Simplicity over complexity.
| Architected, not hacked.
| Flat, not nested.
| Explicit, not implicit.
| Errors should be loud.
| Never is better than right now.
| Now is better than never.
| Be flexible and configurable.
| Build houses from bricks, software from modules. 

