/*  FTHREADS.H  (c) Copyright "Fish" (David B. Trout), 2001-2012     */
/*              Fish's WIN32 version of pthreads                     */
/*                                                                   */
/*   Released under "The Q Public License Version 1"                 */
/*   (http://www.hercules-390.org/herclic.html) as modifications to  */
/*   Hercules.                                                       */

#ifndef _FTHREADS_H_
#define _FTHREADS_H_

#include "hercules.h"

#ifndef _FTHREADS_C_
#ifndef _HUTIL_DLL_
#define FT_DLL_IMPORT DLL_IMPORT
#else   /* _HUTIL_DLL_ */
#define FT_DLL_IMPORT extern
#endif  /* _HUTIL_DLL_ */
#else
#define FT_DLL_IMPORT DLL_EXPORT
#endif

////////////////////////////////////////////////////////////////////////////////////
// Just a handy macro to have around...

#define RC(rc)  (rc)

////////////////////////////////////////////////////////////////////////////////////

#define MyInitializeCriticalSection(pCS)                (InitializeCriticalSectionAndSpinCount((CRITICAL_SECTION*)(pCS),3000))
#define MyEnterCriticalSection(pCS)                     (EnterCriticalSection((CRITICAL_SECTION*)(pCS)))
#define MyTryEnterCriticalSection(pCS)                  (TryEnterCriticalSection((CRITICAL_SECTION*)(pCS)))
#define MyLeaveCriticalSection(pCS)                     (LeaveCriticalSection((CRITICAL_SECTION*)(pCS)))
#define MyDeleteCriticalSection(pCS)                    (DeleteCriticalSection((CRITICAL_SECTION*)(pCS)))

#ifdef _MSVC_
#define MyCreateThread(sec,stack,start,parm,flags,tid)  ((HANDLE) _beginthreadex((sec),(unsigned)(stack),(start),(parm),(flags),(tid)))
#define MyExitThread(code)                              (_endthreadex((code)))
#else // (Cygwin)
#define MyCreateThread(sec,stack,start,parm,flags,tid)  (CreateThread((sec),(stack),(start),(parm),(flags),(tid)))
#define MyExitThread(code)                              (ExitThread((code)))
#endif // _MSVC_

#define MyCreateEvent(sec,man,set,name)                 (CreateEvent((sec),(man),(set),(name)))
#define MySetEvent(h)                                   (SetEvent((h)))
#define MyResetEvent(h)                                 (ResetEvent((h)))
#define MyDeleteEvent(h)                                (CloseHandle((h)))
#define MyCloseHandle(h)                                (CloseHandle((h)))

#define MyWaitForSingleObject(h,millisecs)              (WaitForSingleObject((h),(millisecs)))

////////////////////////////////////////////////////////////////////////////////////
// (need struct timespec for fthread_cond_timedwait)

#if !defined(TIMESPEC_IN_SYS_TYPES_H) && !defined(TIMESPEC_IN_TIME_H)
  // (need to define it ourselves)
  struct timespec
  {
      time_t  tv_sec;     // (seconds)
      long    tv_nsec;    // (nanoseconds)
  };
#endif

////////////////////////////////////////////////////////////////////////////////////
// fthread typedefs...

typedef void*                    FT_W32_HANDLE;       // HANDLE
typedef unsigned long            FT_W32_DWORD;        // DWORD

typedef FT_W32_DWORD             fthread_t;           // thread id
typedef FT_W32_DWORD             fthread_mutexattr_t; // mutex attribute

typedef void* (FT_THREAD_FUNC)(void*);                // thread function
typedef FT_THREAD_FUNC* PFT_THREAD_FUNC;              // thread function ptr

typedef struct _tagFTU_MUTEX        // fthread "mutex" structure
{
    FT_W32_HANDLE  hMutex;          // (ptr to actual mutex structure)
}
fthread_mutex_t;

typedef struct _tagFTU_COND         // fthread "condition variable" structure
{
    FT_W32_HANDLE  hCondVar;        // (ptr to actual condition variable structure)
}
fthread_cond_t;

typedef struct _tagFTU_ATTR         // fthread "thread attribute" structure
{
    size_t        nStackSize;       // (initial stack size in bytes)
    int           nDetachState;     // (requested detach state: detached/joinable)
}
fthread_attr_t;

////////////////////////////////////////////////////////////////////////////////////
// fthread thread attribute types...

#define  FTHREAD_CREATE_JOINABLE    0x4A6F696E                  // "Join" in ASCII
#define  FTHREAD_CREATE_DETACHED    0x44697363                  // "Disc" in ASCII
#define  FTHREAD_CREATE_DEFAULT     FTHREAD_CREATE_JOINABLE

////////////////////////////////////////////////////////////////////////////////////
// Initialize a "thread attribute"...

FT_DLL_IMPORT
int  fthread_attr_init
(
    fthread_attr_t*  pThreadAttr
);

////////////////////////////////////////////////////////////////////////////////////
// Destroy a "thread attribute"...

FT_DLL_IMPORT
int  fthread_attr_destroy
(
    fthread_attr_t*  pThreadAttr
);

////////////////////////////////////////////////////////////////////////////////////
// Set a thread's "detachstate" attribute...

FT_DLL_IMPORT
int  fthread_attr_setdetachstate
(
    fthread_attr_t*  pThreadAttr,
    int              nDetachState
);

////////////////////////////////////////////////////////////////////////////////////
// Retrieve a thread's "detachstate" attribute...

FT_DLL_IMPORT
int  fthread_attr_getdetachstate
(
    const fthread_attr_t*  pThreadAttr,
    int*                   pnDetachState
);

////////////////////////////////////////////////////////////////////////////////////
// Set a thread's initial stack size...

FT_DLL_IMPORT
int  fthread_attr_setstacksize
(
    fthread_attr_t*  pThreadAttr,
    size_t           nStackSize
);

////////////////////////////////////////////////////////////////////////////////////
// Retrieve a thread's initial stack size...

FT_DLL_IMPORT
int  fthread_attr_getstacksize
(
    const fthread_attr_t*  pThreadAttr,
    size_t*                pnStackSize
);

////////////////////////////////////////////////////////////////////////////////////
// Join a thread (i.e. wait for a thread's termination)...

FT_DLL_IMPORT
int  fthread_join
(
    fthread_t       dwThreadID,
    void**          pExitVal
);

////////////////////////////////////////////////////////////////////////////////////
// Detach a thread (i.e. ignore a thread's termination)...

FT_DLL_IMPORT
int  fthread_detach
(
    fthread_t  dwThreadID
);

/////////////////////////////////////////////////////////////////////////////////////
// Get the handle for a specific Thread ID

DLL_EXPORT
HANDLE fthread_get_handle
(
    fthread_t       dwThreadID             // Thread ID
);

////////////////////////////////////////////////////////////////////////////////////
// Create a new thread...

FT_DLL_IMPORT
int  fthread_create
(
    fthread_t*       pdwThreadID,
    fthread_attr_t*  pThreadAttr,
    PFT_THREAD_FUNC  pfnThreadFunc,
    void*            pvThreadArgs,
    const char*      pszThreadName
);

////////////////////////////////////////////////////////////////////////////////////
// Exit from a thread...

FT_DLL_IMPORT
void  fthread_exit
(
    void*  ExitVal
);

////////////////////////////////////////////////////////////////////////////////////
// Return thread-id...

FT_DLL_IMPORT
fthread_t  fthread_self
(
);

////////////////////////////////////////////////////////////////////////////////////
// Compare thread-ids...

FT_DLL_IMPORT
int  fthread_equal
(
    fthread_t  dwThreadID_1,
    fthread_t  dwThreadID_2
);

////////////////////////////////////////////////////////////////////////////////////
// (thread signalling not [currently] supported (yet); always returns ENOTSUP...)

FT_DLL_IMPORT
int  fthread_kill   // FIXME: TODO:
(
    int  dummy1,
    int  dummy2
);

////////////////////////////////////////////////////////////////////////////////////
// Initialize a "mutex"...

FT_DLL_IMPORT
int  fthread_mutex_init
(
          fthread_mutex_t*      pFT_MUTEX,
    const fthread_mutexattr_t*  pFT_MUTEX_ATTR
);

////////////////////////////////////////////////////////////////////////////////////
// Destroy a "mutex"...

FT_DLL_IMPORT
int  fthread_mutex_destroy
(
    fthread_mutex_t*  pFT_MUTEX
);

////////////////////////////////////////////////////////////////////////////////////
// Lock a "mutex"...

FT_DLL_IMPORT
int  fthread_mutex_lock
(
    fthread_mutex_t*  pFT_MUTEX
);

////////////////////////////////////////////////////////////////////////////////////
// Try to lock a "mutex"...

FT_DLL_IMPORT
int  fthread_mutex_trylock
(
    fthread_mutex_t*  pFT_MUTEX
);

////////////////////////////////////////////////////////////////////////////////////
// Unlock a "mutex"...

FT_DLL_IMPORT
int  fthread_mutex_unlock
(
    fthread_mutex_t*  pFT_MUTEX
);

////////////////////////////////////////////////////////////////////////////////////
// Initialize a "condition"...

FT_DLL_IMPORT
int  fthread_cond_init
(
    fthread_cond_t*  pFT_COND_VAR
);

////////////////////////////////////////////////////////////////////////////////////
// Destroy a "condition"...

FT_DLL_IMPORT
int  fthread_cond_destroy
(
    fthread_cond_t*  pFT_COND_VAR
);

////////////////////////////////////////////////////////////////////////////////////
// 'Signal' a "condition"...   (causes ONE waiting thread to be released)

FT_DLL_IMPORT
int  fthread_cond_signal
(
    fthread_cond_t*  pFT_COND_VAR
);

////////////////////////////////////////////////////////////////////////////////////
// 'Broadcast' a "condition"...  (causes ALL waiting threads to be released)

FT_DLL_IMPORT
int  fthread_cond_broadcast
(
    fthread_cond_t*  pFT_COND_VAR
);

////////////////////////////////////////////////////////////////////////////////////
// Wait for a "condition" to occur...

FT_DLL_IMPORT
int  fthread_cond_wait
(
    fthread_cond_t*   pFT_COND_VAR,
    fthread_mutex_t*  pFT_MUTEX
);

////////////////////////////////////////////////////////////////////////////////////
// Wait (but not forever) for a "condition" to occur...

FT_DLL_IMPORT
int  fthread_cond_timedwait
(
    fthread_cond_t*         pFT_COND_VAR,
    fthread_mutex_t*        pFT_MUTEX,
    const struct timespec*  pTimeTimeout
);

////////////////////////////////////////////////////////////////////////////////////
// fthread mutex attribute types...
//
//  FTHREAD_MUTEX_NORMAL      This type of mutex does not detect deadlock. A thread
//                            attempting to relock this mutex without first unlocking
//                            it shall deadlock. Attempting to unlock a mutex locked
//                            by a different thread results in undefined behavior.
//                            Attempting to unlock an unlocked mutex results in
//                            undefined behavior. The FTHREAD_MUTEX_NORMAL mutex type
//                            is not currently supported by fthreads.
//
//  FTHREAD_MUTEX_ERRORCHECK  This type of mutex provides error checking. A thread
//                            attempting to relock this mutex without first unlocking
//                            it shall return with an error. A thread attempting to
//                            unlock a mutex which another thread has locked shall
//                            return with an error. A thread attempting to unlock an
//                            unlocked mutex shall return with an error.
//
//  FTHREAD_MUTEX_RECURSIVE   A thread attempting to relock this mutex without first
//                            unlocking it shall succeed in locking the mutex. The
//                            relocking deadlock which can occur with mutexes of type
//                            FTHREAD_MUTEX_NORMAL cannot occur with this type of mutex.
//                            Multiple locks of this mutex shall require the same number
//                            of unlocks to release the mutex before another thread
//                            can acquire the mutex. A thread attempting to unlock a
//                            mutex which another thread has locked shall return with
//                            an error. A thread attempting to unlock an unlocked mutex
//                            shall return with an error.
//
//  FTHREAD_MUTEX_DEFAULT     Attempting to recursively lock a mutex of this type results
//                            in undefined behavior. Attempting to unlock a mutex of this
//                            type which was not locked by the calling thread results
//                            in undefined behavior. Attempting to unlock a mutex of this
//                            type which is not locked results in undefined behavior.
//                            An implementation may map this mutex to one of the other
//                            mutex types.

#define  FTHREAD_MUTEX_NORMAL       0x44656164                  // "Dead" in ASCII  (*UNSUPPORTED*)
#define  FTHREAD_MUTEX_ERRORCHECK   0x4F6E6365                  // "Once" in ASCII
#define  FTHREAD_MUTEX_RECURSIVE    0x4D616E79                  // "Many" in ASCII
#define  FTHREAD_MUTEX_DEFAULT      FTHREAD_MUTEX_ERRORCHECK

////////////////////////////////////////////////////////////////////////////////////
// Initialize a "mutex" attribute...

FT_DLL_IMPORT
int  fthread_mutexattr_init
(
    fthread_mutexattr_t*  pFT_MUTEX_ATTR
);

////////////////////////////////////////////////////////////////////////////////////
// Destroy a "mutex" attribute...

FT_DLL_IMPORT
int  fthread_mutexattr_destroy
(
    fthread_mutexattr_t*  pFT_MUTEX_ATTR
);

////////////////////////////////////////////////////////////////////////////////////
// Retrieve "mutex" attribute type...

FT_DLL_IMPORT
int fthread_mutexattr_gettype
(
    const fthread_mutexattr_t*  pFT_MUTEX_ATTR,
    int*                        pnMutexType
);

////////////////////////////////////////////////////////////////////////////////////
// Set "mutex" attribute type...

FT_DLL_IMPORT
int fthread_mutexattr_settype
(
    fthread_mutexattr_t*  pFT_MUTEX_ATTR,
    int                   nMutexType
);

////////////////////////////////////////////////////////////////////////////////////
// Thread Scheduling...

#define SCHED_FIFO       0          // First in, first out (FIFO) scheduling policy.
#define SCHED_RR         1          // Round robin scheduling policy.
#define SCHED_SPORADIC   2          // Sporadic server scheduling policy.
#define SCHED_OTHER      3          // Another scheduling policy.

#define FTHREAD_SCHED_POLICY    SCHED_RR    // Our only valid scheduling policy

struct sched_param                  // Scheduling parameters structure...
{
    int  sched_priority;            // Thread priority (see fthreads.c for range)
};
typedef struct sched_param sched_param;

FT_DLL_IMPORT  int  fthread_getschedparam ( fthread_t dwThreadID, int* pnPolicy,       struct sched_param* pSCHPARM );
FT_DLL_IMPORT  int  fthread_setschedparam ( fthread_t dwThreadID, int   nPolicy, const struct sched_param* pSCHPARM );
FT_DLL_IMPORT  int  fthread_setschedprio  ( fthread_t dwThreadID, int nPriority );
FT_DLL_IMPORT  int  fthread_get_min_prio  ( int nPolicy );
FT_DLL_IMPORT  int  fthread_get_max_prio  ( int nPolicy );

////////////////////////////////////////////////////////////////////////////////////

#endif // _FTHREADS_H_
