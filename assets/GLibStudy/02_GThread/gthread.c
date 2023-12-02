/* implement gthread.h's inline functions */
#define G_IMPLEMENT_INLINES 1
#define __G_THREAD_C__

#include "config.h"

#include "gthread.h"
#include "gthreadprivate.h"

#include <string.h>

#ifdef G_OS_UNIX
#include <unistd.h>
#endif

#ifndef G_OS_WIN32
#include <sys/time.h>
#include <time.h>
#else
#include <windows.h>
#endif /* G_OS_WIN32 */

#include "gslice.h"
#include "gstrfuncs.h"
#include "gtestutils.h"
#include "glib_trace.h"
#include "gtrace-private.h"

/**
 * SECTION:threads
 * @title: Threads
 * @short_description: portable support for threads, mutexes, locks,
 *     conditions and thread private data
 * @see_also: #GThreadPool, #GAsyncQueue
 *
 * 线程几乎像进程一样运行，但与进程不同的是，一个进程的所有线程共享相同的内存。
 * 这是好的，因为它通过共享内存提供了线程之间的轻松通信，但也有不好的地方，
 * 因为如果程序没有谨慎设计，可能会发生奇怪的事情（所谓的“Heisenbugs”）。
 * 
 * 线程的并发性质意味着不能假设不同线程中运行的代码的执行顺序，
 * 除非程序员通过同步原语primitive明确强制执行顺序。
 *
 * GLib中与线程相关的函数的目标是提供一种可移植的方式来编写多线程软件。
 * 有用于保护内存部分访问的互斥锁的原语（#GMutex、#GRecMutex和#GRWLock）；
 * 有使用单个位进行锁定的设施（g_bit_lock()）；
 * 有用于线程同步的条件变量的原语（#GCond）；
 * 有线程专用数据的原语 - 每个线程都有一个私有实例（#GPrivate）；
 * 有一次性初始化的功能（#GOnce、g_once_init_enter()）；
 * 最后，有创建和管理线程的原语（#GThread）。
 *
 * GLib的线程系统曾经需要使用g_thread_init()进行初始化，
 * 但自2.32版本以来，GLib的线程系统会在程序启动时自动初始化，
 * 所有与线程创建函数和同步原语相关的函数也会立即可用。
 *
 * 请注意，不能安全地假设您的程序没有线程，即使您不自己调用g_thread_new()。
 * GLib和GIO在某些情况下会为其自身的目的创建线程，例如在使用g_unix_signal_source_new()或使用GDBus时。
 *
 * 最初，UNIX没有线程，因此一些传统的UNIX API在多线程程序中存在问题。一些显著的例子包括：
 * 
 * - 返回数据的C库函数使用静态分配的缓冲区，例如strtok()或strerror()。
 *   对于这些函数，通常有带有_r后缀的线程安全变体，或者您可以查看对应的GLib API（如g_strsplit()或g_strerror()）。
 *
 * - setenv()和unsetenv()函数以不线程安全的方式操纵进程环境，并可能干扰其他线程中的getenv()调用。
 *   请注意，getenv()调用可能隐藏在其他API之后。例如，GNU gettext()在幕后调用getenv()。
 *   通常最好将环境视为只读。如果确实必须修改环境，请在没有其他线程存在的情况下在main()中早早地进行修改。
 *
 * - setlocale()函数更改整个进程的区域设置，影响所有线程。通常会临时更改区域设置以更改字符串扫描或格式化函数的行为，如scanf()或printf()。
 *   GLib提供了许多字符串API（如g_ascii_formatd()或g_ascii_strtod()），通常可用作替代方法。或者，您可以使用uselocale()函数仅为当前线程更改区域设置。
 *
 * - fork()函数仅将调用线程引入子进程的进程映像的一部分。如果其他线程正在关键部分执行，它们可能已经将互斥锁锁定，这可能很容易导致新子进程中的死锁。
 *   因此，应在子进程中尽快调用exit()或exec()，并仅在此之前进行信号安全的库调用。
 *
 * - daemon()函数以与上述描述相反的方式使用fork()。不应在GLib程序中使用它。
 *
 * GLib本身在内部完全线程安全（所有全局数据都会自动锁定），但出于性能原因，不会自动锁定各个数据结构实例。
 * 例如，您必须协调从多个线程访问同一个#GHashTable的访问。从这个规则中的两个显著例外是#GMainLoop和#GAsyncQueue，
 * 它们是线程安全的，不需要进一步的应用级锁定即可从多个线程访问。大多数引用计数函数，如g_object_ref()，也是线程安全的。
 *
 * #GThread的常见用法之一是将长时间阻塞的操作移到主线程之外并移到工作线程中。对于GLib函数（例如单个GIO操作），这是不必要的，并且会使代码复杂化。
 * 相反，应该从主线程中使用函数的…_async()版本，从而消除了在多个线程之间的锁定和同步的需要。如果确实需要将操作移动到工作线程，
 * 请考虑使用g_task_run_in_thread()或#GThreadPool。与#GThread相比，#GThreadPool通常是更好的选择，因为它处理线程重用和任务排队；#GTask在内部使用这个功能。
 *
 * 但是，如果需要顺序执行多个阻塞操作，并且不能使用#GTask进行操作，将它们移到工作线程中可以使代码更清晰。
 */

/* G_LOCK Documentation {{{1 ---------------------------------------------- */

/**
 * G_LOCK_DEFINE:
 * @name: the name of the lock
 *
 * The `G_LOCK_` 宏提供了对#GMutex的方便接口。
 * %G_LOCK_DEFINE 定义一个锁。它可以出现在程序中变量定义可以出现的任何地方，例如函数的第一个块或函数之外。
 * @name参数将被添加前缀以获取#GMutex的名称。这意味着您可以使用现有变量的名称作为参数，例如您打算用锁保护的变量的名称。
 *
 * 以下是 `G_LOCK` 宏示例:
 *
 * |[<!-- language="C" --> 
 *   G_LOCK_DEFINE (current_number); // 实际名称是  g__current_number_lock
 *
 *   int
 *   give_me_next_number (void)
 *   {
 *     static int current_number = 0;
 *     int ret_val;
 *
 *     G_LOCK (current_number);
 *     ret_val = current_number = calc_next_number (current_number);
 *     G_UNLOCK (current_number);
 *
 *     return ret_val;
 *   }
 * ]|
 */

/**
 * G_LOCK_DEFINE_STATIC:
 * @name: the name of the lock
 *
 * 这与%G_LOCK_DEFINE类似，但它创建一个静态对象。
 */

/**
 * G_LOCK_EXTERN:
 * @name: the name of the lock
 *
 * 这声明一个由另一个模块中的%G_LOCK_DEFINE定义的锁。
 */

/**
 * G_LOCK:
 * @name: the name of the lock
 *
 * 类似于g_mutex_lock()，但用于由%G_LOCK_DEFINE定义的锁。
 */

/**
 * G_TRYLOCK:
 * @name: the name of the lock
 *
 * 类似于g_mutex_trylock()，但用于由%G_LOCK_DEFINE定义的锁。
 *
 * 如果成功锁定锁定，则返回%TRUE。
 */

/**
 * G_UNLOCK:
 * @name: the name of the lock
 *
 * 类似于g_mutex_unlock()，但用于由%G_LOCK_DEFINE定义的锁。
 */

/* GMutex Documentation {{{1 ------------------------------------------ */

/**
 * GMutex:
 *
 * #GMutex结构体是一个不透明的数据结构，用于表示互斥锁(mutual exclusion)。它可用于保护数据免受共享访问。
 *
 * 以下函数为例：
 * |[<!-- language="C" --> 
 *   int
 *   give_me_next_number (void)
 *   {
 *     static int current_number = 0;
 *
 *     // 现在执行一个非常复杂的计算来计算新的数字，例如可以是一个随机数生成器
 *     current_number = calc_next_number (current_number);
 *
 *     return current_number;
 *   }
 * ]|
 *
 * 很容易看出，这在多线程应用程序中无法工作。current_number必须受到保护，以防止共享访问。#GMutex可以用作解决此问题的解决方案：
 * 
 * |[<!-- language="C" --> 
 *   int
 *   give_me_next_number (void)
 *   {
 *     static GMutex mutex;
 *     static int current_number = 0;
 *     int ret_val;
 *
 *     g_mutex_lock (&mutex);
 *     ret_val = current_number = calc_next_number (current_number);
 *     g_mutex_unlock (&mutex);
 *
 *     return ret_val;
 *   }
 * ]|
 * 
 * 请注意，#GMutex未初始化为特定值。它在静态存储中的放置确保它将被初始化为全零，这是合适的。
 * 如果将#GMutex放置在其他上下文中（例如嵌入在结构体中），则必须使用g_mutex_init()显式初始化它。
 *
 * #GMutex应只通过 g_mutex_ 函数访问。 
 */

/* GRecMutex Documentation {{{1 -------------------------------------- */

/**
 * GRecMutex（递归互斥锁）：
 *
 * The GRecMutex struct is an opaque data structure to represent a
 * recursive mutex. It is similar to a #GMutex with the difference
 * that it is possible to lock a GRecMutex multiple times in the same
 * thread without deadlock. When doing so, care has to be taken to
 * unlock the recursive mutex as often as it has been locked.
 *  
 * GRecMutex结构体是一个不透明的数据结构，用于表示递归互斥锁。它类似于#GMutex，
 * 但不同之处在于在同一线程中可以多次锁定GRecMutex而不会发生死锁。在这样做时，必须小心确保解锁递归互斥锁的次数与锁定的次数相同。
 * 
 * 如果在静态存储中分配了GRecMutex，那么它可以在不进行初始化的情况下使用。
 * 否则，您应该在使用后调用g_rec_mutex_init()进行初始化，并在完成后调用g_rec_mutex_clear()进行清理。
 * 
 * GRecMutex应只通过 g_rec_mutex_ 函数访问。
 * 
 * 自版本2.32以来可用。

 */

/* GRWLock Documentation {{{1 ---------------------------------------- */

/**
 * GRWLock（读写锁）:
 * 
 * GRWLock结构体是一个不透明的数据结构，用于表示读写锁。它类似于#GMutex，允许多个线程协调对共享资源的访问。
 * 
 * 与互斥锁不同的是，读写锁区分只读（'reader'）和完全（'writer'）访问。
 * 虽然一次只允许一个线程进行写访问（通过持有'writer'锁，使用g_rw_lock_writer_lock()），
 * 但多个线程可以同时获得只读访问（通过持有'reader'锁，使用g_rw_lock_reader_lock()）。
 * 
 * 当一个读者已经持有锁并且一个写者排队等待获取锁时，不确定读者或写者在获取锁时的优先级。
 * 
 * 以下是一个具有访问函数的数组示例：
 *
 * |[<!-- language="C" --> 
 *   GRWLock lock;
 *   GPtrArray *array;
 *
 *   gpointer
 *   my_array_get (guint index)
 *   {
 *     gpointer retval = NULL;
 *
 *     if (!array)
 *       return NULL;
 *
 *     g_rw_lock_reader_lock (&lock);
 *     if (index < array->len)
 *       retval = g_ptr_array_index (array, index);
 *     g_rw_lock_reader_unlock (&lock);
 *
 *     return retval;
 *   }
 *
 *   void
 *   my_array_set (guint index, gpointer data)
 *   {
 *     g_rw_lock_writer_lock (&lock);
 *
 *     if (!array)
 *       array = g_ptr_array_new ();
 *
 *     if (index >= array->len)
 *       g_ptr_array_set_size (array, index+1);
 *     g_ptr_array_index (array, index) = data;
 *
 *     g_rw_lock_writer_unlock (&lock);
 *   }
 *  ]|
 *
 * 这个示例展示了一个可以被多个读者（my_array_get()函数）同时访问的数组，而写者（my_array_set()函数）只允许一个，
 * 并且只有在没有读者访问数组时才允许写者访问。这是因为数组的大小可能会发生更改，使用这些函数现在是完全多线程安全的。
 * 
 * 如果在静态存储中分配了GRWLock，则可以在不进行初始化的情况下使用。否则，您应该在使用后调用
 * g_rw_lock_init()进行初始化，并在完成后调用g_rw_lock_clear()进行清理。
 * 
 * GRWLock应只通过 g_rw_lock_ 函数访问。
 * 
 * 自版本2.32以来可用。
 */

/* GCond Documentation {{{1 ------------------------------------------ */

/**
 * GCond（条件变量）:
 *
 * GCond结构体是一个不透明的数据结构，用于表示条件变量。如果线程发现某个条件为假，它们可以在GCond上阻塞。
 * 如果其他线程更改此条件的状态，它们会发出GCond信号，导致等待的线程被唤醒。
 * 
 * 考虑以下共享变量的示例。一个或多个线程可以等待数据发布到变量上，当另一个线程发布数据时，
 * 它可以向其中一个等待的线程发出信号，以唤醒它们以收集数据。
 * 
 * 以下是使用GCond阻塞线程直到满足条件的示例：
 * 
 * |[<!-- language="C" --> 
 *   gpointer current_data = NULL;
 *   GMutex data_mutex;
 *   GCond data_cond;
 *
 *   void
 *   push_data (gpointer data)
 *   {
 *     g_mutex_lock (&data_mutex);
 *     current_data = data;
 *     g_cond_signal (&data_cond);
 *     g_mutex_unlock (&data_mutex);
 *   }
 *
 *   gpointer
 *   pop_data (void)
 *   {
 *     gpointer data;
 *
 *     g_mutex_lock (&data_mutex);
 *     while (!current_data)
 *       g_cond_wait (&data_cond, &data_mutex);
 *     data = current_data;
 *     current_data = NULL;
 *     g_mutex_unlock (&data_mutex);
 *
 *     return data;
 *   }
 * ]|
 * 
 * 
 * 现在每当线程调用pop_data()时，它都会等待直到current_data非空，也就是直到其他线程调用push_data()。
 * 
 * 该示例显示，条件变量的使用必须始终与互斥锁配对使用。如果没有使用互斥锁，那么在pop_data()中的while循环中检查current_data和等待之间将存在竞争。
 * 具体来说，另一个线程可以在检查后设置current_data，并在第一个线程进入睡眠之前发出条件信号（没有任何线程等待它）。GCond特别有用的地方在于它能够原子地释放互斥锁并进入睡眠。
 * 
 * 同样重要的是，只有在检查条件是否为真的循环内部使用g_cond_wait()和g_cond_wait_until()函数。有关在返回后条件可能仍为假的解释，请参阅g_cond_wait()。
 * 
 * 如果在静态存储中分配了GCond，则可以在不进行初始化的情况下使用。否则，您应该在使用后调用g_cond_init()进行初始化，并在完成后调用g_cond_clear()进行清理。
 * 
 * GCond应只通过g_cond_函数访问。
 */

/* GThread Documentation {{{1 ---------------------------------------- */

/**
 * **GThread（线程）**：
 * 
 * #GThread结构体表示一个正在运行的线程。此结构体由g_thread_new()或g_thread_try_new()返回。您可以通过调用g_thread_self()来获取代表当前线程的#GThread结构体。
 * 
 * GThread是引用计数的，请参阅g_thread_ref()和g_thread_unref()。表示它的线程在运行时持有引用，而g_thread_join()消耗了给定的引用，因此通常不需要显式管理GThread引用。
 * 
 * 该结构体是不透明的 - 其字段不能直接访问。
 */

/**
 * *GThreadFunc（线程函数类型）**
 * @data：传递给线程的数据
 * 指定传递给g_thread_new()或g_thread_try_new()的@func函数的类型。
 * 
 * 返回值：线程的返回值
 */

/**
 * **g_thread_supported（线程是否支持）**：
 * 
 * 此宏返回%TRUE，如果线程系统已初始化，则返回%FALSE，如果尚未初始化。
 * 
 * 对于语言绑定，g_thread_get_initialized()提供与函数相同的功能。
 * 
 * 返回值：%TRUE，如果线程系统已初始化
 */

/* GThreadError {{{1 ------------------------------------------------------- */
/**
 * GThreadError（线程错误）：
 * @G_THREAD_ERROR_AGAIN：由于资源短缺，无法创建线程。稍后重试。
 *
 * 线程相关函数的可能错误。
 **/

/**
 * G_THREAD_ERROR:
 *
 * The error domain of the GLib thread subsystem.
 **/
G_DEFINE_QUARK (g_thread_error, g_thread_error)

/* Local Data {{{1 -------------------------------------------------------- */

static GMutex    g_once_mutex;
static GCond     g_once_cond;
static GSList   *g_once_init_list = NULL;

static guint g_thread_n_created_counter = 0;  /* (atomic) */

static void g_thread_cleanup (gpointer data);
static GPrivate     g_thread_specific_private = G_PRIVATE_INIT (g_thread_cleanup);

/*
 * g_private_set_alloc0:
 * @key: a #GPrivate
 * @size: size of the allocation, in bytes
 *
 * Sets the thread local variable @key to have a newly-allocated and zero-filled
 * value of given @size, and returns a pointer to that memory. Allocations made
 * using this API will be suppressed in valgrind: it is intended to be used for
 * one-time allocations which are known to be leaked, such as those for
 * per-thread initialisation data. Otherwise, this function behaves the same as
 * g_private_set().
 *
 * Returns: (transfer full): new thread-local heap allocation of size @size
 * Since: 2.60
 */
/*< private >*/
gpointer
g_private_set_alloc0 (GPrivate *key,
                      gsize     size)
{
  gpointer allocated = g_malloc0 (size);

  g_private_set (key, allocated);

  return g_steal_pointer (&allocated);
}

/* GOnce {{{1 ------------------------------------------------------------- */

/**
 * GOnce:
 * @status: the status of the #GOnce
 * @retval: the value returned by the call to the function, if @status
 *          is %G_ONCE_STATUS_READY
 *
 * A #GOnce struct controls a one-time initialization function. Any
 * one-time initialization function must have its own unique #GOnce
 * struct.
 *
 * Since: 2.4
 */

/**
 * G_ONCE_INIT:
 *
 * A #GOnce must be initialized with this macro before it can be used.
 *
 * |[<!-- language="C" --> 
 *   GOnce my_once = G_ONCE_INIT;
 * ]|
 *
 * Since: 2.4
 */

/**
 * GOnceStatus:
 * @G_ONCE_STATUS_NOTCALLED: the function has not been called yet.
 * @G_ONCE_STATUS_PROGRESS: the function call is currently in progress.
 * @G_ONCE_STATUS_READY: the function has been called.
 *
 * The possible statuses of a one-time initialization function
 * controlled by a #GOnce struct.
 *
 * Since: 2.4
 */

/**
 * g_once:
 * @once: a #GOnce structure
 * @func: the #GThreadFunc function associated to @once. This function
 *        is called only once, regardless of the number of times it and
 *        its associated #GOnce struct are passed to g_once().
 * @arg: data to be passed to @func
 *
 * The first call to this routine by a process with a given #GOnce
 * struct calls @func with the given argument. Thereafter, subsequent
 * calls to g_once()  with the same #GOnce struct do not call @func
 * again, but return the stored result of the first call. On return
 * from g_once(), the status of @once will be %G_ONCE_STATUS_READY.
 *
 * For example, a mutex or a thread-specific data key must be created
 * exactly once. In a threaded environment, calling g_once() ensures
 * that the initialization is serialized across multiple threads.
 *
 * Calling g_once() recursively on the same #GOnce struct in
 * @func will lead to a deadlock.
 *
 * |[<!-- language="C" --> 
 *   gpointer
 *   get_debug_flags (void)
 *   {
 *     static GOnce my_once = G_ONCE_INIT;
 *
 *     g_once (&my_once, parse_debug_flags, NULL);
 *
 *     return my_once.retval;
 *   }
 * ]|
 *
 * Since: 2.4
 */
gpointer
g_once_impl (GOnce       *once,
	     GThreadFunc  func,
	     gpointer     arg)
{
  g_mutex_lock (&g_once_mutex);

  while (once->status == G_ONCE_STATUS_PROGRESS)
    g_cond_wait (&g_once_cond, &g_once_mutex);

  if (once->status != G_ONCE_STATUS_READY)
    {
      gpointer retval;

      once->status = G_ONCE_STATUS_PROGRESS;
      g_mutex_unlock (&g_once_mutex);

      retval = func (arg);

      g_mutex_lock (&g_once_mutex);
/* We prefer the new C11-style atomic extension of GCC if available. If not,
 * fall back to always locking. */
#if defined(G_ATOMIC_LOCK_FREE) && defined(__GCC_HAVE_SYNC_COMPARE_AND_SWAP_4) && defined(__ATOMIC_SEQ_CST)
      /* Only the second store needs to be atomic, as the two writes are related
       * by a happens-before relationship here. */
      once->retval = retval;
      __atomic_store_n (&once->status, G_ONCE_STATUS_READY, __ATOMIC_RELEASE);
#else
      once->retval = retval;
      once->status = G_ONCE_STATUS_READY;
#endif
      g_cond_broadcast (&g_once_cond);
    }

  g_mutex_unlock (&g_once_mutex);

  return once->retval;
}

/**
 * g_once_init_enter:
 * @location: (not nullable): location of a static initializable variable
 *    containing 0
 *
 * Function to be called when starting a critical initialization
 * section. The argument @location must point to a static
 * 0-initialized variable that will be set to a value other than 0 at
 * the end of the initialization section. In combination with
 * g_once_init_leave() and the unique address @value_location, it can
 * be ensured that an initialization section will be executed only once
 * during a program's life time, and that concurrent threads are
 * blocked until initialization completed. To be used in constructs
 * like this:
 *
 * |[<!-- language="C" --> 
 *   static gsize initialization_value = 0;
 *
 *   if (g_once_init_enter (&initialization_value))
 *     {
 *       gsize setup_value = 42; // initialization code here
 *
 *       g_once_init_leave (&initialization_value, setup_value);
 *     }
 *
 *   // use initialization_value here
 * ]|
 *
 * While @location has a `volatile` qualifier, this is a historical artifact and
 * the pointer passed to it should not be `volatile`.
 *
 * Returns: %TRUE if the initialization section should be entered,
 *     %FALSE and blocks otherwise
 *
 * Since: 2.14
 */
gboolean
(g_once_init_enter) (volatile void *location)
{
  gsize *value_location = (gsize *) location;
  gboolean need_init = FALSE;
  g_mutex_lock (&g_once_mutex);
  if (g_atomic_pointer_get (value_location) == 0)
    {
      if (!g_slist_find (g_once_init_list, (void*) value_location))
        {
          need_init = TRUE;
          g_once_init_list = g_slist_prepend (g_once_init_list, (void*) value_location);
        }
      else
        do
          g_cond_wait (&g_once_cond, &g_once_mutex);
        while (g_slist_find (g_once_init_list, (void*) value_location));
    }
  g_mutex_unlock (&g_once_mutex);
  return need_init;
}

/**
 * g_once_init_leave:
 * @location: (not nullable): location of a static initializable variable
 *    containing 0
 * @result: new non-0 value for *@value_location
 *
 * Counterpart to g_once_init_enter(). Expects a location of a static
 * 0-initialized initialization variable, and an initialization value
 * other than 0. Sets the variable to the initialization value, and
 * releases concurrent threads blocking in g_once_init_enter() on this
 * initialization variable.
 *
 * While @location has a `volatile` qualifier, this is a historical artifact and
 * the pointer passed to it should not be `volatile`.
 *
 * Since: 2.14
 */
void
(g_once_init_leave) (volatile void *location,
                     gsize          result)
{
  gsize *value_location = (gsize *) location;
  gsize old_value;

  g_return_if_fail (result != 0);

  old_value = (gsize) g_atomic_pointer_exchange (value_location, result);
  g_return_if_fail (old_value == 0);

  g_mutex_lock (&g_once_mutex);
  g_return_if_fail (g_once_init_list != NULL);
  g_once_init_list = g_slist_remove (g_once_init_list, (void*) value_location);
  g_cond_broadcast (&g_once_cond);
  g_mutex_unlock (&g_once_mutex);
}

/* GThread {{{1 -------------------------------------------------------- */

/**
 * g_thread_ref:
 * @thread: a #GThread
 *
 * Increase the reference count on @thread.
 *
 * Returns: (transfer full): a new reference to @thread
 *
 * Since: 2.32
 */
GThread *
g_thread_ref (GThread *thread)
{
  GRealThread *real = (GRealThread *) thread;

  g_atomic_int_inc (&real->ref_count);

  return thread;
}

/**
 * g_thread_unref:
 * @thread: (transfer full): a #GThread
 *
 * Decrease the reference count on @thread, possibly freeing all
 * resources associated with it.
 *
 * Note that each thread holds a reference to its #GThread while
 * it is running, so it is safe to drop your own reference to it
 * if you don't need it anymore.
 *
 * Since: 2.32
 */
void
g_thread_unref (GThread *thread)
{
  GRealThread *real = (GRealThread *) thread;

  if (g_atomic_int_dec_and_test (&real->ref_count))
    {
      if (real->ours)
        g_system_thread_free (real);
      else
        g_slice_free (GRealThread, real);
    }
}

static void
g_thread_cleanup (gpointer data)
{
  g_thread_unref (data);
}

gpointer
g_thread_proxy (gpointer data)
{
  GRealThread* thread = data;

  g_assert (data);
  g_private_set (&g_thread_specific_private, data);

  TRACE (GLIB_THREAD_SPAWNED (thread->thread.func, thread->thread.data,
                              thread->name));

  if (thread->name)
    {
      g_system_thread_set_name (thread->name);
      g_free (thread->name);
      thread->name = NULL;
    }

  thread->retval = thread->thread.func (thread->thread.data);

  return NULL;
}

guint
g_thread_n_created (void)
{
  return g_atomic_int_get (&g_thread_n_created_counter);
}

/**
 * g_thread_new:
 * @name: （可选）新线程的名称
 * @func: （闭包数据）（异步范围）要在新线程中执行的函数
 * @data: （可选）要提供给新线程的参数
 *
 * 此函数创建一个新线程。新线程通过使用参数 data 调用 @func 来启动。
 * 线程将运行直到 @func 返回或者从新线程调用 g_thread_exit()。
 * @func的返回值成为线程的返回值，可以通过 g_thread_join() 获取。
 *
 * @name 可以用于在调试器中区分线程。它不用于其他目的，也不必是唯一的。某些系统限制 @name 的长度为 16 字节。
 *
 * 如果无法创建线程，程序将中止。如果希望处理创建失败的情况，请参阅 g_thread_try_new()。
 *
 * 如果你正在使用线程来卸载（可能是许多）短暂任务，#GThreadPool 可能比手动生成和跟踪多个 #GThreads 更合适。
 *
 * 要释放此函数返回的结构，请使用 g_thread_unref()。请注意，g_thread_join() 也会隐式地取消引用 #GThread。
 *
 * 默认情况下，新线程继承其创建新线程的线程的调度策略（POSIX）或线程优先级（Windows）。
 *
 * 此行为在 GLib 2.64 中有所改变：在此之前，Windows 上的线程没有继承线程优先级，而是使用默认优先级生成。
 * 从 GLib 2.64 开始，Windows 和 POSIX 之间的行为现在是一致的，所有线程都继承其父线程的优先级。
 *
 * 返回值：（完全转移）新的 #GThread
 *
 * 自版本：2.32
 */
GThread *
g_thread_new (const gchar *name,
              GThreadFunc  func,
              gpointer     data)
{
  GError *error = NULL;
  GThread *thread;

  thread = g_thread_new_internal (name, g_thread_proxy, func, data, 0, &error);

  if G_UNLIKELY (thread == NULL)
    g_error ("creating thread '%s': %s", name ? name : "", error->message);

  return thread;
}

/**
 * g_thread_try_new:
 * @name: （可选）新线程的名称
 * @func: （闭包数据）（异步范围）要在新线程中执行的函数
 * @data: （可选）要提供给新线程的参数
 * @error: 错误返回位置，或 %NULL
 *
 * 此函数与 g_thread_new() 相同，但允许出现失败的可能性。
 *
 * 如果无法创建线程（由于资源限制），将设置 @error 并返回 %NULL。
 *
 * 返回值：（完全转移）新的 #GThread，如果发生错误则为 %NULL
 *
 * 自版本：2.32
 */
GThread *
g_thread_try_new (const gchar  *name,
                  GThreadFunc   func,
                  gpointer      data,
                  GError      **error)
{
  return g_thread_new_internal (name, g_thread_proxy, func, data, 0, error);
}

GThread *
g_thread_new_internal (const gchar *name,
                       GThreadFunc proxy,
                       GThreadFunc func,
                       gpointer data,
                       gsize stack_size,
                       GError **error)
{
  g_return_val_if_fail (func != NULL, NULL);

  g_atomic_int_inc (&g_thread_n_created_counter);

  g_trace_mark (G_TRACE_CURRENT_TIME, 0, "GLib", "GThread created", "%s", name ? name : "(unnamed)");
  return (GThread *) g_system_thread_new (proxy, stack_size, name, func, data, error);
}

/**
 * g_thread_exit:
 * @retval: 此线程的返回值
 *
 * 终止当前线程。（特别注意是 退出运行当前函数的线程）
 *
 * 如果另一个线程正在使用 g_thread_join() 等待我们，等待的线程将被唤醒，并将 @retval 作为 g_thread_join() 的返回值。
 *
 * 使用参数 @retval 调用 g_thread_exit() 相当于从传递给 g_thread_new() 的函数 @func 中返回 @retval。
 *
 * 你只能从使用 g_thread_new() 或相关 API 创建的线程中调用 g_thread_exit()。不能从使用另一个线程库创建的线程或 #GThreadPool 中调用此函数。
 */
void
g_thread_exit (gpointer retval)
{
  GRealThread* real = (GRealThread*) g_thread_self ();

  if G_UNLIKELY (!real->ours)
    g_error ("attempt to g_thread_exit() a thread not created by GLib");

  real->retval = retval;

  g_system_thread_exit ();
}

/**
 * g_thread_join:
 * @thread: （完全转移）一个 #GThread
 *
 * 等待 @thread 完成，即给定给 g_thread_new() 的函数 @func 返回或调用 g_thread_exit()。
 * 如果 @thread 已经终止，则 g_thread_join() 立即返回。
 *
 * 任何线程都可以通过调用 g_thread_join() 等待任何其他线程，而不仅仅是其“创建者”。
 * 对于同一个 @thread，从多个线程调用 g_thread_join() 会导致未定义的行为。
 *
 * @func 返回的值或给定给 g_thread_exit() 的值将作为此函数的返回值。
 *
 * g_thread_join() 消耗了传入的 @thread 的引用。这通常会导致释放 #GThread 结构和相关资源。
 * 如果想在 g_thread_join() 调用之后保持 GThread 活动，请使用 g_thread_ref() 获取额外的引用。
 *
 * 返回值：（完全转移）线程的返回值
 */
gpointer
g_thread_join (GThread *thread)
{
  GRealThread *real = (GRealThread*) thread;
  gpointer retval;

  g_return_val_if_fail (thread, NULL);
  g_return_val_if_fail (real->ours, NULL);

  g_system_thread_wait (real);

  retval = real->retval;

  /* Just to make sure, this isn't used any more */
  thread->joinable = 0;

  g_thread_unref (thread);

  return retval;
}

/**
 * g_thread_self:
 *
 * 此函数返回当前线程对应的 #GThread。请注意，此函数不增加返回结构体的引用计数。
 *
 * 此函数将为即使不是由 GLib 创建的线程（即由其他线程 API 创建的线程）返回一个 #GThread。
 * 这可能对线程识别（比较）有用，但你不能在这些线程上使用 GLib 函数（例如 g_thread_join()）。
 *
 * 返回值：（无转移）代表当前线程的 #GThread
 */
GThread*
g_thread_self (void)
{
  GRealThread* thread = g_private_get (&g_thread_specific_private);

  if (!thread)
    {
      /* If no thread data is available, provide and set one.
       * This can happen for the main thread and for threads
       * that are not created by GLib.
       */
      thread = g_slice_new0 (GRealThread);
      thread->ref_count = 1;

      g_private_set (&g_thread_specific_private, thread);
    }

  return (GThread*) thread;
}

/**
 * g_get_num_processors:
 *
 * 确定系统为此进程同时调度的线程数量的近似值。
 * 这旨在用作 g_thread_pool_new() 的参数，用于 CPU 密集型任务和类似情况。
 *
 * 返回值：可调度线程的数量，始终大于 0
 *
 * 自版本：2.36
 */
guint
g_get_num_processors (void)
{
#ifdef G_OS_WIN32
  unsigned int count;
  SYSTEM_INFO sysinfo;
  DWORD_PTR process_cpus;
  DWORD_PTR system_cpus;

  /* This *never* fails, use it as fallback */
  GetNativeSystemInfo (&sysinfo);
  count = (int) sysinfo.dwNumberOfProcessors;

  if (GetProcessAffinityMask (GetCurrentProcess (),
                              &process_cpus, &system_cpus))
    {
      unsigned int af_count;

      for (af_count = 0; process_cpus != 0; process_cpus >>= 1)
        if (process_cpus & 1)
          af_count++;

      /* Prefer affinity-based result, if available */
      if (af_count > 0)
        count = af_count;
    }

  if (count > 0)
    return count;
#elif defined(_SC_NPROCESSORS_ONLN)
  {
    int count;

    count = sysconf (_SC_NPROCESSORS_ONLN);
    if (count > 0)
      return count;
  }
#elif defined HW_NCPU
  {
    int mib[2], count = 0;
    size_t len;

    mib[0] = CTL_HW;
    mib[1] = HW_NCPU;
    len = sizeof(count);

    if (sysctl (mib, 2, &count, &len, NULL, 0) == 0 && count > 0)
      return count;
  }
#endif

  return 1; /* Fallback */
}

/* Epilogue {{{1 */
/* vim: set foldmethod=marker: */