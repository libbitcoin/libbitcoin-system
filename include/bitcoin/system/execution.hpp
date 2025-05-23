/**
 * Thread pool-based implementation of parallel standard library algorithms.
 * github.com/alugowski/poolSTL
 *
 * MIT License:
 *
 * Copyright (c) 2023 Adam Lugowski
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */


#ifndef POOLSTL_HPP
#define POOLSTL_HPP


#ifndef POOLSTL_EXECUTION_HPP
#define POOLSTL_EXECUTION_HPP

#include <memory>
#include <mutex>
#include <stdexcept>
#include <type_traits>


#ifndef AL_TASK_THREAD_POOL_HPP
#define AL_TASK_THREAD_POOL_HPP

// Version macros.
#define TASK_THREAD_POOL_VERSION_MAJOR 1
#define TASK_THREAD_POOL_VERSION_MINOR 0
#define TASK_THREAD_POOL_VERSION_PATCH 10

#include <condition_variable>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <type_traits>

// MSVC does not correctly set the __cplusplus macro by default, so we must read it from _MSVC_LANG
// See https://devblogs.microsoft.com/cppblog/msvc-now-correctly-reports-__cplusplus/
#if __cplusplus >= 201703L || (defined(_MSVC_LANG) && _MSVC_LANG >= 201703L)
#define TTP_CXX17 1
#else
#define TTP_CXX17 0
#endif

#if TTP_CXX17
#define TTP_NODISCARD [[nodiscard]]
#else
#define TTP_NODISCARD
#endif

namespace task_thread_pool {

#if !TTP_CXX17
    /**
     * A reimplementation of std::decay_t, which is only available since C++14.
     */
    template <class T>
    using decay_t = typename std::decay<T>::type;
#endif

    /**
     * A fast and lightweight thread pool that uses C++11 threads.
     */
    class task_thread_pool {
    public:
        /**
         * Create a task_thread_pool and start worker threads.
         *
         * @param num_threads Number of worker threads. If 0 then number of threads is equal to the
         *                    number of physical cores on the machine, as given by std::thread::hardware_concurrency().
         */
        explicit task_thread_pool(unsigned int num_threads = 0) {
            if (num_threads < 1) {
                num_threads = std::thread::hardware_concurrency();
                if (num_threads < 1) { num_threads = 1; }
            }
            start_threads(num_threads);
        }

        /**
         * Finish all tasks left in the queue then shut down worker threads.
         * If the pool is currently paused then it is resumed.
         */
        ~task_thread_pool() {
            unpause();
            wait_for_queued_tasks();
            stop_all_threads();
        }

        /**
         * Drop all tasks that have been submitted but not yet started by a worker.
         *
         * Tasks already in progress continue executing.
         */
        void clear_task_queue() {
            const std::lock_guard<std::mutex> tasks_lock(task_mutex);
            tasks = {};
        }

        /**
         * Get number of enqueued tasks.
         *
         * @return Number of tasks that have been enqueued but not yet started.
         */
        TTP_NODISCARD size_t get_num_queued_tasks() const {
            const std::lock_guard<std::mutex> tasks_lock(task_mutex);
            return tasks.size();
        }

        /**
         * Get number of in-progress tasks.
         *
         * @return Approximate number of tasks currently being processed by worker threads.
         */
        TTP_NODISCARD size_t get_num_running_tasks() const {
            const std::lock_guard<std::mutex> tasks_lock(task_mutex);
            return num_inflight_tasks;
        }

        /**
         * Get total number of tasks in the pool.
         *
         * @return Approximate number of tasks both enqueued and running.
         */
        TTP_NODISCARD size_t get_num_tasks() const {
            const std::lock_guard<std::mutex> tasks_lock(task_mutex);
            return tasks.size() + num_inflight_tasks;
        }

        /**
         * Get number of worker threads.
         *
         * @return Number of worker threads.
         */
        TTP_NODISCARD unsigned int get_num_threads() const {
            const std::lock_guard<std::recursive_mutex> threads_lock(thread_mutex);
            return static_cast<unsigned int>(threads.size());
        }

        /**
         * Set number of worker threads. Will start or stop worker threads as necessary.
         *
         * @param num_threads Number of worker threads. If 0 then number of threads is equal to the
         *                    number of physical cores on the machine, as given by std::thread::hardware_concurrency().
         * @return Previous number of worker threads.
         */
        unsigned int set_num_threads(unsigned int num_threads) {
            const std::lock_guard<std::recursive_mutex> threads_lock(thread_mutex);
            unsigned int previous_num_threads = get_num_threads();

            if (num_threads < 1) {
                num_threads = std::thread::hardware_concurrency();
                if (num_threads < 1) { num_threads = 1; }
            }

            if (previous_num_threads <= num_threads) {
                // expanding the thread pool
                start_threads(num_threads - previous_num_threads);
            } else {
                // contracting the thread pool
                stop_all_threads();
                {
                    const std::lock_guard<std::mutex> tasks_lock(task_mutex);
                    pool_running = true;
                }
                start_threads(num_threads);
            }

            return previous_num_threads;
        }

        /**
         * Stop executing queued tasks. Use `unpause()` to resume. Note: Destroying the pool will implicitly unpause.
         *
         * Any in-progress tasks continue executing.
         */
        void pause() {
            const std::lock_guard<std::mutex> tasks_lock(task_mutex);
            pool_paused = true;
        }

        /**
         * Resume executing queued tasks.
         */
        void unpause() {
            const std::lock_guard<std::mutex> tasks_lock(task_mutex);
            pool_paused = false;
            task_cv.notify_all();
        }

        /**
         * Check whether the pool is paused.
         *
         * @return true if pause() has been called without an intervening unpause().
         */
        TTP_NODISCARD bool is_paused() const {
            const std::lock_guard<std::mutex> tasks_lock(task_mutex);
            return pool_paused;
        }

        /**
         * Submit a Callable for the pool to execute and return a std::future.
         *
         * @param func The Callable to execute. Can be a function, a lambda, std::packaged_task, std::function, etc.
         * @param args Arguments for func. Optional.
         * @return std::future that can be used to get func's return value or thrown exception.
         */
        template <typename F, typename... A,
#if TTP_CXX17
            typename R = std::invoke_result_t<std::decay_t<F>, std::decay_t<A>...>
#else
            typename R = typename std::result_of<decay_t<F>(decay_t<A>...)>::type
#endif
        >
        TTP_NODISCARD std::future<R> submit(F&& func, A&&... args) {
#if defined(_MSC_VER)
            // MSVC's packaged_task is not movable even though it should be.
            // Discussion about this bug and its future fix:
            // https://developercommunity.visualstudio.com/t/unable-to-move-stdpackaged-task-into-any-stl-conta/108672
            std::shared_ptr<std::packaged_task<R()>> ptask =
                std::make_shared<std::packaged_task<R()>>(std::bind(std::forward<F>(func), std::forward<A>(args)...));
            submit_detach([ptask] { (*ptask)(); });
            return ptask->get_future();
#else
            std::packaged_task<R()> task(std::bind(std::forward<F>(func), std::forward<A>(args)...));
            auto ret = task.get_future();
            submit_detach(std::move(task));
            return ret;
#endif
        }

        /**
         * Submit a zero-argument Callable for the pool to execute.
         *
         * @param func The Callable to execute. Can be a function, a lambda, std::packaged_task, std::function, etc.
         */
        template <typename F>
        void submit_detach(F&& func) {
            const std::lock_guard<std::mutex> tasks_lock(task_mutex);
            tasks.emplace(std::forward<F>(func));
            task_cv.notify_one();
        }

        /**
         * Submit a Callable with arguments for the pool to execute.
         *
         * @param func The Callable to execute. Can be a function, a lambda, std::packaged_task, std::function, etc.
         */
        template <typename F, typename... A>
        void submit_detach(F&& func, A&&... args) {
            const std::lock_guard<std::mutex> tasks_lock(task_mutex);
            tasks.emplace(std::bind(std::forward<F>(func), std::forward<A>(args)...));
            task_cv.notify_one();
        }

        /**
         * Block until the task queue is empty. Some tasks may be in-progress when this method returns.
         */
        void wait_for_queued_tasks() {
            std::unique_lock<std::mutex> tasks_lock(task_mutex);
            notify_task_finish = true;
            task_finished_cv.wait(tasks_lock, [&] { return tasks.empty(); });
            notify_task_finish = false;
        }

        /**
         * Block until all tasks have finished.
         */
        void wait_for_tasks() {
            std::unique_lock<std::mutex> tasks_lock(task_mutex);
            notify_task_finish = true;
            task_finished_cv.wait(tasks_lock, [&] { return tasks.empty() && num_inflight_tasks == 0; });
            notify_task_finish = false;
        }

    protected:

        /**
         * Main function for worker threads.
         */
        void worker_main() {
            bool finished_task = false;

            while (true) {
                std::unique_lock<std::mutex> tasks_lock(task_mutex);

                if (finished_task) {
                    --num_inflight_tasks;
                    if (notify_task_finish) {
                        task_finished_cv.notify_all();
                    }
                }

                task_cv.wait(tasks_lock, [&]() { return !pool_running || (!pool_paused && !tasks.empty()); });

                if (!pool_running) {
                    break;
                }

                // Must mean that (!pool_paused && !tasks.empty()) is true

                std::packaged_task<void()> task{std::move(tasks.front())};
                tasks.pop();
                ++num_inflight_tasks;
                tasks_lock.unlock();

                try {
                    task();
                } catch (...) {
                    // std::packaged_task::operator() may throw in some error conditions, such as if the task
                    // had already been run. Nothing that the pool can do anything about.
                }

                finished_task = true;
            }
        }

        /**
         * Start worker threads.
         *
         * @param num_threads How many threads to start.
         */
        void start_threads(const unsigned int num_threads) {
            const std::lock_guard<std::recursive_mutex> threads_lock(thread_mutex);

            for (unsigned int i = 0; i < num_threads; ++i) {
                threads.emplace_back(&task_thread_pool::worker_main, this);
            }
        }

        /**
         * Stop, join, and destroy all worker threads.
         */
        void stop_all_threads() {
            const std::lock_guard<std::recursive_mutex> threads_lock(thread_mutex);

            {
                const std::lock_guard<std::mutex> tasks_lock(task_mutex);
                pool_running = false;
                task_cv.notify_all();
            }

            for (auto& thread : threads) {
                if (thread.joinable()) {
                    thread.join();
                }
            }
            threads.clear();
        }

        /**
         * The worker threads.
         *
         * Access protected by thread_mutex
         */
        std::vector<std::thread> threads;

        /**
         * A mutex for methods that start/stop threads.
         */
        mutable std::recursive_mutex thread_mutex;

        /**
         * The task queue.
         *
         * Access protected by task_mutex.
         */
        std::queue<std::packaged_task<void()>> tasks = {};

        /**
         * A mutex for all variables related to tasks.
         */
        mutable std::mutex task_mutex;

        /**
         * Used to notify changes to the task queue, such as a new task added, pause/unpause, etc.
         */
        std::condition_variable task_cv;

        /**
         * Used to notify of finished tasks.
         */
        std::condition_variable task_finished_cv;

        /**
         * A signal for worker threads that the pool is either running or shutting down.
         *
         * Access protected by task_mutex.
         */
        bool pool_running = true;

        /**
         * A signal for worker threads to not pull new tasks from the queue.
         *
         * Access protected by task_mutex.
         */
        bool pool_paused = false;

        /**
         * A signal for worker threads that they should notify task_finished_cv when they finish a task.
         *
         * Access protected by task_mutex.
         */
        bool notify_task_finish = false;

        /**
         * A counter of the number of tasks in-progress by worker threads.
         * Incremented when a task is popped off the task queue and decremented when that task is complete.
         *
         * Access protected by task_mutex.
         */
        int num_inflight_tasks = 0;
    };
}

// clean up
#undef TTP_NODISCARD
#undef TTP_CXX17

#endif

#ifndef POOLSTL_INTERNAL_UTILS_HPP
#define POOLSTL_INTERNAL_UTILS_HPP

// Version macros.
#define POOLSTL_VERSION_MAJOR 0
#define POOLSTL_VERSION_MINOR 3
#define POOLSTL_VERSION_PATCH 5

#include <cstddef>
#include <iterator>

#if __cplusplus >= 201703L || (defined(_MSVC_LANG) && _MSVC_LANG >= 201703L)
#define POOLSTL_HAVE_CXX17 1
#define POOLSTL_NO_DISCARD [[nodiscard]]
#else
#define POOLSTL_HAVE_CXX17 0
#define POOLSTL_NO_DISCARD
#endif

#if POOLSTL_HAVE_CXX17 && (!defined(_GLIBCXX_RELEASE) || _GLIBCXX_RELEASE >= 9)
#define POOLSTL_HAVE_CXX17_LIB 1
#else
#define POOLSTL_HAVE_CXX17_LIB 0
#endif

#if __cplusplus >= 201402L || (defined(_MSVC_LANG) && _MSVC_LANG >= 201402L)
#define POOLSTL_HAVE_CXX14 1
#else
#define POOLSTL_HAVE_CXX14 0
#endif

namespace poolstl {
    namespace internal {

        inline constexpr std::size_t get_chunk_size(std::size_t num_steps, unsigned int num_threads) {
            return (num_steps / num_threads) + ((num_steps % num_threads) > 0 ? 1 : 0);
        }

        template<typename Iterator>
        constexpr typename std::iterator_traits<Iterator>::difference_type
        get_chunk_size(Iterator first, Iterator last, unsigned int num_threads) {
            using diff_t = typename std::iterator_traits<Iterator>::difference_type;
            return static_cast<diff_t>(get_chunk_size((std::size_t)std::distance(first, last), num_threads));
        }

        template<typename Iterator>
        constexpr typename std::iterator_traits<Iterator>::difference_type
        get_iter_chunk_size(const Iterator& iter, const Iterator& last,
                            typename std::iterator_traits<Iterator>::difference_type chunk_size) {
            return std::min(chunk_size, std::distance(iter, last));
        }

        template<typename Iterator>
        Iterator advanced(Iterator iter, typename std::iterator_traits<Iterator>::difference_type offset) {
            Iterator ret = iter;
            std::advance(ret, offset);
            return ret;
        }

        /**
         * An iterator wrapper that calls std::future<>::get().
         * @tparam Iterator
         */
        template<typename Iterator>
        class getting_iter : public Iterator {
        public:
            using value_type = decltype((*std::declval<Iterator>()).get());
            using difference_type = typename std::iterator_traits<Iterator>::difference_type;
            using pointer = value_type*;
            using reference = value_type&;
            explicit getting_iter(Iterator iter) : iter(iter) {}

            getting_iter operator++() { ++iter; return *this; }
            getting_iter operator++(int) { getting_iter ret(*this); ++iter; return ret; }

            value_type operator*() { return (*iter).get(); }
            value_type operator[](difference_type offset) { return iter[offset].get(); }

            bool operator==(const getting_iter<Iterator> &other) const { return iter == other.iter; }
            bool operator!=(const getting_iter<Iterator> &other) const { return iter != other.iter; }

        protected:
            Iterator iter;
        };

        template<typename Iterator>
        getting_iter<Iterator> get_wrap(Iterator iter) {
            return getting_iter<Iterator>(iter);
        }

        template <class Container>
        void get_futures(Container& futures) {
            for (auto &future: futures) {
                future.get();
            }
        }

        /**
         * Identify a pivot element for quicksort. Chooses the middle element of the range.
         */
        template <typename Iterator>
        typename std::iterator_traits<Iterator>::value_type quicksort_pivot(Iterator first, Iterator last) {
            return *(std::next(first, std::distance(first, last) / 2));
        }

        /**
         * Predicate for std::partition (for quicksort)
         */
        template <class Compare, class T>
        struct pivot_predicate {
            pivot_predicate(Compare comp, const T& pivot) : comp(comp), pivot(pivot) {}

            bool operator()(const T& em) {
                return comp(em, pivot);
            }
            Compare comp;
            const T pivot;
        };

        /*
         * Some methods are only available with C++17 and up. Reimplement on older standards.
         */
#if POOLSTL_HAVE_CXX17_LIB
        namespace cpp17 = std;
#else
        namespace cpp17 {

            // std::reduce

            template<class InputIt, class Tp, class BinOp>
            Tp reduce(InputIt first, InputIt last, Tp init, BinOp b) {
                for (; first != last; ++first)
                    init = b(init, *first);
                return init;
            }

            template<class InputIt>
            typename std::iterator_traits<InputIt>::value_type reduce(InputIt first, InputIt last) {
                return reduce(first, last,
                              typename std::iterator_traits<InputIt>::value_type{},
                              std::plus<typename std::iterator_traits<InputIt>::value_type>());
            }

            // std::transform

            template<class InputIt, class OutputIt, class UnaryOperation>
            OutputIt transform(InputIt first1, InputIt last1, OutputIt d_first,
                               UnaryOperation unary_op) {
                while (first1 != last1) {
                    *d_first++ = unary_op(*first1++);
                }

                return d_first;
            }

            template<class InputIt1, class InputIt2, class OutputIt, class BinaryOperation>
            OutputIt transform(InputIt1 first1, InputIt1 last1,
                               InputIt2 first2, OutputIt d_first,
                               BinaryOperation binary_op) {
                while (first1 != last1) {
                    *d_first++ = binary_op(*first1++, *first2++);
                }

                return d_first;
            }
        }
#endif
    }
}

#endif

namespace poolstl {

    namespace ttp = task_thread_pool;

    namespace execution {
        namespace internal {
            /**
             * Holds the thread pool used by par.
             */
            inline std::shared_ptr<ttp::task_thread_pool> get_default_pool() {
                static std::shared_ptr<ttp::task_thread_pool> pool;
                static std::once_flag flag;
                std::call_once(flag, [&](){ pool = std::make_shared<ttp::task_thread_pool>(); });
                return pool;
            }
        }

        /**
         * Base class for all poolSTL policies.
         */
        struct poolstl_policy {
        };

        /**
         * A sequential policy that simply forwards to the non-policy overload.
         */
        struct sequenced_policy : public poolstl_policy {
            POOLSTL_NO_DISCARD ttp::task_thread_pool* pool() const {
                // never called, but must exist for C++11 support
                throw std::runtime_error("poolSTL: requested thread pool for seq policy.");
            }

            POOLSTL_NO_DISCARD bool par_allowed() const {
                return false;
            }
        };

        /**
         * A parallel policy that can use a user-specified thread pool or a default one.
         */
        struct parallel_policy : public poolstl_policy {
            parallel_policy() = default;
            explicit parallel_policy(ttp::task_thread_pool* on_pool, bool par_ok): on_pool(on_pool), par_ok(par_ok) {}

            parallel_policy on(ttp::task_thread_pool& pool) const {
                return parallel_policy{&pool, par_ok};
            }

            parallel_policy par_if(bool call_par) const {
                return parallel_policy{on_pool, call_par};
            }

            POOLSTL_NO_DISCARD ttp::task_thread_pool* pool() const {
                if (on_pool) {
                    return on_pool;
                } else {
                    return internal::get_default_pool().get();
                }
            }

            POOLSTL_NO_DISCARD bool par_allowed() const {
                return par_ok;
            }

        protected:
            ttp::task_thread_pool *on_pool = nullptr;
            bool par_ok = true;
        };

        constexpr sequenced_policy seq{};
        constexpr parallel_policy par{};

        /**
         * EXPERIMENTAL: Subject to significant changes or removal.
         * Use pure threads for each operation instead of a shared thread pool.
         *
         * Advantage:
         *  - Fewer symbols (no packaged_task with its operators, destructors, vtable, etc) means smaller binary
         *    which can mean a lot when there are many calls.
         *  - No thread pool to manage.
         *
         * Disadvantages:
         *  - Threads are started and joined for every operation, so it is harder to amortize that cost.
         *  - Barely any algorithms are supported.
         */
        struct pure_threads_policy : public poolstl_policy {
            explicit pure_threads_policy(unsigned int num_threads, bool par_ok): num_threads(num_threads),
                                                                                 par_ok(par_ok) {}

            POOLSTL_NO_DISCARD unsigned int get_num_threads() const {
                if (num_threads == 0) {
                    return std::thread::hardware_concurrency();
                }
                return num_threads;
            }

            POOLSTL_NO_DISCARD bool par_allowed() const {
                return par_ok;
            }

        protected:
            unsigned int num_threads = 1;
            bool par_ok = true;
        };

        /**
         * Choose parallel or sequential at runtime.
         *
         * @param call_par Whether to use a parallel policy.
         * @return `par` if call_par is true, else a sequential policy (like `seq`).
         */
        inline parallel_policy par_if(bool call_par) {
            return parallel_policy{nullptr, call_par};
        }

        /**
         * Choose parallel or sequential at runtime, with pool selection.
         *
         * @param call_par Whether to use a parallel policy.
         * @return `par.on(pool)` if call_par is true, else a sequential policy (like `seq`).
         */
        inline parallel_policy par_if(bool call_par, ttp::task_thread_pool& pool) {
            return parallel_policy{&pool, call_par};
        }

        /**
         * EXPERIMENTAL: Subject to significant changes or removal. See `pure_threads_policy`.
         * Choose parallel or sequential at runtime, with thread count selection.
         *
         * @param call_par Whether to use a parallel policy.
         * @return `par.on(pool)` if call_par is true, else `seq`.
         */
        inline pure_threads_policy par_if_threads(bool call_par, unsigned int num_threads) {
            return pure_threads_policy{num_threads, call_par};
        }
    }

    using execution::seq;
    using execution::par;
    using execution::par_if;

    namespace internal {
        /**
         * To enable/disable seq overload resolution
         */
        template <class ExecPolicy, class Tp>
        using enable_if_seq =
            typename std::enable_if<
                std::is_same<poolstl::execution::sequenced_policy,
                    typename std::remove_cv<typename std::remove_reference<ExecPolicy>::type>::type>::value,
                Tp>::type;

        /**
         * To enable/disable par overload resolution
         */
        template <class ExecPolicy, class Tp>
        using enable_if_par =
            typename std::enable_if<
                std::is_same<poolstl::execution::parallel_policy,
                    typename std::remove_cv<typename std::remove_reference<ExecPolicy>::type>::type>::value,
                Tp>::type;

        /**
         * To enable/disable par overload resolution
         */
        template <class ExecPolicy, class Tp>
        using enable_if_poolstl_policy =
            typename std::enable_if<
                std::is_base_of<poolstl::execution::poolstl_policy,
                    typename std::remove_cv<typename std::remove_reference<ExecPolicy>::type>::type>::value,
                Tp>::type;

        template <class ExecPolicy>
        bool is_seq(const ExecPolicy& policy) {
            return !policy.par_allowed();
        }

        template <class ExecPolicy>
        using is_pure_threads_policy = std::is_same<poolstl::execution::pure_threads_policy,
            typename std::remove_cv<typename std::remove_reference<ExecPolicy>::type>::type>;
    }
}

#endif

#ifndef POOLSTL_ALGORITHM_HPP
#define POOLSTL_ALGORITHM_HPP

#include <functional>


#ifndef POOLSTL_INTERNAL_TTP_IMPL_HPP
#define POOLSTL_INTERNAL_TTP_IMPL_HPP

#include <algorithm>
#include <numeric>
#include <utility>
#include <vector>


#ifndef POOLSTL_EXECUTION_HPP
#define POOLSTL_EXECUTION_HPP

#include <memory>
#include <mutex>
#include <stdexcept>
#include <type_traits>


#ifndef AL_TASK_THREAD_POOL_HPP
#define AL_TASK_THREAD_POOL_HPP

// Version macros.
#define TASK_THREAD_POOL_VERSION_MAJOR 1
#define TASK_THREAD_POOL_VERSION_MINOR 0
#define TASK_THREAD_POOL_VERSION_PATCH 10

#include <condition_variable>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <type_traits>

// MSVC does not correctly set the __cplusplus macro by default, so we must read it from _MSVC_LANG
// See https://devblogs.microsoft.com/cppblog/msvc-now-correctly-reports-__cplusplus/
#if __cplusplus >= 201703L || (defined(_MSVC_LANG) && _MSVC_LANG >= 201703L)
#define TTP_CXX17 1
#else
#define TTP_CXX17 0
#endif

#if TTP_CXX17
#define TTP_NODISCARD [[nodiscard]]
#else
#define TTP_NODISCARD
#endif

namespace task_thread_pool {

#if !TTP_CXX17
    /**
     * A reimplementation of std::decay_t, which is only available since C++14.
     */
    template <class T>
    using decay_t = typename std::decay<T>::type;
#endif

    /**
     * A fast and lightweight thread pool that uses C++11 threads.
     */
    class task_thread_pool {
    public:
        /**
         * Create a task_thread_pool and start worker threads.
         *
         * @param num_threads Number of worker threads. If 0 then number of threads is equal to the
         *                    number of physical cores on the machine, as given by std::thread::hardware_concurrency().
         */
        explicit task_thread_pool(unsigned int num_threads = 0) {
            if (num_threads < 1) {
                num_threads = std::thread::hardware_concurrency();
                if (num_threads < 1) { num_threads = 1; }
            }
            start_threads(num_threads);
        }

        /**
         * Finish all tasks left in the queue then shut down worker threads.
         * If the pool is currently paused then it is resumed.
         */
        ~task_thread_pool() {
            unpause();
            wait_for_queued_tasks();
            stop_all_threads();
        }

        /**
         * Drop all tasks that have been submitted but not yet started by a worker.
         *
         * Tasks already in progress continue executing.
         */
        void clear_task_queue() {
            const std::lock_guard<std::mutex> tasks_lock(task_mutex);
            tasks = {};
        }

        /**
         * Get number of enqueued tasks.
         *
         * @return Number of tasks that have been enqueued but not yet started.
         */
        TTP_NODISCARD size_t get_num_queued_tasks() const {
            const std::lock_guard<std::mutex> tasks_lock(task_mutex);
            return tasks.size();
        }

        /**
         * Get number of in-progress tasks.
         *
         * @return Approximate number of tasks currently being processed by worker threads.
         */
        TTP_NODISCARD size_t get_num_running_tasks() const {
            const std::lock_guard<std::mutex> tasks_lock(task_mutex);
            return num_inflight_tasks;
        }

        /**
         * Get total number of tasks in the pool.
         *
         * @return Approximate number of tasks both enqueued and running.
         */
        TTP_NODISCARD size_t get_num_tasks() const {
            const std::lock_guard<std::mutex> tasks_lock(task_mutex);
            return tasks.size() + num_inflight_tasks;
        }

        /**
         * Get number of worker threads.
         *
         * @return Number of worker threads.
         */
        TTP_NODISCARD unsigned int get_num_threads() const {
            const std::lock_guard<std::recursive_mutex> threads_lock(thread_mutex);
            return static_cast<unsigned int>(threads.size());
        }

        /**
         * Set number of worker threads. Will start or stop worker threads as necessary.
         *
         * @param num_threads Number of worker threads. If 0 then number of threads is equal to the
         *                    number of physical cores on the machine, as given by std::thread::hardware_concurrency().
         * @return Previous number of worker threads.
         */
        unsigned int set_num_threads(unsigned int num_threads) {
            const std::lock_guard<std::recursive_mutex> threads_lock(thread_mutex);
            unsigned int previous_num_threads = get_num_threads();

            if (num_threads < 1) {
                num_threads = std::thread::hardware_concurrency();
                if (num_threads < 1) { num_threads = 1; }
            }

            if (previous_num_threads <= num_threads) {
                // expanding the thread pool
                start_threads(num_threads - previous_num_threads);
            } else {
                // contracting the thread pool
                stop_all_threads();
                {
                    const std::lock_guard<std::mutex> tasks_lock(task_mutex);
                    pool_running = true;
                }
                start_threads(num_threads);
            }

            return previous_num_threads;
        }

        /**
         * Stop executing queued tasks. Use `unpause()` to resume. Note: Destroying the pool will implicitly unpause.
         *
         * Any in-progress tasks continue executing.
         */
        void pause() {
            const std::lock_guard<std::mutex> tasks_lock(task_mutex);
            pool_paused = true;
        }

        /**
         * Resume executing queued tasks.
         */
        void unpause() {
            const std::lock_guard<std::mutex> tasks_lock(task_mutex);
            pool_paused = false;
            task_cv.notify_all();
        }

        /**
         * Check whether the pool is paused.
         *
         * @return true if pause() has been called without an intervening unpause().
         */
        TTP_NODISCARD bool is_paused() const {
            const std::lock_guard<std::mutex> tasks_lock(task_mutex);
            return pool_paused;
        }

        /**
         * Submit a Callable for the pool to execute and return a std::future.
         *
         * @param func The Callable to execute. Can be a function, a lambda, std::packaged_task, std::function, etc.
         * @param args Arguments for func. Optional.
         * @return std::future that can be used to get func's return value or thrown exception.
         */
        template <typename F, typename... A,
#if TTP_CXX17
            typename R = std::invoke_result_t<std::decay_t<F>, std::decay_t<A>...>
#else
            typename R = typename std::result_of<decay_t<F>(decay_t<A>...)>::type
#endif
        >
        TTP_NODISCARD std::future<R> submit(F&& func, A&&... args) {
#if defined(_MSC_VER)
            // MSVC's packaged_task is not movable even though it should be.
            // Discussion about this bug and its future fix:
            // https://developercommunity.visualstudio.com/t/unable-to-move-stdpackaged-task-into-any-stl-conta/108672
            std::shared_ptr<std::packaged_task<R()>> ptask =
                std::make_shared<std::packaged_task<R()>>(std::bind(std::forward<F>(func), std::forward<A>(args)...));
            submit_detach([ptask] { (*ptask)(); });
            return ptask->get_future();
#else
            std::packaged_task<R()> task(std::bind(std::forward<F>(func), std::forward<A>(args)...));
            auto ret = task.get_future();
            submit_detach(std::move(task));
            return ret;
#endif
        }

        /**
         * Submit a zero-argument Callable for the pool to execute.
         *
         * @param func The Callable to execute. Can be a function, a lambda, std::packaged_task, std::function, etc.
         */
        template <typename F>
        void submit_detach(F&& func) {
            const std::lock_guard<std::mutex> tasks_lock(task_mutex);
            tasks.emplace(std::forward<F>(func));
            task_cv.notify_one();
        }

        /**
         * Submit a Callable with arguments for the pool to execute.
         *
         * @param func The Callable to execute. Can be a function, a lambda, std::packaged_task, std::function, etc.
         */
        template <typename F, typename... A>
        void submit_detach(F&& func, A&&... args) {
            const std::lock_guard<std::mutex> tasks_lock(task_mutex);
            tasks.emplace(std::bind(std::forward<F>(func), std::forward<A>(args)...));
            task_cv.notify_one();
        }

        /**
         * Block until the task queue is empty. Some tasks may be in-progress when this method returns.
         */
        void wait_for_queued_tasks() {
            std::unique_lock<std::mutex> tasks_lock(task_mutex);
            notify_task_finish = true;
            task_finished_cv.wait(tasks_lock, [&] { return tasks.empty(); });
            notify_task_finish = false;
        }

        /**
         * Block until all tasks have finished.
         */
        void wait_for_tasks() {
            std::unique_lock<std::mutex> tasks_lock(task_mutex);
            notify_task_finish = true;
            task_finished_cv.wait(tasks_lock, [&] { return tasks.empty() && num_inflight_tasks == 0; });
            notify_task_finish = false;
        }

    protected:

        /**
         * Main function for worker threads.
         */
        void worker_main() {
            bool finished_task = false;

            while (true) {
                std::unique_lock<std::mutex> tasks_lock(task_mutex);

                if (finished_task) {
                    --num_inflight_tasks;
                    if (notify_task_finish) {
                        task_finished_cv.notify_all();
                    }
                }

                task_cv.wait(tasks_lock, [&]() { return !pool_running || (!pool_paused && !tasks.empty()); });

                if (!pool_running) {
                    break;
                }

                // Must mean that (!pool_paused && !tasks.empty()) is true

                std::packaged_task<void()> task{std::move(tasks.front())};
                tasks.pop();
                ++num_inflight_tasks;
                tasks_lock.unlock();

                try {
                    task();
                } catch (...) {
                    // std::packaged_task::operator() may throw in some error conditions, such as if the task
                    // had already been run. Nothing that the pool can do anything about.
                }

                finished_task = true;
            }
        }

        /**
         * Start worker threads.
         *
         * @param num_threads How many threads to start.
         */
        void start_threads(const unsigned int num_threads) {
            const std::lock_guard<std::recursive_mutex> threads_lock(thread_mutex);

            for (unsigned int i = 0; i < num_threads; ++i) {
                threads.emplace_back(&task_thread_pool::worker_main, this);
            }
        }

        /**
         * Stop, join, and destroy all worker threads.
         */
        void stop_all_threads() {
            const std::lock_guard<std::recursive_mutex> threads_lock(thread_mutex);

            {
                const std::lock_guard<std::mutex> tasks_lock(task_mutex);
                pool_running = false;
                task_cv.notify_all();
            }

            for (auto& thread : threads) {
                if (thread.joinable()) {
                    thread.join();
                }
            }
            threads.clear();
        }

        /**
         * The worker threads.
         *
         * Access protected by thread_mutex
         */
        std::vector<std::thread> threads;

        /**
         * A mutex for methods that start/stop threads.
         */
        mutable std::recursive_mutex thread_mutex;

        /**
         * The task queue.
         *
         * Access protected by task_mutex.
         */
        std::queue<std::packaged_task<void()>> tasks = {};

        /**
         * A mutex for all variables related to tasks.
         */
        mutable std::mutex task_mutex;

        /**
         * Used to notify changes to the task queue, such as a new task added, pause/unpause, etc.
         */
        std::condition_variable task_cv;

        /**
         * Used to notify of finished tasks.
         */
        std::condition_variable task_finished_cv;

        /**
         * A signal for worker threads that the pool is either running or shutting down.
         *
         * Access protected by task_mutex.
         */
        bool pool_running = true;

        /**
         * A signal for worker threads to not pull new tasks from the queue.
         *
         * Access protected by task_mutex.
         */
        bool pool_paused = false;

        /**
         * A signal for worker threads that they should notify task_finished_cv when they finish a task.
         *
         * Access protected by task_mutex.
         */
        bool notify_task_finish = false;

        /**
         * A counter of the number of tasks in-progress by worker threads.
         * Incremented when a task is popped off the task queue and decremented when that task is complete.
         *
         * Access protected by task_mutex.
         */
        int num_inflight_tasks = 0;
    };
}

// clean up
#undef TTP_NODISCARD
#undef TTP_CXX17

#endif

#ifndef POOLSTL_INTERNAL_UTILS_HPP
#define POOLSTL_INTERNAL_UTILS_HPP

// Version macros.
#define POOLSTL_VERSION_MAJOR 0
#define POOLSTL_VERSION_MINOR 3
#define POOLSTL_VERSION_PATCH 5

#include <cstddef>
#include <iterator>

#if __cplusplus >= 201703L || (defined(_MSVC_LANG) && _MSVC_LANG >= 201703L)
#define POOLSTL_HAVE_CXX17 1
#define POOLSTL_NO_DISCARD [[nodiscard]]
#else
#define POOLSTL_HAVE_CXX17 0
#define POOLSTL_NO_DISCARD
#endif

#if POOLSTL_HAVE_CXX17 && (!defined(_GLIBCXX_RELEASE) || _GLIBCXX_RELEASE >= 9)
#define POOLSTL_HAVE_CXX17_LIB 1
#else
#define POOLSTL_HAVE_CXX17_LIB 0
#endif

#if __cplusplus >= 201402L || (defined(_MSVC_LANG) && _MSVC_LANG >= 201402L)
#define POOLSTL_HAVE_CXX14 1
#else
#define POOLSTL_HAVE_CXX14 0
#endif

namespace poolstl {
    namespace internal {

        inline constexpr std::size_t get_chunk_size(std::size_t num_steps, unsigned int num_threads) {
            return (num_steps / num_threads) + ((num_steps % num_threads) > 0 ? 1 : 0);
        }

        template<typename Iterator>
        constexpr typename std::iterator_traits<Iterator>::difference_type
        get_chunk_size(Iterator first, Iterator last, unsigned int num_threads) {
            using diff_t = typename std::iterator_traits<Iterator>::difference_type;
            return static_cast<diff_t>(get_chunk_size((std::size_t)std::distance(first, last), num_threads));
        }

        template<typename Iterator>
        constexpr typename std::iterator_traits<Iterator>::difference_type
        get_iter_chunk_size(const Iterator& iter, const Iterator& last,
                            typename std::iterator_traits<Iterator>::difference_type chunk_size) {
            return std::min(chunk_size, std::distance(iter, last));
        }

        template<typename Iterator>
        Iterator advanced(Iterator iter, typename std::iterator_traits<Iterator>::difference_type offset) {
            Iterator ret = iter;
            std::advance(ret, offset);
            return ret;
        }

        /**
         * An iterator wrapper that calls std::future<>::get().
         * @tparam Iterator
         */
        template<typename Iterator>
        class getting_iter : public Iterator {
        public:
            using value_type = decltype((*std::declval<Iterator>()).get());
            using difference_type = typename std::iterator_traits<Iterator>::difference_type;
            using pointer = value_type*;
            using reference = value_type&;
            explicit getting_iter(Iterator iter) : iter(iter) {}

            getting_iter operator++() { ++iter; return *this; }
            getting_iter operator++(int) { getting_iter ret(*this); ++iter; return ret; }

            value_type operator*() { return (*iter).get(); }
            value_type operator[](difference_type offset) { return iter[offset].get(); }

            bool operator==(const getting_iter<Iterator> &other) const { return iter == other.iter; }
            bool operator!=(const getting_iter<Iterator> &other) const { return iter != other.iter; }

        protected:
            Iterator iter;
        };

        template<typename Iterator>
        getting_iter<Iterator> get_wrap(Iterator iter) {
            return getting_iter<Iterator>(iter);
        }

        template <class Container>
        void get_futures(Container& futures) {
            for (auto &future: futures) {
                future.get();
            }
        }

        /**
         * Identify a pivot element for quicksort. Chooses the middle element of the range.
         */
        template <typename Iterator>
        typename std::iterator_traits<Iterator>::value_type quicksort_pivot(Iterator first, Iterator last) {
            return *(std::next(first, std::distance(first, last) / 2));
        }

        /**
         * Predicate for std::partition (for quicksort)
         */
        template <class Compare, class T>
        struct pivot_predicate {
            pivot_predicate(Compare comp, const T& pivot) : comp(comp), pivot(pivot) {}

            bool operator()(const T& em) {
                return comp(em, pivot);
            }
            Compare comp;
            const T pivot;
        };

        /*
         * Some methods are only available with C++17 and up. Reimplement on older standards.
         */
#if POOLSTL_HAVE_CXX17_LIB
        namespace cpp17 = std;
#else
        namespace cpp17 {

            // std::reduce

            template<class InputIt, class Tp, class BinOp>
            Tp reduce(InputIt first, InputIt last, Tp init, BinOp b) {
                for (; first != last; ++first)
                    init = b(init, *first);
                return init;
            }

            template<class InputIt>
            typename std::iterator_traits<InputIt>::value_type reduce(InputIt first, InputIt last) {
                return reduce(first, last,
                              typename std::iterator_traits<InputIt>::value_type{},
                              std::plus<typename std::iterator_traits<InputIt>::value_type>());
            }

            // std::transform

            template<class InputIt, class OutputIt, class UnaryOperation>
            OutputIt transform(InputIt first1, InputIt last1, OutputIt d_first,
                               UnaryOperation unary_op) {
                while (first1 != last1) {
                    *d_first++ = unary_op(*first1++);
                }

                return d_first;
            }

            template<class InputIt1, class InputIt2, class OutputIt, class BinaryOperation>
            OutputIt transform(InputIt1 first1, InputIt1 last1,
                               InputIt2 first2, OutputIt d_first,
                               BinaryOperation binary_op) {
                while (first1 != last1) {
                    *d_first++ = binary_op(*first1++, *first2++);
                }

                return d_first;
            }
        }
#endif
    }
}

#endif

namespace poolstl {

    namespace ttp = task_thread_pool;

    namespace execution {
        namespace internal {
            /**
             * Holds the thread pool used by par.
             */
            inline std::shared_ptr<ttp::task_thread_pool> get_default_pool() {
                static std::shared_ptr<ttp::task_thread_pool> pool;
                static std::once_flag flag;
                std::call_once(flag, [&](){ pool = std::make_shared<ttp::task_thread_pool>(); });
                return pool;
            }
        }

        /**
         * Base class for all poolSTL policies.
         */
        struct poolstl_policy {
        };

        /**
         * A sequential policy that simply forwards to the non-policy overload.
         */
        struct sequenced_policy : public poolstl_policy {
            POOLSTL_NO_DISCARD ttp::task_thread_pool* pool() const {
                // never called, but must exist for C++11 support
                throw std::runtime_error("poolSTL: requested thread pool for seq policy.");
            }

            POOLSTL_NO_DISCARD bool par_allowed() const {
                return false;
            }
        };

        /**
         * A parallel policy that can use a user-specified thread pool or a default one.
         */
        struct parallel_policy : public poolstl_policy {
            parallel_policy() = default;
            explicit parallel_policy(ttp::task_thread_pool* on_pool, bool par_ok): on_pool(on_pool), par_ok(par_ok) {}

            parallel_policy on(ttp::task_thread_pool& pool) const {
                return parallel_policy{&pool, par_ok};
            }

            parallel_policy par_if(bool call_par) const {
                return parallel_policy{on_pool, call_par};
            }

            POOLSTL_NO_DISCARD ttp::task_thread_pool* pool() const {
                if (on_pool) {
                    return on_pool;
                } else {
                    return internal::get_default_pool().get();
                }
            }

            POOLSTL_NO_DISCARD bool par_allowed() const {
                return par_ok;
            }

        protected:
            ttp::task_thread_pool *on_pool = nullptr;
            bool par_ok = true;
        };

        constexpr sequenced_policy seq{};
        constexpr parallel_policy par{};

        /**
         * EXPERIMENTAL: Subject to significant changes or removal.
         * Use pure threads for each operation instead of a shared thread pool.
         *
         * Advantage:
         *  - Fewer symbols (no packaged_task with its operators, destructors, vtable, etc) means smaller binary
         *    which can mean a lot when there are many calls.
         *  - No thread pool to manage.
         *
         * Disadvantages:
         *  - Threads are started and joined for every operation, so it is harder to amortize that cost.
         *  - Barely any algorithms are supported.
         */
        struct pure_threads_policy : public poolstl_policy {
            explicit pure_threads_policy(unsigned int num_threads, bool par_ok): num_threads(num_threads),
                                                                                 par_ok(par_ok) {}

            POOLSTL_NO_DISCARD unsigned int get_num_threads() const {
                if (num_threads == 0) {
                    return std::thread::hardware_concurrency();
                }
                return num_threads;
            }

            POOLSTL_NO_DISCARD bool par_allowed() const {
                return par_ok;
            }

        protected:
            unsigned int num_threads = 1;
            bool par_ok = true;
        };

        /**
         * Choose parallel or sequential at runtime.
         *
         * @param call_par Whether to use a parallel policy.
         * @return `par` if call_par is true, else a sequential policy (like `seq`).
         */
        inline parallel_policy par_if(bool call_par) {
            return parallel_policy{nullptr, call_par};
        }

        /**
         * Choose parallel or sequential at runtime, with pool selection.
         *
         * @param call_par Whether to use a parallel policy.
         * @return `par.on(pool)` if call_par is true, else a sequential policy (like `seq`).
         */
        inline parallel_policy par_if(bool call_par, ttp::task_thread_pool& pool) {
            return parallel_policy{&pool, call_par};
        }

        /**
         * EXPERIMENTAL: Subject to significant changes or removal. See `pure_threads_policy`.
         * Choose parallel or sequential at runtime, with thread count selection.
         *
         * @param call_par Whether to use a parallel policy.
         * @return `par.on(pool)` if call_par is true, else `seq`.
         */
        inline pure_threads_policy par_if_threads(bool call_par, unsigned int num_threads) {
            return pure_threads_policy{num_threads, call_par};
        }
    }

    using execution::seq;
    using execution::par;
    using execution::par_if;

    namespace internal {
        /**
         * To enable/disable seq overload resolution
         */
        template <class ExecPolicy, class Tp>
        using enable_if_seq =
            typename std::enable_if<
                std::is_same<poolstl::execution::sequenced_policy,
                    typename std::remove_cv<typename std::remove_reference<ExecPolicy>::type>::type>::value,
                Tp>::type;

        /**
         * To enable/disable par overload resolution
         */
        template <class ExecPolicy, class Tp>
        using enable_if_par =
            typename std::enable_if<
                std::is_same<poolstl::execution::parallel_policy,
                    typename std::remove_cv<typename std::remove_reference<ExecPolicy>::type>::type>::value,
                Tp>::type;

        /**
         * To enable/disable par overload resolution
         */
        template <class ExecPolicy, class Tp>
        using enable_if_poolstl_policy =
            typename std::enable_if<
                std::is_base_of<poolstl::execution::poolstl_policy,
                    typename std::remove_cv<typename std::remove_reference<ExecPolicy>::type>::type>::value,
                Tp>::type;

        template <class ExecPolicy>
        bool is_seq(const ExecPolicy& policy) {
            return !policy.par_allowed();
        }

        template <class ExecPolicy>
        using is_pure_threads_policy = std::is_same<poolstl::execution::pure_threads_policy,
            typename std::remove_cv<typename std::remove_reference<ExecPolicy>::type>::type>;
    }
}

#endif

namespace poolstl {
    namespace internal {

#if POOLSTL_HAVE_CXX17_LIB
        /**
         * Call std::apply in parallel.
         */
        template <class ExecPolicy, class Op, class ArgContainer>
        std::vector<std::future<void>>
        parallel_apply(ExecPolicy &&policy, Op op, const ArgContainer& args_list) {
            std::vector<std::future<void>> futures;
            auto& task_pool = *policy.pool();

            for (const auto& args : args_list) {
                futures.emplace_back(task_pool.submit([](Op op, const auto& args_fwd) {
                        std::apply(op, args_fwd);
                    }, op, args));
            }

            return futures;
        }
#endif

        /**
         * Chunk a single range, with autodetected return types.
         */
        template <class ExecPolicy, class RandIt, class Chunk,
            class ChunkRet = decltype(std::declval<Chunk>()(std::declval<RandIt>(), std::declval<RandIt>()))>
        std::vector<std::future<ChunkRet>>
        parallel_chunk_for_gen(ExecPolicy &&policy, RandIt first, RandIt last, Chunk chunk,
                               ChunkRet* = (decltype(std::declval<Chunk>()(std::declval<RandIt>(),
                                                     std::declval<RandIt>()))*)nullptr,
                               int extra_split_factor = 1) {
            std::vector<std::future<ChunkRet>> futures;
            auto& task_pool = *policy.pool();
            auto chunk_size = get_chunk_size(first, last, extra_split_factor * task_pool.get_num_threads());

            while (first < last) {
                auto iter_chunk_size = get_iter_chunk_size(first, last, chunk_size);
                RandIt loop_end = advanced(first, iter_chunk_size);

                futures.emplace_back(task_pool.submit(chunk, first, loop_end));

                first = loop_end;
            }

            return futures;
        }

        /**
         * Chunk a single range.
         */
        template <class ExecPolicy, class RandIt, class Chunk, class ChunkRet, typename... A>
        std::vector<std::future<ChunkRet>>
        parallel_chunk_for_1(ExecPolicy &&policy, RandIt first, RandIt last,
                             Chunk chunk, ChunkRet*, int extra_split_factor, A&&... chunk_args) {
            std::vector<std::future<ChunkRet>> futures;
            auto& task_pool = *policy.pool();
            auto chunk_size = get_chunk_size(first, last, extra_split_factor * task_pool.get_num_threads());

            while (first < last) {
                auto iter_chunk_size = get_iter_chunk_size(first, last, chunk_size);
                RandIt loop_end = advanced(first, iter_chunk_size);

                futures.emplace_back(task_pool.submit(chunk, first, loop_end, chunk_args...));

                first = loop_end;
            }

            return futures;
        }

        /**
         * Chunk a single range.
         */
        template <class ExecPolicy, class RandIt, class Chunk, class ChunkRet, typename... A>
        typename std::enable_if<!is_pure_threads_policy<ExecPolicy>::value, void>::type
        parallel_chunk_for_1_wait(ExecPolicy &&policy, RandIt first, RandIt last,
                                  Chunk chunk, ChunkRet* rettype, int extra_split_factor, A&&... chunk_args) {
            auto futures = parallel_chunk_for_1(std::forward<ExecPolicy>(policy), first, last,
                                                chunk, rettype, extra_split_factor, chunk_args...);
            get_futures(futures);
        }

        /**
         * Element-wise chunk two ranges.
         */
        template <class ExecPolicy, class RandIt1, class RandIt2, class Chunk, class ChunkRet, typename... A>
        std::vector<std::future<ChunkRet>>
        parallel_chunk_for_2(ExecPolicy &&policy, RandIt1 first1, RandIt1 last1, RandIt2 first2,
                             Chunk chunk, ChunkRet*, A&&... chunk_args) {
            std::vector<std::future<ChunkRet>> futures;
            auto& task_pool = *policy.pool();
            auto chunk_size = get_chunk_size(first1, last1, task_pool.get_num_threads());

            while (first1 < last1) {
                auto iter_chunk_size = get_iter_chunk_size(first1, last1, chunk_size);
                RandIt1 loop_end = advanced(first1, iter_chunk_size);

                futures.emplace_back(task_pool.submit(chunk, first1, loop_end, first2, chunk_args...));

                first1 = loop_end;
                std::advance(first2, iter_chunk_size);
            }

            return futures;
        }

        /**
         * Element-wise chunk three ranges.
         */
        template <class ExecPolicy, class RandIt1, class RandIt2, class RandIt3,
                  class Chunk, class ChunkRet, typename... A>
        std::vector<std::future<ChunkRet>>
        parallel_chunk_for_3(ExecPolicy &&policy, RandIt1 first1, RandIt1 last1, RandIt2 first2, RandIt3 first3,
                           Chunk chunk, ChunkRet*, A&&... chunk_args) {
            std::vector<std::future<ChunkRet>> futures;
            auto& task_pool = *policy.pool();
            auto chunk_size = get_chunk_size(first1, last1, task_pool.get_num_threads());

            while (first1 < last1) {
                auto iter_chunk_size = get_iter_chunk_size(first1, last1, chunk_size);
                RandIt1 loop_end = advanced(first1, iter_chunk_size);

                futures.emplace_back(task_pool.submit(chunk, first1, loop_end, first2, first3, chunk_args...));

                first1 = loop_end;
                std::advance(first2, iter_chunk_size);
                std::advance(first3, iter_chunk_size);
            }

            return futures;
        }

        /**
         * Sort a range in parallel.
         *
         * @param sort_func Sequential sort method, like std::sort or std::stable_sort
         * @param merge_func Sequential merge method, like std::inplace_merge
         */
        template <class ExecPolicy, class RandIt, class Compare, class SortFunc, class MergeFunc>
        void parallel_mergesort(ExecPolicy &&policy, RandIt first, RandIt last,
                                Compare comp, SortFunc sort_func, MergeFunc merge_func) {
            if (first == last) {
                return;
            }

            // Sort chunks in parallel
            auto futures = parallel_chunk_for_gen(std::forward<ExecPolicy>(policy), first, last,
                             [&comp, sort_func] (RandIt chunk_first, RandIt chunk_last) {
                                 sort_func(chunk_first, chunk_last, comp);
                                 return std::make_pair(chunk_first, chunk_last);
                             });

            // Merge the sorted ranges
            using SortedRange = std::pair<RandIt, RandIt>;
            auto& task_pool = *policy.pool();
            std::vector<SortedRange> subranges;
            do {
                for (auto& future : futures) {
                    subranges.emplace_back(future.get());
                }
                futures.clear();

                for (std::size_t i = 0; i < subranges.size(); ++i) {
                    if (i + 1 < subranges.size()) {
                        // pair up and merge
                        auto& lhs = subranges[i];
                        auto& rhs = subranges[i + 1];
                        futures.emplace_back(task_pool.submit([&comp, merge_func] (RandIt chunk_first,
                                                                                   RandIt chunk_middle,
                                                                                   RandIt chunk_last) {
                            merge_func(chunk_first, chunk_middle, chunk_last, comp);
                            return std::make_pair(chunk_first, chunk_last);
                        }, lhs.first, lhs.second, rhs.second));
                        ++i;
                    } else {
                        // forward the final extra range
                        std::promise<SortedRange> p;
                        futures.emplace_back(p.get_future());
                        p.set_value(subranges[i]);
                    }
                }

                subranges.clear();
            } while (futures.size() > 1);
            futures.front().get();
        }

        /**
         * Quicksort worker function.
         */
        template <class RandIt, class Compare, class SortFunc, class PartFunc, class PivotFunc>
        void quicksort_impl(task_thread_pool::task_thread_pool* task_pool, const RandIt first, const RandIt last,
                            Compare comp, SortFunc sort_func, PartFunc part_func, PivotFunc pivot_func,
                            std::ptrdiff_t target_leaf_size,
                            std::vector<std::future<void>>* futures, std::mutex* mutex,
                            std::condition_variable* cv, int* inflight_spawns) {
            using T = typename std::iterator_traits<RandIt>::value_type;

            auto partition_size = std::distance(first, last);

            if (partition_size > target_leaf_size) {
                // partition the range
                auto mid = part_func(first, last, pivot_predicate<Compare, T>(comp, pivot_func(first, last)));

                if (mid != first && mid != last) {
                    // was able to partition the range, so recurse
                    std::lock_guard<std::mutex> guard(*mutex);
                    ++(*inflight_spawns);

                    futures->emplace_back(task_pool->submit(
                        quicksort_impl<RandIt, Compare, SortFunc, PartFunc, PivotFunc>,
                        task_pool, first, mid, comp, sort_func, part_func, pivot_func, target_leaf_size,
                        futures, mutex, cv, inflight_spawns));

                    futures->emplace_back(task_pool->submit(
                        quicksort_impl<RandIt, Compare, SortFunc, PartFunc, PivotFunc>,
                        task_pool, mid, last, comp, sort_func, part_func, pivot_func, target_leaf_size,
                        futures, mutex, cv, inflight_spawns));
                    return;
                }
            }

            // Range does not need to be subdivided (or was unable to subdivide). Run the sequential sort.
            {
                // notify main thread that partitioning may be finished
                std::lock_guard<std::mutex> guard(*mutex);
                --(*inflight_spawns);
            }
            cv->notify_one();

            sort_func(first, last, comp);
        }

        /**
         * Sort a range in parallel using quicksort.
         *
         * @param sort_func Sequential sort method, like std::sort or std::stable_sort
         * @param part_func Method that partitions a range, like std::partition or std::stable_partition
         * @param pivot_func Method that identifies the pivot
         */
        template <class ExecPolicy, class RandIt, class Compare, class SortFunc, class PartFunc, class PivotFunc>
        void parallel_quicksort(ExecPolicy &&policy, RandIt first, RandIt last,
                                Compare comp, SortFunc sort_func, PartFunc part_func, PivotFunc pivot_func) {
            if (first == last) {
                return;
            }

            auto& task_pool = *policy.pool();

            // Target partition size. Range will be recursively partitioned into partitions no bigger than this
            // size. Target approximately twice as many partitions as threads to reduce impact of uneven pivot
            // selection.
            auto num_threads = task_pool.get_num_threads();
            std::ptrdiff_t target_leaf_size = std::max(std::distance(first, last) / (num_threads * 2),
                                                       (std::ptrdiff_t)5);

            if (num_threads == 1) {
                target_leaf_size = std::distance(first, last);
            }

            // task_thread_pool does not support creating task DAGs, so organize the code such that
            // all parallel tasks are independent. The parallel tasks can spawn additional parallel tasks, and they
            // record their "child" task's std::future into a common vector to be waited on by the main thread.
            std::mutex mutex;

            // Futures of parallel tasks. Access protected by mutex.
            std::vector<std::future<void>> futures;

            // For signaling that all partitioning has been completed and futures vector is complete. Uses mutex.
            std::condition_variable cv;

            // Number of `quicksort_impl` calls that haven't finished yet. Nonzero value means futures vector may
            // still be modified. Access protected by mutex.
            int inflight_spawns = 1;

            // Root task.
            quicksort_impl(&task_pool, first, last, comp, sort_func, part_func, pivot_func, target_leaf_size,
                           &futures, &mutex, &cv, &inflight_spawns);

            // Wait for all partitioning to finish.
            {
                std::unique_lock<std::mutex> lock(mutex);
                cv.wait(lock, [&] { return inflight_spawns == 0; });
            }

            // Wait on all the parallel tasks.
            get_futures(futures);
        }

        /**
         * Partition range according to predicate. Unstable.
         *
         * This implementation only parallelizes with p=2; will spawn and wait for only one task.
         */
        template <class RandIt, class Predicate>
        RandIt partition_p2(task_thread_pool::task_thread_pool &task_pool, RandIt first, RandIt last, Predicate pred) {
            auto range_size = std::distance(first, last);
            if (range_size < 4) {
                return std::partition(first, last, pred);
            }

            // approach should be generalizable to arbitrary p

            RandIt mid = std::next(first + range_size / 2);

            // partition left and right halves in parallel
            auto left_future = task_pool.submit(std::partition<RandIt, Predicate>, first, mid, pred);
            RandIt right_mid = std::partition(mid, last, pred);
            RandIt left_mid = left_future.get();

            // merge the two partitioned halves
            auto left_highs_size = std::distance(left_mid, mid);
            auto right_lows_size = std::distance(mid, right_mid);
            if (left_highs_size <= right_lows_size) {
                std::swap_ranges(left_mid, mid, right_mid - left_highs_size);
                return right_mid - left_highs_size;
            } else {
                std::swap_ranges(mid, right_mid, left_mid);
                return left_mid + right_lows_size;
            }
        }
    }
}

#endif

#ifndef POOLSTL_INTERNAL_THREAD_IMPL_HPP
#define POOLSTL_INTERNAL_THREAD_IMPL_HPP

/**
 * EXPERIMENTAL: Subject to significant changes or removal.
 * An implementation using only std::thread and no thread pool at all.
 *
 * Advantage:
 *  - Fewer symbols (no packaged_task with its operators, destructors, vtable, etc) means smaller binary
 *    which can mean a lot when there are many calls like with many templates.
 *  - No thread pool to manage.
 *
 * Disadvantages:
 *  - Threads are started and joined for every operation, so it is harder to amortize that cost.
 *  - Barely any algorithms are supported.
 */



namespace poolstl {
    namespace internal {

        template <class ExecPolicy, class RandIt, class Chunk, class ChunkRet, typename... A>
        typename std::enable_if<is_pure_threads_policy<ExecPolicy>::value, void>::type
        parallel_chunk_for_1_wait(ExecPolicy &&policy, RandIt first, RandIt last,
                                  Chunk chunk, ChunkRet*, int extra_split_factor, A&&... chunk_args) {
            std::vector<std::thread> threads;
            auto chunk_size = get_chunk_size(first, last, extra_split_factor * policy.get_num_threads());

            while (first < last) {
                auto iter_chunk_size = get_iter_chunk_size(first, last, chunk_size);
                RandIt loop_end = advanced(first, iter_chunk_size);

                threads.emplace_back(std::thread(chunk, first, loop_end, chunk_args...));

                first = loop_end;
            }

            for (auto& thread : threads) {
                if (thread.joinable()) {
                    thread.join();
                }
            }
        }
    }
}

#endif

namespace poolstl {
    /**
     * NOTE: Iterators are expected to be random access.
     *
     * Like `std::sort`, but allows specifying the sequential sort method, which must have the
     * same signature as the comparator version of `std::sort`.
     *
     * Implemented as a high-level quicksort that delegates to `sort_func`, in parallel, once the range has been
     * sufficiently partitioned.
     */
    template <class ExecPolicy, class RandIt, class Compare>
    poolstl::internal::enable_if_poolstl_policy<ExecPolicy, void>
    pluggable_sort(ExecPolicy &&policy, RandIt first, RandIt last, Compare comp,
                   void (sort_func)(RandIt, RandIt, Compare) = std::sort) {
        if (poolstl::internal::is_seq<ExecPolicy>(policy)) {
            sort_func(first, last, comp);
            return;
        }

        // Parallel partition.
        // The partition_p2 method spawns and waits for its own child task. A deadlock is possible if all worker
        // threads are waiting for tasks that in turn have to workers to execute them. This is only an issue because
        // our thread pool does not have the concept of dependencies.
        // So ensure
        auto& task_pool = *policy.pool();
        std::atomic<int> allowed_parallel_partitions{(int)task_pool.get_num_threads() / 2};

        auto part_func = [&task_pool, &allowed_parallel_partitions](RandIt chunk_first, RandIt chunk_last,
                                   poolstl::internal::pivot_predicate<Compare,
                                   typename std::iterator_traits<RandIt>::value_type> pred) {
            if (allowed_parallel_partitions.fetch_sub(1) > 0) {
                return poolstl::internal::partition_p2(task_pool, chunk_first, chunk_last, pred);
            } else {
                return std::partition(chunk_first, chunk_last, pred);
            }
        };

        poolstl::internal::parallel_quicksort(std::forward<ExecPolicy>(policy), first, last, comp, sort_func, part_func,
                                              poolstl::internal::quicksort_pivot<RandIt>);
    }

    /**
     * NOTE: Iterators are expected to be random access.
     *
     * Like `std::sort`, but allows specifying the sequential sort method, which must have the
     * same signature as the comparator version of `std::sort`.
     *
     * Implemented as a parallel high-level quicksort that delegates to `sort_func` once the range has been
     * sufficiently partitioned.
     */
    template <class ExecPolicy, class RandIt>
    poolstl::internal::enable_if_poolstl_policy<ExecPolicy, void>
    pluggable_sort(ExecPolicy &&policy, RandIt first, RandIt last,
                   void (sort_func)(RandIt, RandIt,
                                    std::less<typename std::iterator_traits<RandIt>::value_type>) = std::sort){
        using T = typename std::iterator_traits<RandIt>::value_type;
        pluggable_sort(std::forward<ExecPolicy>(policy), first, last, std::less<T>(), sort_func);
    }
}

namespace std {

    /**
     * NOTE: Iterators are expected to be random access.
     * See std::copy https://en.cppreference.com/w/cpp/algorithm/copy
     */
    template <class ExecPolicy, class RandIt1, class RandIt2>
    poolstl::internal::enable_if_poolstl_policy<ExecPolicy, RandIt2>
    copy(ExecPolicy &&policy, RandIt1 first, RandIt1 last, RandIt2 dest) {
        if (poolstl::internal::is_seq<ExecPolicy>(policy)) {
            return std::copy(first, last, dest);
        }

        auto futures = poolstl::internal::parallel_chunk_for_2(std::forward<ExecPolicy>(policy), first, last, dest,
                                                               std::copy<RandIt1, RandIt2>, (RandIt2*)nullptr);
        poolstl::internal::get_futures(futures);
        return poolstl::internal::advanced(dest, std::distance(first, last));
    }

    /**
     * NOTE: Iterators are expected to be random access.
     * See std::copy_n https://en.cppreference.com/w/cpp/algorithm/copy_n
     */
    template <class ExecPolicy, class RandIt1, class Size, class RandIt2>
    poolstl::internal::enable_if_poolstl_policy<ExecPolicy, RandIt2>
    copy_n(ExecPolicy &&policy, RandIt1 first, Size n, RandIt2 dest) {
        if (n <= 0) {
            return dest;
        }
        RandIt1 last = poolstl::internal::advanced(first, n);
        std::copy(std::forward<ExecPolicy>(policy), first, last, dest);
        return poolstl::internal::advanced(dest, n);
    }

    /**
     * NOTE: Iterators are expected to be random access.
     * See std::count_if https://en.cppreference.com/w/cpp/algorithm/count_if
     */
    template <class ExecPolicy, class RandIt, class UnaryPredicate>
    poolstl::internal::enable_if_poolstl_policy<ExecPolicy, typename iterator_traits<RandIt>::difference_type>
    count_if(ExecPolicy&& policy, RandIt first, RandIt last, UnaryPredicate p) {
        if (poolstl::internal::is_seq<ExecPolicy>(policy)) {
            return std::count_if(first, last, p);
        }

        using T = typename iterator_traits<RandIt>::difference_type;

        auto futures = poolstl::internal::parallel_chunk_for_1(std::forward<ExecPolicy>(policy), first, last,
                                                               std::count_if<RandIt, UnaryPredicate>,
                                                               (T*)nullptr, 1, p);

        return poolstl::internal::cpp17::reduce(
            poolstl::internal::get_wrap(futures.begin()),
            poolstl::internal::get_wrap(futures.end()), (T)0, std::plus<T>());
    }

    /**
     * NOTE: Iterators are expected to be random access.
     * See std::count https://en.cppreference.com/w/cpp/algorithm/count
     */
    template <class ExecPolicy, class RandIt, class T>
    poolstl::internal::enable_if_poolstl_policy<ExecPolicy, typename iterator_traits<RandIt>::difference_type>
    count(ExecPolicy&& policy, RandIt first, RandIt last, const T& value) {
        return std::count_if(std::forward<ExecPolicy>(policy), first, last,
                             [&value](const T& test) { return test == value; });
    }

    /**
     * NOTE: Iterators are expected to be random access.
     * See std::fill https://en.cppreference.com/w/cpp/algorithm/fill
     */
    template <class ExecPolicy, class RandIt, class Tp>
    poolstl::internal::enable_if_poolstl_policy<ExecPolicy, void>
    fill(ExecPolicy &&policy, RandIt first, RandIt last, const Tp& value) {
        if (poolstl::internal::is_seq<ExecPolicy>(policy)) {
            std::fill(first, last, value);
            return;
        }

        poolstl::internal::parallel_chunk_for_1_wait(std::forward<ExecPolicy>(policy), first, last,
                                                     std::fill<RandIt, Tp>, (void*)nullptr, 1, value);
    }

    /**
     * NOTE: Iterators are expected to be random access.
     * See std::fill_n https://en.cppreference.com/w/cpp/algorithm/fill_n
     */
    template <class ExecPolicy, class RandIt, class Size, class Tp>
    poolstl::internal::enable_if_poolstl_policy<ExecPolicy, RandIt>
    fill_n(ExecPolicy &&policy, RandIt first, Size n, const Tp& value) {
        if (n <= 0) {
            return first;
        }
        RandIt last = poolstl::internal::advanced(first, n);
        std::fill(std::forward<ExecPolicy>(policy), first, last, value);
        return last;
    }

    /**
     * NOTE: Iterators are expected to be random access.
     * See std::find_if https://en.cppreference.com/w/cpp/algorithm/find_if
     */
    template <class ExecPolicy, class RandIt, class UnaryPredicate>
    poolstl::internal::enable_if_poolstl_policy<ExecPolicy, RandIt>
    find_if(ExecPolicy &&policy, RandIt first, RandIt last, UnaryPredicate p) {
        if (poolstl::internal::is_seq<ExecPolicy>(policy)) {
            return std::find_if(first, last, p);
        }

        using diff_t = typename std::iterator_traits<RandIt>::difference_type;
        diff_t n = std::distance(first, last);
        std::atomic<diff_t> extremum(n);

        poolstl::internal::parallel_chunk_for_1_wait(std::forward<ExecPolicy>(policy), first, last,
                                        [&first, &extremum, &p](RandIt chunk_first, RandIt chunk_last) {
                                            if (std::distance(first, chunk_first) > extremum) {
                                             // already found by another task
                                             return;
                                            }

                                            RandIt chunk_res = std::find_if(chunk_first, chunk_last, p);
                                            if (chunk_res != chunk_last) {
                                                // Found, update exremum using a priority update CAS, as discussed in
                                                // "Reducing Contention Through Priority Updates", PPoPP '13
                                                const diff_t k = std::distance(first, chunk_res);
                                                for (diff_t old = extremum; k < old; old = extremum) {
                                                    extremum.compare_exchange_weak(old, k);
                                                }
                                            }
                                        }, (void*)nullptr,
                                        8); // use small tasks so later ones may exit early if item is already found
        return extremum == n ? last : first + extremum;
    }

    /**
     * NOTE: Iterators are expected to be random access.
     * See std::find_if_not https://en.cppreference.com/w/cpp/algorithm/find_if_not
     */
    template <class ExecPolicy, class RandIt, class UnaryPredicate>
    poolstl::internal::enable_if_poolstl_policy<ExecPolicy, RandIt>
    find_if_not(ExecPolicy &&policy, RandIt first, RandIt last, UnaryPredicate p) {
        return std::find_if(std::forward<ExecPolicy>(policy), first, last,
#if POOLSTL_HAVE_CXX17_LIB
                            std::not_fn(p)
#else
                            [&p](const typename std::iterator_traits<RandIt>::value_type& test) { return !p(test); }
#endif
                            );
    }

    /**
     * NOTE: Iterators are expected to be random access.
     * See std::find https://en.cppreference.com/w/cpp/algorithm/find
     */
    template <class ExecPolicy, class RandIt, class T>
    poolstl::internal::enable_if_poolstl_policy<ExecPolicy, RandIt>
    find(ExecPolicy &&policy, RandIt first, RandIt last, const T& value) {
        return std::find_if(std::forward<ExecPolicy>(policy), first, last,
                            [&value](const T& test) { return value == test; });
    }

    /**
     * NOTE: Iterators are expected to be random access.
     * See std::for_each https://en.cppreference.com/w/cpp/algorithm/for_each
     */
    template <class ExecPolicy, class RandIt, class UnaryFunction>
    poolstl::internal::enable_if_poolstl_policy<ExecPolicy, void>
    for_each(ExecPolicy &&policy, RandIt first, RandIt last, UnaryFunction f) {
        // Using a lambda instead of just calling the non-policy std::for_each because it appears to
        // result in a smaller binary.
        auto chunk_func = [&f](RandIt chunk_first, RandIt chunk_last) {
            for (; chunk_first != chunk_last; ++chunk_first) {
                f(*chunk_first);
            }
        };

        if (poolstl::internal::is_seq<ExecPolicy>(policy)) {
            chunk_func(first, last);
            return;
        }

        poolstl::internal::parallel_chunk_for_1_wait(std::forward<ExecPolicy>(policy), first, last,
                                                     chunk_func, (void*)nullptr, 1);
    }

    /**
     * NOTE: Iterators are expected to be random access.
     * See std::for_each_n https://en.cppreference.com/w/cpp/algorithm/for_each_n
     */
    template <class ExecPolicy, class RandIt, class Size, class UnaryFunction>
    poolstl::internal::enable_if_poolstl_policy<ExecPolicy, RandIt>
    for_each_n(ExecPolicy &&policy, RandIt first, Size n, UnaryFunction f) {
        RandIt last = poolstl::internal::advanced(first, n);
        std::for_each(std::forward<ExecPolicy>(policy), first, last, f);
        return last;
    }

    /**
     * NOTE: Iterators are expected to be random access.
     * See std::partition https://en.cppreference.com/w/cpp/algorithm/partition
     *
     * Current implementation uses at most 2 threads.
     */
    template <class ExecPolicy, class RandIt, class Predicate>
    poolstl::internal::enable_if_poolstl_policy<ExecPolicy, RandIt>
    partition(ExecPolicy &&policy, RandIt first, RandIt last, Predicate pred) {
        if (poolstl::internal::is_seq<ExecPolicy>(policy)) {
            return std::partition(first, last, pred);
        }

        return poolstl::internal::partition_p2(*policy.pool(), first, last, pred);
    }

    /**
     * NOTE: Iterators are expected to be random access.
     * See std::sort https://en.cppreference.com/w/cpp/algorithm/sort
     */
    template <class ExecPolicy, class RandIt, class Compare>
    poolstl::internal::enable_if_poolstl_policy<ExecPolicy, void>
    sort(ExecPolicy &&policy, RandIt first, RandIt last, Compare comp) {
        if (poolstl::internal::is_seq<ExecPolicy>(policy)) {
            std::sort(first, last, comp);
            return;
        }

        poolstl::pluggable_sort(std::forward<ExecPolicy>(policy), first, last, comp, std::sort<RandIt, Compare>);
    }

    /**
     * NOTE: Iterators are expected to be random access.
     * See std::sort https://en.cppreference.com/w/cpp/algorithm/sort
     */
    template <class ExecPolicy, class RandIt>
    poolstl::internal::enable_if_poolstl_policy<ExecPolicy, void>
    sort(ExecPolicy &&policy, RandIt first, RandIt last) {
        using T = typename std::iterator_traits<RandIt>::value_type;
        std::sort(std::forward<ExecPolicy>(policy), first, last, std::less<T>());
    }

    /**
     * NOTE: Iterators are expected to be random access.
     * See std::stable_sort https://en.cppreference.com/w/cpp/algorithm/stable_sort
     */
    template <class ExecPolicy, class RandIt, class Compare>
    poolstl::internal::enable_if_poolstl_policy<ExecPolicy, void>
    stable_sort(ExecPolicy &&policy, RandIt first, RandIt last, Compare comp) {
        if (poolstl::internal::is_seq<ExecPolicy>(policy)) {
            std::stable_sort(first, last, comp);
            return;
        }

        poolstl::internal::parallel_quicksort(std::forward<ExecPolicy>(policy), first, last, comp,
                                              std::stable_sort<RandIt, Compare>,
                                              std::stable_partition<RandIt, poolstl::internal::pivot_predicate<Compare,
                                                  typename std::iterator_traits<RandIt>::value_type>>,
                                              poolstl::internal::quicksort_pivot<RandIt>);
    }

    /**
     * NOTE: Iterators are expected to be random access.
     * See std::stable_sort https://en.cppreference.com/w/cpp/algorithm/stable_sort
     */
    template <class ExecPolicy, class RandIt>
    poolstl::internal::enable_if_poolstl_policy<ExecPolicy, void>
    stable_sort(ExecPolicy &&policy, RandIt first, RandIt last) {
        using T = typename std::iterator_traits<RandIt>::value_type;
        std::stable_sort(std::forward<ExecPolicy>(policy), first, last, std::less<T>());
    }

    /**
     * NOTE: Iterators are expected to be random access.
     * See std::transform https://en.cppreference.com/w/cpp/algorithm/transform
     */
    template <class ExecPolicy, class RandIt1, class RandIt2, class UnaryOperation>
    poolstl::internal::enable_if_poolstl_policy<ExecPolicy, RandIt2>
    transform(ExecPolicy&& policy, RandIt1 first1, RandIt1 last1,
              RandIt2 dest, UnaryOperation unary_op) {
        if (poolstl::internal::is_seq<ExecPolicy>(policy)) {
            return poolstl::internal::cpp17::transform(first1, last1, dest, unary_op);
        }

        auto futures = poolstl::internal::parallel_chunk_for_2(std::forward<ExecPolicy>(policy), first1, last1, dest,
                                                               poolstl::internal::cpp17::transform<RandIt1, RandIt2,
                                                                                                   UnaryOperation>,
                                                               (RandIt2*)nullptr, unary_op);
        poolstl::internal::get_futures(futures);
        return dest + std::distance(first1, last1);
    }

    /**
     * NOTE: Iterators are expected to be random access.
     * See std::transform https://en.cppreference.com/w/cpp/algorithm/transform
     */
    template <class ExecPolicy, class RandIt1, class RandIt2, class RandIt3, class BinaryOperation>
    poolstl::internal::enable_if_poolstl_policy<ExecPolicy, RandIt3>
    transform(ExecPolicy&& policy, RandIt1 first1, RandIt1 last1,
              RandIt2 first2, RandIt3 dest, BinaryOperation binary_op) {
        if (poolstl::internal::is_seq<ExecPolicy>(policy)) {
            return poolstl::internal::cpp17::transform(first1, last1, first2, dest, binary_op);
        }

        auto futures = poolstl::internal::parallel_chunk_for_3(std::forward<ExecPolicy>(policy), first1, last1,
                                                               first2, dest,
                                                               poolstl::internal::cpp17::transform<RandIt1, RandIt2,
                                                                                              RandIt3, BinaryOperation>,
                                                               (RandIt3*)nullptr, binary_op);
        poolstl::internal::get_futures(futures);
        return dest + std::distance(first1, last1);
    }

    /**
     * NOTE: Iterators are expected to be random access.
     * See std::all_of https://en.cppreference.com/w/cpp/algorithm/all_of
     */
    template <class ExecPolicy, typename RandIt, typename Predicate>
    poolstl::internal::enable_if_poolstl_policy<ExecPolicy, bool>
    all_of(ExecPolicy&& policy, RandIt first, RandIt last, Predicate pred) {
        return last == std::find_if_not(std::forward<ExecPolicy>(policy), first, last, pred);
    }

    /**
     * NOTE: Iterators are expected to be random access.
     * See std::none_of https://en.cppreference.com/w/cpp/algorithm/none_of
     */
    template <class ExecPolicy, typename RandIt, typename Predicate>
    poolstl::internal::enable_if_poolstl_policy<ExecPolicy, bool>
    none_of(ExecPolicy&& policy, RandIt first, RandIt last, Predicate pred) {
        return last == std::find_if(std::forward<ExecPolicy>(policy), first, last, pred);
    }

    /**
     * NOTE: Iterators are expected to be random access.
     * See std::any_of https://en.cppreference.com/w/cpp/algorithm/any_of
     */
    template <class ExecPolicy, typename RandIt, typename Predicate>
    poolstl::internal::enable_if_poolstl_policy<ExecPolicy, bool>
    any_of(ExecPolicy&& policy, RandIt first, RandIt last, Predicate pred) {
        return !std::none_of(std::forward<ExecPolicy>(policy), first, last, pred);
    }
}

namespace poolstl {

    template <class RandIt, class ChunkConstructor, class UnaryFunction>
    void for_each_chunk(RandIt first, RandIt last, ChunkConstructor construct, UnaryFunction f) {
        if (first == last) {
            return;
        }

        auto chunk_data = construct();
        for (; first != last; ++first) {
            f(*first, chunk_data);
        }
    }

    /**
     * NOTE: Iterators are expected to be random access.
     *
     * Like `std::for_each`, but exposes the chunking. The `construct` method is called once per parallel chunk and
     * its output is passed to `f`.
     *
     * Useful for cases where an expensive workspace can be shared between loop iterations
     * but cannot be shared by all parallel iterations.
     */
    template <class ExecPolicy, class RandIt, class ChunkConstructor, class UnaryFunction>
    poolstl::internal::enable_if_poolstl_policy<ExecPolicy, void>
    for_each_chunk(ExecPolicy&& policy, RandIt first, RandIt last, ChunkConstructor construct, UnaryFunction f) {
        if (poolstl::internal::is_seq<ExecPolicy>(policy)) {
            for_each_chunk(first, last, construct, f);
            return;
        }

        poolstl::internal::parallel_chunk_for_1_wait(std::forward<ExecPolicy>(policy), first, last,
                                                     for_each_chunk <RandIt, ChunkConstructor, UnaryFunction>,
                                                     (void*)nullptr, 1, construct, f);
    }

    /**
     * NOTE: Iterators are expected to be random access.
     *
     * Parallel merge sort.
     *
     * @param comp Comparator.
     * @param sort_func Sequential sort method. Must have the same signature as the comparator version of `std::sort`.
     * @param merge_func Sequential merge method. Must have the same signature as `std::inplace_merge`.
     */
    template <class ExecPolicy, class RandIt, class Compare>
    poolstl::internal::enable_if_poolstl_policy<ExecPolicy, void>
    pluggable_mergesort(ExecPolicy &&policy, RandIt first, RandIt last, Compare comp,
                        void (sort_func)(RandIt, RandIt, Compare) = std::sort,
                        void (merge_func)(RandIt, RandIt, RandIt, Compare) = std::inplace_merge) {
        if (poolstl::internal::is_seq<ExecPolicy>(policy)) {
            sort_func(first, last, comp);
            return;
        }

        poolstl::internal::parallel_mergesort(std::forward<ExecPolicy>(policy),
                                              first, last, comp, sort_func, merge_func);
    }

    /**
     * NOTE: Iterators are expected to be random access.
     *
     * Parallel merge sort.
     *
     * Uses `std::less` comparator.
     *
     * @param sort_func Sequential sort method. Must have the same signature as the comparator version of `std::sort`.
     * @param merge_func Sequential merge method. Must have the same signature as `std::inplace_merge`.
     */
    template <class ExecPolicy, class RandIt>
    poolstl::internal::enable_if_poolstl_policy<ExecPolicy, void>
    pluggable_mergesort(ExecPolicy &&policy, RandIt first, RandIt last,
                   void (sort_func)(RandIt, RandIt,
                                    std::less<typename std::iterator_traits<RandIt>::value_type>) = std::sort,
                   void (merge_func)(RandIt, RandIt, RandIt,
                                    std::less<typename std::iterator_traits<RandIt>::value_type>) = std::inplace_merge){
        using T = typename std::iterator_traits<RandIt>::value_type;
        pluggable_mergesort(std::forward<ExecPolicy>(policy), first, last, std::less<T>(), sort_func, merge_func);
    }

    /**
     * NOTE: Iterators are expected to be random access.
     *
     * Parallel quicksort that allows specifying the sequential sort and partition methods.
     *
     * @param comp Comparator.
     * @param sort_func Sequential sort method to use once range is sufficiently partitioned. Must have the same
     *                  signature as the comparator version of `std::sort`.
     * @param part_func Sequential partition method. Must have the same signature as `std::partition`.
     * @param pivot_func Method that identifies the pivot element
     */
    template <class ExecPolicy, class RandIt, class Compare>
    poolstl::internal::enable_if_poolstl_policy<ExecPolicy, void>
    pluggable_quicksort(ExecPolicy &&policy, RandIt first, RandIt last, Compare comp,
                        void (sort_func)(RandIt, RandIt, Compare) = std::sort,
                        RandIt (part_func)(RandIt, RandIt, poolstl::internal::pivot_predicate<Compare,
                            typename std::iterator_traits<RandIt>::value_type>) = std::partition,
                        typename std::iterator_traits<RandIt>::value_type (pivot_func)(RandIt, RandIt) =
                            poolstl::internal::quicksort_pivot) {
        if (poolstl::internal::is_seq<ExecPolicy>(policy)) {
            sort_func(first, last, comp);
            return;
        }

        poolstl::internal::parallel_quicksort(std::forward<ExecPolicy>(policy),
                                              first, last, comp, sort_func, part_func, pivot_func);
    }

    /**
     * NOTE: Iterators are expected to be random access.
     *
     * Parallel quicksort that allows specifying the sequential sort and partition methods.
     *
     * Uses `std::less` comparator.
     *
     * @param sort_func Sequential sort method to use once range is sufficiently partitioned. Must have the same
     *                  signature as the comparator version of `std::sort`.
     * @param part_func Sequential partition method. Must have the same signature as `std::partition`.
     * @param pivot_func Method that identifies the pivot element
     */
    template <class ExecPolicy, class RandIt>
    poolstl::internal::enable_if_poolstl_policy<ExecPolicy, void>
    pluggable_quicksort(ExecPolicy &&policy, RandIt first, RandIt last,
                        void (sort_func)(RandIt, RandIt,
                                    std::less<typename std::iterator_traits<RandIt>::value_type>) = std::sort,
                        RandIt (part_func)(RandIt, RandIt, poolstl::internal::pivot_predicate<
                            std::less<typename std::iterator_traits<RandIt>::value_type>,
                            typename std::iterator_traits<RandIt>::value_type>) = std::partition,
                        typename std::iterator_traits<RandIt>::value_type (pivot_func)(RandIt, RandIt) =
                            poolstl::internal::quicksort_pivot) {
        using T = typename std::iterator_traits<RandIt>::value_type;
        pluggable_quicksort(std::forward<ExecPolicy>(policy), first, last, std::less<T>(),
                            sort_func, part_func, pivot_func);
    }
}

#endif

#ifndef POOLSTL_NUMERIC_HPP
#define POOLSTL_NUMERIC_HPP

#include <tuple>


namespace std {

#if POOLSTL_HAVE_CXX17_LIB
    /**
     * NOTE: Iterators are expected to be random access.
     * See std::exclusive_scan https://en.cppreference.com/w/cpp/algorithm/exclusive_scan
     */
    template <class ExecPolicy, class RandIt1, class RandIt2, class T, class BinaryOp>
    poolstl::internal::enable_if_poolstl_policy<ExecPolicy, RandIt2>
    exclusive_scan(ExecPolicy &&policy, RandIt1 first, RandIt1 last, RandIt2 dest, T init, BinaryOp binop) {
        if (first == last) {
            return dest;
        }

        if (poolstl::internal::is_seq<ExecPolicy>(policy)) {
            return std::exclusive_scan(first, last, dest, init, binop);
        }

        // Pass 1: Chunk the input and find the sum of each chunk
        auto futures = poolstl::internal::parallel_chunk_for_gen(std::forward<ExecPolicy>(policy), first, last,
                             [binop](RandIt1 chunk_first, RandIt1 chunk_last) {
                                 auto sum = std::accumulate(chunk_first, chunk_last, T{}, binop);
                                 return std::make_tuple(std::make_pair(chunk_first, chunk_last), sum);
                             });

        std::vector<std::pair<RandIt1, RandIt1>> ranges;
        std::vector<T> sums;

        for (auto& future : futures) {
            auto res = future.get();
            ranges.push_back(std::get<0>(res));
            sums.push_back(std::get<1>(res));
        }

        // find initial values for each range
        std::exclusive_scan(sums.begin(), sums.end(), sums.begin(), init, binop);

        // Pass 2: perform exclusive scan of each chunk, using the sum of previous chunks as init
        std::vector<std::tuple<RandIt1, RandIt1, RandIt2, T>> args;
        for (std::size_t i = 0; i < sums.size(); ++i) {
            auto chunk_first = std::get<0>(ranges[i]);
            args.emplace_back(std::make_tuple(
                chunk_first, std::get<1>(ranges[i]),
                dest + (chunk_first - first),
                sums[i]));
        }

        auto futures2 = poolstl::internal::parallel_apply(std::forward<ExecPolicy>(policy),
            [binop](RandIt1 chunk_first, RandIt1 chunk_last, RandIt2 chunk_dest, T chunk_init){
                std::exclusive_scan(chunk_first, chunk_last, chunk_dest, chunk_init, binop);
            }, args);

        poolstl::internal::get_futures(futures2);
        return dest + (last - first);
    }

    /**
     * NOTE: Iterators are expected to be random access.
     * See std::exclusive_scan https://en.cppreference.com/w/cpp/algorithm/exclusive_scan
     */
    template <class ExecPolicy, class RandIt1, class RandIt2, class T>
    poolstl::internal::enable_if_poolstl_policy<ExecPolicy, RandIt2>
    exclusive_scan(ExecPolicy &&policy, RandIt1 first, RandIt1 last, RandIt2 dest, T init) {
        return std::exclusive_scan(std::forward<ExecPolicy>(policy), first, last, dest, init, std::plus<T>());
    }
#endif

    /**
     * NOTE: Iterators are expected to be random access.
     * See std::reduce https://en.cppreference.com/w/cpp/algorithm/reduce
     */
    template <class ExecPolicy, class RandIt, class T, class BinaryOp>
    poolstl::internal::enable_if_poolstl_policy<ExecPolicy, T>
    reduce(ExecPolicy &&policy, RandIt first, RandIt last, T init, BinaryOp binop) {
        if (poolstl::internal::is_seq<ExecPolicy>(policy)) {
            return poolstl::internal::cpp17::reduce(first, last, init, binop);
        }

        auto futures = poolstl::internal::parallel_chunk_for_1(std::forward<ExecPolicy>(policy), first, last,
                                                               poolstl::internal::cpp17::reduce<RandIt, T, BinaryOp>,
                                                               (T*)nullptr, 1, init, binop);

        return poolstl::internal::cpp17::reduce(
            poolstl::internal::get_wrap(futures.begin()),
            poolstl::internal::get_wrap(futures.end()), init, binop);
    }

    /**
     * NOTE: Iterators are expected to be random access.
     * See std::reduce https://en.cppreference.com/w/cpp/algorithm/reduce
     */
    template <class ExecPolicy, class RandIt, class T>
    poolstl::internal::enable_if_poolstl_policy<ExecPolicy, T>
    reduce(ExecPolicy &&policy, RandIt first, RandIt last, T init) {
        return std::reduce(std::forward<ExecPolicy>(policy), first, last, init, std::plus<T>());
    }

    /**
     * NOTE: Iterators are expected to be random access.
     * See std::reduce https://en.cppreference.com/w/cpp/algorithm/reduce
     */
    template <class ExecPolicy, class RandIt>
    poolstl::internal::enable_if_poolstl_policy<
        ExecPolicy, typename std::iterator_traits<RandIt>::value_type>
    reduce(ExecPolicy &&policy, RandIt first, RandIt last) {
        return std::reduce(std::forward<ExecPolicy>(policy), first, last,
                           typename std::iterator_traits<RandIt>::value_type{});
    }

#if POOLSTL_HAVE_CXX17_LIB
    /**
     * NOTE: Iterators are expected to be random access.
     * See std::transform_reduce https://en.cppreference.com/w/cpp/algorithm/transform_reduce
     */
    template <class ExecPolicy, class RandIt1, class T, class BinaryReductionOp, class UnaryTransformOp>
    poolstl::internal::enable_if_poolstl_policy<ExecPolicy, T>
    transform_reduce(ExecPolicy&& policy, RandIt1 first1, RandIt1 last1, T init,
                     BinaryReductionOp reduce_op, UnaryTransformOp transform_op) {
        if (poolstl::internal::is_seq<ExecPolicy>(policy)) {
            return std::transform_reduce(first1, last1, init, reduce_op, transform_op);
        }

        auto futures = poolstl::internal::parallel_chunk_for_1(std::forward<ExecPolicy>(policy), first1, last1,
                                                               std::transform_reduce<RandIt1, T,
                                                                                   BinaryReductionOp, UnaryTransformOp>,
                                                               (T*)nullptr, 1, init, reduce_op, transform_op);

        return poolstl::internal::cpp17::reduce(
            poolstl::internal::get_wrap(futures.begin()),
            poolstl::internal::get_wrap(futures.end()), init, reduce_op);
    }

    /**
     * NOTE: Iterators are expected to be random access.
     * See std::transform_reduce https://en.cppreference.com/w/cpp/algorithm/transform_reduce
     */
    template <class ExecPolicy, class RandIt1, class RandIt2, class T, class BinaryReductionOp, class BinaryTransformOp>
    poolstl::internal::enable_if_poolstl_policy<ExecPolicy, T>
    transform_reduce(ExecPolicy&& policy, RandIt1 first1, RandIt1 last1, RandIt2 first2, T init,
                     BinaryReductionOp reduce_op, BinaryTransformOp transform_op) {
        if (poolstl::internal::is_seq<ExecPolicy>(policy)) {
            return std::transform_reduce(first1, last1, first2, init, reduce_op, transform_op);
        }

        auto futures = poolstl::internal::parallel_chunk_for_2(std::forward<ExecPolicy>(policy), first1, last1, first2,
                                                               std::transform_reduce<RandIt1, RandIt2, T,
                                                                                  BinaryReductionOp, BinaryTransformOp>,
                                                               (T*)nullptr, init, reduce_op, transform_op);

        return poolstl::internal::cpp17::reduce(
            poolstl::internal::get_wrap(futures.begin()),
            poolstl::internal::get_wrap(futures.end()), init, reduce_op);
    }

    /**
     * NOTE: Iterators are expected to be random access.
     * See std::transform_reduce https://en.cppreference.com/w/cpp/algorithm/transform_reduce
     */
    template< class ExecPolicy, class RandIt1, class RandIt2, class T >
    poolstl::internal::enable_if_poolstl_policy<ExecPolicy, T>
    transform_reduce(ExecPolicy&& policy, RandIt1 first1, RandIt1 last1, RandIt2 first2, T init ) {
        return transform_reduce(std::forward<ExecPolicy>(policy),
            first1, last1, first2, init, std::plus<>(), std::multiplies<>());
    }
#endif

}

#endif

// Note that iota_iter.hpp is self-contained in its own right.

#ifndef POOLSTL_IOTA_ITER_HPP
#define POOLSTL_IOTA_ITER_HPP

#include <cstddef>
#include <iterator>

namespace poolstl {

    /**
     * An iterator over the integers.
     *
     * Effectively a view on a fictional vector populated by std::iota, but without materializing anything.
     *
     * Useful to parallelize loops that are not over a container, like this:
     *
     * \code{.cpp}
     * for (int i = 0; i < 10; ++i) {
     * }
     *\endcode
     *
     * Becomes:
     * \code{.cpp}
     * std::for_each(iota_iter<int>(0), iota_iter<int>(10), [](int i) {
     * });
     * \endcode
     *
     * @tparam T A type that acts as an integer.
     */
    template<typename T>
    class iota_iter {
    public:
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T *;
        using reference = T;
        using iterator_category = std::random_access_iterator_tag;

        iota_iter() : value{} {}
        explicit iota_iter(T rhs) : value(rhs) {}
        iota_iter(const iota_iter<T> &rhs) : value(rhs.value) {}

        iota_iter<T> &operator=(T rhs) { value = rhs; return *this; }
        iota_iter<T> &operator=(const iota_iter &rhs) { value = rhs.value; return *this; }

        reference operator*() const { return value; }
        reference operator[](difference_type rhs) const { return value + rhs; }
        // operator-> has no meaning in this application

        bool operator==(const iota_iter<T> &rhs) const { return value == rhs.value; }
        bool operator!=(const iota_iter<T> &rhs) const { return value != rhs.value; }
        bool operator<(const iota_iter<T> &rhs) const { return value < rhs.value; }
        bool operator>(const iota_iter<T> &rhs) const { return value > rhs.value; }
        bool operator<=(const iota_iter<T> &rhs) const { return value <= rhs.value; }
        bool operator>=(const iota_iter<T> &rhs) const { return value >= rhs.value; }

        iota_iter<T> &operator+=(difference_type rhs) { value += rhs; return *this; }
        iota_iter<T> &operator-=(difference_type rhs) { value -= rhs; return *this; }

        iota_iter<T> &operator++() { ++value; return *this; }
        iota_iter<T> &operator--() { --value; return *this; }
        iota_iter<T> operator++(int) { iota_iter<T> ret(value); ++value; return ret; }
        iota_iter<T> operator--(int) { iota_iter<T> ret(value); --value; return ret; }

        difference_type operator-(const iota_iter<T> &rhs) const { return value - rhs.value; }
        iota_iter<T> operator-(difference_type rhs) const { return iota_iter(value - rhs); }
        iota_iter<T> operator+(difference_type rhs) const { return iota_iter(value + rhs); }

        friend inline iota_iter<T> operator+(difference_type lhs, const iota_iter<T> &rhs) {
            return iota_iter(lhs + rhs.value);
        }

    protected:
        T value;
    };
}

namespace std {
    /**
     * Specialize std::iterator_traits for poolstl::iota_iter.
     */
    template <typename T>
    struct iterator_traits<poolstl::iota_iter<T>> {
        using value_type =        typename poolstl::iota_iter<T>::value_type;
        using difference_type =   typename poolstl::iota_iter<T>::difference_type;
        using pointer =           typename poolstl::iota_iter<T>::pointer;
        using reference =         typename poolstl::iota_iter<T>::reference;
        using iterator_category = typename poolstl::iota_iter<T>::iterator_category;
    };
}

#endif

/*
 * Optionally alias `poolstl::par` as `std::execution::par` to enable poolSTL to fill in for missing compiler support.
 *
 * USE AT YOUR OWN RISK!
 *
 * To use this define POOLSTL_STD_SUPPLEMENT=1 before including poolstl.hpp.
 *
 * Attempts to autodetect native support by checking for <execution>, including it if it exists, and then checking for
 * the __cpp_lib_parallel_algorithm feature macro.
 *
 * If native support is not found then the standard execution policies are declared as forwards to poolSTL.
 *
 * GCC and Clang: TBB is required if <execution> is #included. If you'd like to use the poolSTL supplement in cases
 * that TBB is not available, have your build system define POOLSTL_STD_SUPPLEMENT_NO_INCLUDE if TBB is not found.
 * PoolSTL will then not include <execution> and the supplement will kick in.
 * Your code must not #include <execution>.
 *
 * MinGW: the compiler declares support, but actual performance is sequential (see poolSTL benchmark). To use
 * the supplement anyway define POOLSTL_STD_SUPPLEMENT_FORCE to override the autodetection.
 * Your code must not #include <execution>.
 *
 * Define POOLSTL_ALLOW_SUPPLEMENT=0 to override POOLSTL_STD_SUPPLEMENT and disable this feature.
 */
#ifndef POOLSTL_ALLOW_SUPPLEMENT
#define POOLSTL_ALLOW_SUPPLEMENT 1
#endif

#if POOLSTL_ALLOW_SUPPLEMENT && defined(POOLSTL_STD_SUPPLEMENT)

#if __cplusplus >= 201603L || (defined(_MSVC_LANG) && _MSVC_LANG >= 201603L)
#if __has_include(<execution>)
#ifndef POOLSTL_STD_SUPPLEMENT_NO_INCLUDE
#endif
#endif
#endif

#if !defined(__cpp_lib_parallel_algorithm) || defined(POOLSTL_STD_SUPPLEMENT_FORCE)
namespace std {
    namespace execution {
        using ::poolstl::execution::sequenced_policy;
        using ::poolstl::execution::seq;
        using ::poolstl::execution::parallel_policy;
        using ::poolstl::execution::par;
        using parallel_unsequenced_policy = ::poolstl::execution::parallel_policy;
        constexpr parallel_unsequenced_policy par_unseq{};
    }
}

#endif
#endif

#endif
