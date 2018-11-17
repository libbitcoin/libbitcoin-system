//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2005-2012. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/interprocess for documentation.
//
//////////////////////////////////////////////////////////////////////////////

// Modification of boost header to patch a lack of unicode path support.

#ifndef LIBBITCOIN_SYSTEM_INTERPROCESS_FILE_LOCK_HPP
#define LIBBITCOIN_SYSTEM_INTERPROCESS_FILE_LOCK_HPP

#include <string>
#include <boost/interprocess/detail/config_begin.hpp>
#include <boost/interprocess/detail/workaround.hpp>
#include <boost/interprocess/exceptions.hpp>
#include <boost/interprocess/detail/os_file_functions.hpp>
#include <boost/interprocess/detail/os_thread_functions.hpp>
#include <boost/interprocess/detail/posix_time_types_wrk.hpp>
#include <boost/interprocess/sync/detail/common_algorithms.hpp>
#include <boost/interprocess/sync/detail/locks.hpp>
#include <boost/move/move.hpp>

// Includes <windows.h> from bitcoin/unicode.hpp under _MSC_VER.
#include <bitcoin/bitcoin/unicode/unicode.hpp>

//!\file
//!Describes a class that wraps file locking capabilities.

namespace libbitcoin {
namespace interprocess {

//!A file lock, is a mutual exclusion utility similar to a mutex using a
//!file. A file lock has sharable and exclusive locking capabilities and
//!can be used with scoped_lock and sharable_lock classes.
//!A file lock can't guarantee synchronization between threads of the same
//!process so just use file locks to synchronize threads from different processes.
class file_lock
{
   /// @cond
   //Non-copyable
   BOOST_MOVABLE_BUT_NOT_COPYABLE(file_lock)
   /// @endcond

   public:
   //!Constructs an empty file mapping.
   //!Does not throw
   file_lock()
      :  m_file_hnd(boost::interprocess::file_handle_t(boost::interprocess::ipcdetail::invalid_file()))
   {}

   // MODIFIED TO EXPECT UTF8 ENCODING ON WINDOWS.
   //!Opens a file lock. Throws boost::interprocess::interprocess_exception if the file does not
   //!exist or there are no operating system resources.
   file_lock(const std::string& name);

   //!Moves the ownership of "moved"'s file mapping object to *this.
   //!After the call, "moved" does not represent any file mapping object.
   //!Does not throw
   file_lock(BOOST_RV_REF(file_lock) moved)
      :  m_file_hnd(boost::interprocess::file_handle_t(boost::interprocess::ipcdetail::invalid_file()))
   {  this->swap(moved);   }

   //!Moves the ownership of "moved"'s file mapping to *this.
   //!After the call, "moved" does not represent any file mapping.
   //!Does not throw
   file_lock &operator=(BOOST_RV_REF(file_lock) moved)
   {
      file_lock tmp(boost::move(moved));
      this->swap(tmp);
      return *this;
   }

   //!Closes a file lock. Does not throw.
   virtual ~file_lock();

   //!Swaps two file_locks.
   //!Does not throw.
   void swap(file_lock &other)
   {
      boost::interprocess::file_handle_t tmp = m_file_hnd;
      m_file_hnd = other.m_file_hnd;
      other.m_file_hnd = tmp;
   }

   //Exclusive locking

   //!Effects: The calling thread tries to obtain exclusive ownership of the mutex,
   //!   and if another thread has exclusive, or sharable ownership of
   //!   the mutex, it waits until it can obtain the ownership.
   //!Throws: boost::interprocess::interprocess_exception on error.
   void lock();

   //!Effects: The calling thread tries to acquire exclusive ownership of the mutex
   //!   without waiting. If no other thread has exclusive, or sharable
   //!   ownership of the mutex this succeeds.
   //!Returns: If it can acquire exclusive ownership immediately returns true.
   //!   If it has to wait, returns false.
   //!Throws: boost::interprocess::interprocess_exception on error.
   bool try_lock();

   //!Effects: The calling thread tries to acquire exclusive ownership of the mutex
   //!   waiting if necessary until no other thread has exclusive, or sharable
   //!   ownership of the mutex or abs_time is reached.
   //!Returns: If acquires exclusive ownership, returns true. Otherwise returns false.
   //!Throws: boost::interprocess::interprocess_exception on error.
   bool timed_lock(const boost::posix_time::ptime &abs_time);

   //!Precondition: The thread must have exclusive ownership of the mutex.
   //!Effects: The calling thread releases the exclusive ownership of the mutex.
   //!Throws: An exception derived from boost::interprocess::interprocess_exception on error.
   void unlock();

   //Sharable locking

   //!Effects: The calling thread tries to obtain sharable ownership of the mutex,
   //!   and if another thread has exclusive ownership of the mutex, waits until
   //!   it can obtain the ownership.
   //!Throws: boost::interprocess::interprocess_exception on error.
   void lock_sharable();

   //!Effects: The calling thread tries to acquire sharable ownership of the mutex
   //!   without waiting. If no other thread has exclusive ownership of the
   //!   mutex this succeeds.
   //!Returns: If it can acquire sharable ownership immediately returns true. If it
   //!   has to wait, returns false.
   //!Throws: boost::interprocess::interprocess_exception on error.
   bool try_lock_sharable();

   //!Effects: The calling thread tries to acquire sharable ownership of the mutex
   //!   waiting if necessary until no other thread has exclusive ownership of
   //!   the mutex or abs_time is reached.
   //!Returns: If acquires sharable ownership, returns true. Otherwise returns false.
   //!Throws: boost::interprocess::interprocess_exception on error.
   bool timed_lock_sharable(const boost::posix_time::ptime &abs_time);

   //!Precondition: The thread must have sharable ownership of the mutex.
   //!Effects: The calling thread releases the sharable ownership of the mutex.
   //!Throws: An exception derived from boost::interprocess::interprocess_exception on error.
   void unlock_sharable();
   /// @cond
   private:
   boost::interprocess::file_handle_t m_file_hnd;

   /// @endcond
};

#ifdef _MSC_VER

extern "C" __declspec(dllimport) void * __stdcall CreateFileW(const wchar_t *, unsigned long, unsigned long, struct boost::interprocess::winapi::interprocess_security_attributes*, unsigned long, unsigned long, void *);

// ENABLE UNICODE PATHS ON WINDOWS FROM UTF8 STRING REGARDLESS OF BUILD CONFIGURATION.
inline void* CreateFileUTF8(const std::string& name, unsigned long access, unsigned long mode,
    struct boost::interprocess::winapi::interprocess_security_attributes *psec, unsigned long creation, unsigned long attributes, void *ptemplate)
{
    return CreateFileW(bc::to_utf16(name).c_str(), access, mode, psec, creation, attributes, ptemplate);
}

// ADAPTED FROM boost/interprocess/winapi/win32_api.hpp UNDER SAME LICENSE AS ABOVE.
inline void *create_file(const std::string& name, unsigned long access, unsigned long creation_flags, unsigned long attributes, boost::interprocess::winapi::interprocess_security_attributes *psec)
{
    for (unsigned int attempt(0); attempt < boost::interprocess::winapi::error_sharing_violation_tries; ++attempt){
        void * const handle = libbitcoin::interprocess::CreateFileUTF8(name, access,
            boost::interprocess::winapi::file_share_read | boost::interprocess::winapi::file_share_write | boost::interprocess::winapi::file_share_delete,
            psec, creation_flags, attributes, 0);
        bool const invalid(boost::interprocess::winapi::invalid_handle_value == handle);
        if (!invalid){
            return handle;
        }
        if (boost::interprocess::winapi::error_sharing_violation != boost::interprocess::winapi::get_last_error()){
            return handle;
        }
        boost::interprocess::winapi::sleep(boost::interprocess::winapi::error_sharing_violation_sleep_ms);
    }
    return boost::interprocess::winapi::invalid_handle_value;
}

// ADAPTED FROM boost/interprocess/os_file_functions.hpp UNDER SAME LICENSE AS ABOVE.
inline boost::interprocess::file_handle_t open_existing_file
(const std::string& name, boost::interprocess::mode_t mode, bool temporary = false)
{
    unsigned long attr = temporary ? boost::interprocess::winapi::file_attribute_temporary : 0;
    return libbitcoin::interprocess::create_file
        (name, (unsigned int)mode, boost::interprocess::winapi::open_existing, attr, 0);
}

#endif

// MODIFIED TO ACCEPT STRING IN SUPPORT OF WINDOWS UNICODE PATHS.
inline file_lock::file_lock(const std::string& name)
{
#ifdef _MSC_VER
    m_file_hnd = libbitcoin::interprocess::open_existing_file(name, boost::interprocess::read_write);
#else
    m_file_hnd = boost::interprocess::ipcdetail::open_existing_file(name.c_str(), boost::interprocess::read_write);
#endif

   if(m_file_hnd == boost::interprocess::ipcdetail::invalid_file()){
      boost::interprocess::error_info err(boost::interprocess::system_error_code());
      throw boost::interprocess::interprocess_exception(err);
   }
}

inline file_lock::~file_lock()
{
   if(m_file_hnd != boost::interprocess::ipcdetail::invalid_file()){
      boost::interprocess::ipcdetail::close_file(m_file_hnd);
      m_file_hnd = boost::interprocess::ipcdetail::invalid_file();
   }
}

inline void file_lock::lock()
{
   if(!boost::interprocess::ipcdetail::acquire_file_lock(m_file_hnd)){
      boost::interprocess::error_info err(boost::interprocess::system_error_code());
      throw boost::interprocess::interprocess_exception(err);
   }
}

inline bool file_lock::try_lock()
{
   bool result;
   if(!boost::interprocess::ipcdetail::try_acquire_file_lock(m_file_hnd, result)){
      boost::interprocess::error_info err(boost::interprocess::system_error_code());
      throw boost::interprocess::interprocess_exception(err);
   }
   return result;
}

inline bool file_lock::timed_lock(const boost::posix_time::ptime &abs_time)
{  return boost::interprocess::ipcdetail::try_based_timed_lock(*this, abs_time);   }

inline void file_lock::unlock()
{
   if(!boost::interprocess::ipcdetail::release_file_lock(m_file_hnd)){
      boost::interprocess::error_info err(boost::interprocess::system_error_code());
      throw boost::interprocess::interprocess_exception(err);
   }
}

inline void file_lock::lock_sharable()
{
   if(!boost::interprocess::ipcdetail::acquire_file_lock_sharable(m_file_hnd)){
      boost::interprocess::error_info err(boost::interprocess::system_error_code());
      throw boost::interprocess::interprocess_exception(err);
   }
}

inline bool file_lock::try_lock_sharable()
{
   bool result;
   if(!boost::interprocess::ipcdetail::try_acquire_file_lock_sharable(m_file_hnd, result)){
      boost::interprocess::error_info err(boost::interprocess::system_error_code());
      throw boost::interprocess::interprocess_exception(err);
   }
   return result;
}

inline bool file_lock::timed_lock_sharable(const boost::posix_time::ptime &abs_time)
{
   boost::interprocess::ipcdetail::lock_to_sharable<file_lock> lsh(*this);
   return boost::interprocess::ipcdetail::try_based_timed_lock(lsh, abs_time);
}

inline void file_lock::unlock_sharable()
{
   if(!boost::interprocess::ipcdetail::release_file_lock_sharable(m_file_hnd)){
      boost::interprocess::error_info err(boost::interprocess::system_error_code());
      throw boost::interprocess::interprocess_exception(err);
   }
}

} // namespace interprocess
} // namespace libbitcoin

#include <boost/interprocess/detail/config_end.hpp>

#endif // LIBBITCOIN_INTERPROCESS_FILE_LOCK_HPP
