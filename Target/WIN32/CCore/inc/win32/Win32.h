/* Win32.h */
//----------------------------------------------------------------------------------------
//
//  Project: CCore 4.01
//
//  Tag: Target/WIN32
//
//  License: Boost Software License - Version 1.0 - August 17th, 2003
//
//            see http://www.boost.org/LICENSE_1_0.txt or the local copy
//
//  Copyright (c) 2020 Sergey Strukov. All rights reserved.
//
//----------------------------------------------------------------------------------------

#ifndef CCore_inc_win32_Win32_h
#define CCore_inc_win32_Win32_h

#include <CCore/inc/win32/Win32Types.h>

#define WIN32_CALLTYPE   __stdcall

#define WIN32_API        __declspec(dllimport) WIN32_CALLTYPE

namespace Win32 {

/*--------------------------------------------------------------------------------------*/
/* basic types                                                                          */
/*--------------------------------------------------------------------------------------*/

/* type proc_t */

using proc_t = void (WIN32_CALLTYPE *)(void) ;

/*--------------------------------------------------------------------------------------*/
/* common structures                                                                    */
/*--------------------------------------------------------------------------------------*/

/* struct SecurityAttributes */

struct SecurityAttributes
 {
  ulen_t cb;

  void_ptr desc;
  bool_t inherit;
 };

/* struct Overlapped */

struct Overlapped
 {
  file_len_t internal;
  file_len_t offset;
  handle_t h_event;
 };

/* struct Coord */

struct Coord
 {
  short x;
  short y;
 };

/*--------------------------------------------------------------------------------------*/
/* functions                                                                            */
/*--------------------------------------------------------------------------------------*/

extern "C" {

/*--------------------------------------------------------------------------------------*/
/* Error codes                                                                          */
/*--------------------------------------------------------------------------------------*/

/* enum ErrorCodes */

enum ErrorCodes
 {
  ErrorFileNotFound     =   2,
  ErrorPathNotFound     =   3,
  ErrorTooManyOpenFiles =   4,
  ErrorAccessDenied     =   5,
  ErrorInvalidHandle    =   6,
  ErrorNotEnoughMemory  =   8,
  ErrorOutOfMemory      =  14,
  ErrorInvalidDrive     =  15,
  ErrorNoMoreFiles      =  18,
  ErrorWriteProtect     =  19,
  ErrorSeek             =  25,
  ErrorWriteFault       =  29,
  ErrorReadFault        =  30,
  ErrorHandleEOF        =  38,
  ErrorFileExists       =  80,
  ErrorInvalidParameter =  87,
  ErrorBrokenPipe       = 109,
  ErrorDiskFull         = 112,
  ErrorSmallBuffer      = 122,
  ErrorInvalidName      = 123,
  ErrorDirNotEmpty      = 145,
  ErrorAlreadyExists    = 183,
  ErrorEnvNotFound      = 203,

  ErrorIOPending        = 997,
  WSAErrorIOPending     = ErrorIOPending
 };

/*--------------------------------------------------------------------------------------*/
/* Format error message flags                                                           */
/*--------------------------------------------------------------------------------------*/

/* enum FormatMessageFlags */

enum FormatMessageFlags
 {
  FormatMessageFromSystem    = 0x1000,
  FormatMessageIgnoreInserts = 0x0200,
  FormatMessageMaxWidthMask  = 0x00FF
 };

/*--------------------------------------------------------------------------------------*/
/* Error functions                                                                      */
/*--------------------------------------------------------------------------------------*/

/* GetLastError() */

error_t WIN32_API GetLastError(void);

/* FormatMessageW() */

ulen_t WIN32_API FormatMessageW(flags_t format_message_flags,
                                handle_t,
                                error_t code,
                                unsigned,
                                wchar *buf,
                                ulen_t buf_len,
                                void_ptr);

/*--------------------------------------------------------------------------------------*/
/* Character flags                                                                      */
/*--------------------------------------------------------------------------------------*/

/* enum CodePageId */

enum CodePageId
 {
  CodePageActive =     0,
  CodePageOEM    =     1,
  CodePageMAC    =     2,
  CodePageThread =     3,
  CodePageSymbol =    42,
  CodePageUTF7   = 65000,
  CodePageUTF8   = 65001
 };

/* enum MultiByteFlags */

enum MultiByteFlags
 {
  MultiBytePrecomposed       = 0x0001,
  MultiByteComposite         = 0x0002,
  MultiByteUseGlyphChars     = 0x0004,
  MultiByteErrorInvalidChars = 0x0008
 };

/*--------------------------------------------------------------------------------------*/
/* Character functions                                                                  */
/*--------------------------------------------------------------------------------------*/

/* MultiByteToWideChar() */

int WIN32_API MultiByteToWideChar(codepage_t codepage,
                                  flags_t flags,
                                  const char *str,
                                  int str_len,
                                  wchar *out,
                                  int out_len);

/* CharLowerW() */

wchar * WIN32_API CharLowerW(wchar *str);

/*--------------------------------------------------------------------------------------*/
/* System information structures                                                        */
/*--------------------------------------------------------------------------------------*/

/* struct SystemInfo */

struct SystemInfo
 {
  unsigned short cpu_arch;
  unsigned short reserved1;
  unsigned page_len;
  void_ptr min_address;
  void_ptr max_address;
  unsigned cpu_mask;
  unsigned cpu_count;
  unsigned cpu_type;
  unsigned alloc_granularity;
  unsigned short cpu_level;
  unsigned short cpu_revision;
 };

/*--------------------------------------------------------------------------------------*/
/* System information functions                                                         */
/*--------------------------------------------------------------------------------------*/

/* GetSystemInfo() */

void WIN32_API GetSystemInfo(SystemInfo *info);

/*--------------------------------------------------------------------------------------*/
/* Time functions                                                                       */
/*--------------------------------------------------------------------------------------*/

/* GetTickCount() */

timeout_t WIN32_API GetTickCount(void);

/* GetSystemTimeAsFileTime() */

void WIN32_API GetSystemTimeAsFileTime(file_time_t *ret);

/*--------------------------------------------------------------------------------------*/
/* Virtual memory flags                                                                 */
/*--------------------------------------------------------------------------------------*/

/* enum AllocFlags */

enum AllocFlags
 {
  AllocReserve = 0x2000,
  AllocCommit  = 0x1000
 };

/* enum PageFlags */

enum PageFlags
 {
  PageReadWrite = 0x0004
 };

/* enum FreeFlags */

enum FreeFlags
 {
  FreeRelease  = 0x8000
 };

/*--------------------------------------------------------------------------------------*/
/* Virtual memory functions                                                             */
/*--------------------------------------------------------------------------------------*/

/* VirtualAlloc() */

void_ptr WIN32_API VirtualAlloc(void_ptr address,
                                ulen_t len,
                                flags_t alloc_flags,
                                flags_t page_flags);

/* VirtualFree() */

bool_t WIN32_API VirtualFree(void_ptr address,
                             ulen_t len,
                             flags_t free_flags);

/*--------------------------------------------------------------------------------------*/
/* Handle constants                                                                     */
/*--------------------------------------------------------------------------------------*/

enum HandleFlags
 {
  HandleInherit = 0x0001,
  HandleNoClose = 0x0002
 };

/*--------------------------------------------------------------------------------------*/
/* Handle functions                                                                     */
/*--------------------------------------------------------------------------------------*/

/* CloseHandle() */

bool_t WIN32_API CloseHandle(handle_t h_any);

/* SetHandleInformation() */

bool_t WIN32_API SetHandleInformation(handle_t h_any,flags_t mask,flags_t flags);

/*--------------------------------------------------------------------------------------*/
/* Global memory constants                                                              */
/*--------------------------------------------------------------------------------------*/

enum GMemFlags
 {
  GMemMovable  = 0x0002,
  GMemZeroInit = 0x0040
 };

/*--------------------------------------------------------------------------------------*/
/* Global memory functions                                                              */
/*--------------------------------------------------------------------------------------*/

handle_t WIN32_API GlobalAlloc(flags_t flags, ulen_t len);

handle_t WIN32_API GlobalFree(handle_t h_mem);

ulen_t WIN32_API GlobalSize(handle_t h_mem);

void_ptr WIN32_API GlobalLock(handle_t h_mem);

bool_t WIN32_API GlobalUnlock(handle_t h_mem);

/*--------------------------------------------------------------------------------------*/
/* Wait constants                                                                       */
/*--------------------------------------------------------------------------------------*/

/* const TryTimeout */

inline constexpr timeout_t TryTimeout = timeout_t(0) ;

/* const NoTimeout */

inline constexpr timeout_t NoTimeout = timeout_t(-1) ;

/* enum WaitOptions */

enum WaitOptions
 {
  WaitFailed     =  -1,
  WaitObject_0   =   0,
  WaitTimeout    = 258
 };

/*--------------------------------------------------------------------------------------*/
/* Wait functions                                                                       */
/*--------------------------------------------------------------------------------------*/

/* WaitForSingleObject() */

options_t WIN32_API WaitForSingleObject(handle_t h_any, timeout_t timeout);

/* WaitForMultipleObjects() */

options_t WIN32_API WaitForMultipleObjects(ulen_t hcount,
                                           handle_t hlist[/* hcount */],
                                           bool_t wait_all,
                                           timeout_t timeout);

/*--------------------------------------------------------------------------------------*/
/* DLL functions                                                                        */
/*--------------------------------------------------------------------------------------*/

/* GetModuleHandleW() */

handle_t WIN32_API GetModuleHandleW(const wchar *module_name);

/* GetModuleFileNameW() */

ulen_t WIN32_API GetModuleFileNameW(handle_t h_module,
                                    wchar *buf,
                                    ulen_t len);

/*--------------------------------------------------------------------------------------*/
/* TLS constants                                                                        */
/*--------------------------------------------------------------------------------------*/

/* const TlsNoIndex */

inline constexpr index_t TlsNoIndex = index_t(-1) ;

/*--------------------------------------------------------------------------------------*/
/* TLS functions                                                                        */
/*--------------------------------------------------------------------------------------*/

/* TlsAlloc() */

index_t WIN32_API TlsAlloc(void);

/* TlsFree() */

bool_t WIN32_API TlsFree(index_t index);

/* TlsGetValue() */

void_ptr WIN32_API TlsGetValue(index_t index);

/* TlsSetValue() */

bool_t WIN32_API TlsSetValue(index_t index, void_ptr value);

/*--------------------------------------------------------------------------------------*/
/* Event functions                                                                      */
/*--------------------------------------------------------------------------------------*/

/* CreateEventW() */

handle_t WIN32_API CreateEventW(SecurityAttributes *,
                                bool_t manual_reset,
                                bool_t initial_state,
                                const wchar *object_name);

/* SetEvent() */

bool_t WIN32_API SetEvent(handle_t h_event);

/*--------------------------------------------------------------------------------------*/
/* Semaphore constants                                                                  */
/*--------------------------------------------------------------------------------------*/

/* const MaxSemaphoreCount */

inline constexpr sem_count_t MaxSemaphoreCount = 0x7FFF'FFFF ;

/*--------------------------------------------------------------------------------------*/
/* Semaphore functions                                                                  */
/*--------------------------------------------------------------------------------------*/

/* CreateSemaphoreW() */

handle_t WIN32_API CreateSemaphoreW(SecurityAttributes *,
                                    sem_count_t initial_count,
                                    sem_count_t max_count,
                                    const wchar *object_name);

/* ReleaseSemaphore() */

bool_t WIN32_API ReleaseSemaphore(handle_t h_sem,
                                  sem_count_t delta,
                                  sem_count_t *prev_count);

/*--------------------------------------------------------------------------------------*/
/* Process constants                                                                    */
/*--------------------------------------------------------------------------------------*/

/* enum ProcessCreationFlags */

enum ProcessCreationFlags
 {
  CreateNewConsole   = 0x0010,
  UnicodeEnvironment = 0x0400
 };

/* enum StartupInfoFlags */

enum StartupInfoFlags
 {
  StartupInfo_show_window = 0x0001,
  StartupInfo_std_handles = 0x0100
 };

/* enum ThreadPriority */

enum ThreadPriority
 {
  ThreadPriorityIdle         = -15,
  ThreadPriorityLowest       =  -2,
  ThreadPriorityLow          =  -1,
  ThreadPriorityNormal       =   0,
  ThreadPriorityHigh         =   1,
  ThreadPriorityHighest      =   2,
  ThreadPriorityTimeCritical =  15
 };

/*--------------------------------------------------------------------------------------*/
/* Process structures                                                                   */
/*--------------------------------------------------------------------------------------*/

/* struct StartupInfo */

struct StartupInfo
 {
  ulen_t cb;

  const wchar *reserved;
  const wchar *desktop;
  const wchar *title;

  int x;
  int y;
  int dx;
  int dy;

  unsigned dx_chars;
  unsigned dy_chars;

  flags_t fill_attr;
  flags_t flags;

  short show_window;
  short reserved2;

  void_ptr reserved3;

  handle_t h_stdin;
  handle_t h_stdout;
  handle_t h_stderr;
 };

/* struct ProcessInfo */

struct ProcessInfo
 {
  handle_t h_process;
  handle_t h_thread;
  numid_t process_id;
  numid_t thread_id;
 };

/*--------------------------------------------------------------------------------------*/
/* Process functions                                                                    */
/*--------------------------------------------------------------------------------------*/

/* GetEnvironmentStringsW() */

wchar * WIN32_API GetEnvironmentStringsW(void);

/* FreeEnvironmentStringsW() */

bool_t WIN32_API FreeEnvironmentStringsW(wchar *envblock);

/* GetEnvironmentVariableW() */

ulen_t WIN32_API GetEnvironmentVariableW(const wchar *name,
                                         wchar *buf,
                                         ulen_t len);

/* GetStartupInfoW() */

void WIN32_API GetStartupInfoW(StartupInfo *info);

/* GetCurrentProcess() */

handle_t WIN32_API GetCurrentProcess(void);

/* GetCurrentThread() */

handle_t WIN32_API GetCurrentThread(void);

/* TerminateProcess() */

bool_t WIN32_API TerminateProcess(handle_t h_process, unsigned exit_code);

/* Sleep() */

void WIN32_API Sleep(timeout_t timeout);

/* GetCurrentThreadId() */

numid_t WIN32_API GetCurrentThreadId(void);

/* CreateProcessW() */

bool_t WIN32_API CreateProcessW(const wchar *program,
                                wchar *arg,
                                SecurityAttributes *,
                                SecurityAttributes *,
                                bool_t inherit_handles,
                                flags_t process_creation_flags,
                                const wchar *envblock,
                                const wchar *dir,
                                StartupInfo *info,
                                ProcessInfo *pinfo);

/* GetExitCodeProcess() */

bool_t WIN32_API GetExitCodeProcess(handle_t h_process, unsigned *exit_code);

/* SetThreadPriority() */

bool_t WIN32_API SetThreadPriority(handle_t h_thread, options_t priority);

/*--------------------------------------------------------------------------------------*/
/* System property functions                                                            */
/*--------------------------------------------------------------------------------------*/

/* GetACP() */

codepage_t WIN32_API GetACP(void);

/*--------------------------------------------------------------------------------------*/
/* Console constants                                                                    */
/*--------------------------------------------------------------------------------------*/

/* enum ConsoleModes */

enum ConsoleModes
 {
  ConEcho      = 0x0004,
  ConIns       = 0x0020,
  ConLineInput = 0x0002
 };

/* enum ConEventType */

enum ConEventType
 {
  ConKeyEvent     =  1,
  ConMouse        =  2,
  ConBufsizeEvent =  4,
  ConMenuEvent    =  8,
  ConFocusEvent   = 16
 };

/*--------------------------------------------------------------------------------------*/
/* Console structures                                                                   */
/*--------------------------------------------------------------------------------------*/

/* struct ConKeyRecord */

struct ConKeyRecord
 {
  bool_t key_down;

  unsigned short repeat;
  unsigned short vkey;
  unsigned short vscan;

  union
   {
    unsigned short unicode;
    char ascii;
   } ch;

  flags_t control_state;
 };

/* struct ConMouseRecord */

struct ConMouseRecord
 {
  Coord pos;
  flags_t button_state;
  flags_t control_state;
  flags_t flags;
 };

/* struct ConMenuRecord */

struct ConMenuRecord
 {
  unsigned command_id;
 };

/* struct ConFocusRecord */

struct ConFocusRecord
 {
  bool_t set_focus;
 };

/* struct ConBufsizeRecord */

struct ConBufsizeRecord
 {
  Coord size;
 };

/* struct ConInputRecord */

struct ConInputRecord
 {
  unsigned short event_type;

  union
   {
    ConKeyRecord key;
    ConMouseRecord mouse;
    ConMenuRecord menu;
    ConFocusRecord focus;
    ConBufsizeRecord bufsize;
   } event;
 };

/*--------------------------------------------------------------------------------------*/
/* Console functions                                                                    */
/*--------------------------------------------------------------------------------------*/

/* SetConsoleOutputCP() */

bool_t WIN32_API SetConsoleOutputCP(codepage_t code_page);

/* GetConsoleOutputCP() */

codepage_t WIN32_API GetConsoleOutputCP(void);

/* SetConsoleCP() */

bool_t WIN32_API SetConsoleCP(codepage_t code_page);

/* GetConsoleCP() */

codepage_t WIN32_API GetConsoleCP(void);

/* SetConsoleMode() */

bool_t WIN32_API SetConsoleMode(handle_t h_con, flags_t modes);

/* GetConsoleMode() */

bool_t WIN32_API GetConsoleMode(handle_t h_con, flags_t *modes);

/* ReadConsoleInputW() */

bool_t WIN32_API ReadConsoleInputW(handle_t h_con,
                                   ConInputRecord *buf,
                                   ulen_t buf_len,
                                   ulen_t *ret_len);

/*--------------------------------------------------------------------------------------*/
/* Pipe functions                                                                       */
/*--------------------------------------------------------------------------------------*/

/* CreatePipe() */

bool_t WIN32_API CreatePipe(handle_t *h_read, handle_t *h_write, SecurityAttributes *sa, ulen_t buf_len);

/* PeekNamedPipe() */

bool_t WIN32_API PeekNamedPipe(handle_t h_read,
                               void_ptr buf,
                               ulen_t buf_len,
                               ulen_t *ret_len,
                               ulen_t *avail_len,
                               ulen_t *msgleft_len);

/*--------------------------------------------------------------------------------------*/
/* File flags and options                                                               */
/*--------------------------------------------------------------------------------------*/

/* const InvalidFileHandle */

inline constexpr handle_t InvalidFileHandle = -1 ;

/* enum StdHandleOptions */

enum StdHandleOptions
 {
  StdInputHandle  = -10,
  StdOutputHandle = -11
 };

/* enum AccessFlags */

enum AccessFlags : unsigned
 {
  AccessRead   = 0x8000'0000,
  AccessWrite  = 0x4000'0000,
  AccessDelete = 0x0001'0000
 };

/* enum ShareFlags */

enum ShareFlags
 {
  ShareRead  = 0x0001,
  ShareWrite = 0x0002
 };

/* enum CreationOptions */

enum CreationOptions        // +-------------+----------+
 {                          // | file exists | no file  |
                            // +-------------+----------+
  CreateNew        = 1,     // |  failure    |  create  |
  CreateAlways     = 2,     // |  overwrite  |  create  |
  OpenExisting     = 3,     // |  open       |  failure |
  OpenAlways       = 4,     // |  open       |  create  |
  TruncateExisting = 5      // |  overwrite  |  failure |
 };                         // +-------------+----------+

/* enum FileFlags */

enum FileFlags
 {
  FileAttributeNormal = 0x0000'0080,
  FileBackupSemantic  = 0x0200'0000,
  FileDeleteOnClose   = 0x0400'0000,
  FileSequentialScan  = 0x0800'0000,
  FileAsyncIO         = 0x4000'0000
 };

/* enum FromOptions */

enum FromOptions
 {
  FromBegin   = 0,
  FromCurrent = 1,
  FromEnd     = 2
 };

/* enum FileInfoOptions */

enum FileInfoOptions
 {
  FileDispositionInfo = 4
 };

/* enum FileAttributes */

enum FileAttributes
 {
  FileAttributes_Directory = 0x0010
 };

/* const InvalidFileAttributes */

inline constexpr flags_t InvalidFileAttributes = flags_t(-1) ;

/* enum MoveFileExFlags */

enum MoveFileExFlags
 {
  MoveFileEx_AllowOverwrite = 0x0001
 };

/*--------------------------------------------------------------------------------------*/
/* File structures                                                                      */
/*--------------------------------------------------------------------------------------*/

/* struct FileDispositionInfoData */

struct FileDispositionInfoData
 {
  bool_t delete_file;
 };

/*--------------------------------------------------------------------------------------*/
/* File functions                                                                       */
/*--------------------------------------------------------------------------------------*/

/* GetStdHandle() */

handle_t WIN32_API GetStdHandle(options_t std_handle_options);

/* CreateFileW() */

handle_t WIN32_API CreateFileW(const wchar *file_name,
                               flags_t access_flags,
                               flags_t share_flags,
                               SecurityAttributes *,
                               options_t creation_options,
                               flags_t file_flags,
                               handle_t);

/* WriteFile() */

bool_t WIN32_API WriteFile(handle_t h_file,
                           const_void_ptr buf,
                           ulen_t buf_len,
                           ulen_t *ret_len,
                           Overlapped *olap);

/* ReadFile() */

bool_t WIN32_API ReadFile(handle_t h_file,
                          void_ptr buf,
                          ulen_t buf_len,
                          ulen_t *ret_len,
                          Overlapped *olap);

/* GetOverlappedResult() */

bool_t WIN32_API GetOverlappedResult(handle_t h_file,
                                     Overlapped *olap,
                                     ulen_t *ret_len,
                                     bool_t wait_flag);

/* GetFileSizeEx() */

bool_t WIN32_API GetFileSizeEx(handle_t h_file, file_len_t *ret);

/* SetFilePointerEx() */

bool_t WIN32_API SetFilePointerEx(handle_t h_file,
                                  file_len_t to,
                                  file_len_t *ret,
                                  options_t from_options);

/* FlushFileBuffers() */

bool_t WIN32_API FlushFileBuffers(handle_t h_file);

/* SetFileInformationByHandle() */

bool_t WIN32_API SetFileInformationByHandle(handle_t h_file,
                                            options_t file_info_options,
                                            void_ptr buf,
                                            ulen_t buf_len);

/*--------------------------------------------------------------------------------------*/
/* File system structures                                                               */
/*--------------------------------------------------------------------------------------*/

/* struct FileTime */

struct FileTime
 {
  unsigned lo;
  unsigned hi;
 };

/* struct FindFileData */

struct FindFileData
 {
  flags_t attr;

  FileTime creation_time;
  FileTime last_access_time;
  FileTime last_write_time;

  ulen_t file_len_hi;
  ulen_t file_len_lo;

  flags_t reserved0;
  flags_t reserved1;

  wchar file_name[260];
  wchar alt_file_name[14];
 };

/*--------------------------------------------------------------------------------------*/
/* File system functions                                                                */
/*--------------------------------------------------------------------------------------*/

/* GetFileTime() */

bool_t WIN32_API GetFileTime(handle_t h_file, FileTime *creation_time,
                                              FileTime *last_access_time,
                                              FileTime *last_write_time);

/* GetFileAttributesW() */

flags_t WIN32_API GetFileAttributesW(const wchar *path);

/* DeleteFileW() */

bool_t WIN32_API DeleteFileW(const wchar *path);

/* CreateDirectoryW() */

bool_t WIN32_API CreateDirectoryW(const wchar *path, SecurityAttributes *);

/* RemoveDirectoryW() */

bool_t WIN32_API RemoveDirectoryW(const wchar *path);

/* MoveFileExW() */

bool_t WIN32_API MoveFileExW(const wchar *old_path, const wchar *new_path, flags_t flags);

/* FindFirstFileW() */

handle_t WIN32_API FindFirstFileW(const wchar *path, FindFileData *find_data);

/* FindNextFileW() */

bool_t WIN32_API FindNextFileW(handle_t h_find, FindFileData *find_data);

/* FindClose() */

bool_t WIN32_API FindClose(handle_t h_find);

/* GetFullPathNameW() */

ulen_t WIN32_API GetFullPathNameW(const wchar *path,
                                  ulen_t buf_len,
                                  wchar *buf,
                                  wchar **file_part);

/*--------------------------------------------------------------------------------------*/
/* Socket flags and options                                                             */
/*--------------------------------------------------------------------------------------*/

/* const InvalidSocket */

inline constexpr socket_t InvalidSocket = -1 ;

/* const InvalidULen */

inline constexpr ulen_t InvalidULen = ulen_t(-1) ;

/* enum WSAVersions */

enum WSAVersions
 {
  WSAVersion_2_00 = 0x0002,
  WSAVersion_2_02 = 0x0202
 };

/* enum WSAAddressFamily */

enum WSAAddressFamily
 {
  WSA_IPv4 = 2
 };

/* enum WSASocketType */

enum WSASocketType
 {
  WSA_Datagram = 2
 };

/* enum WSAProtocol */

enum WSAProtocol
 {
  WSA_UDP = 17
 };

/* enum WSASocketFlags */

enum WSASocketFlags
 {
  WSA_AsyncIO = 0x0001
 };

/* enum WSAWaitOptions */

enum WSAWaitOptions
 {
  WSAWaitFailed     =  -1,
  WSAWaitObject_0   =   0,
  WSAWaitTimeout    = 258
 };

/*--------------------------------------------------------------------------------------*/
/* Socket structures                                                                    */
/*--------------------------------------------------------------------------------------*/

/* type WSAGroup */

using WSAGroup = unsigned ;

/* type WSAVersion */

using WSAVersion = unsigned short ;

/* struct WSAInfo */

struct WSAInfo
 {
  WSAVersion version;
  WSAVersion hi_version;

  char desc[257];
  char status[129];

  // deprecated

  unsigned short max_sockets;
  unsigned short max_UDP_data_len;
  char *vendor_info;
 };

/* struct WSAProtocolInfo */

struct WSAProtocolInfo;

/* struct WSASockSet */

struct WSASockSet
 {
  ulen_t count;
  socket_t set[64];
 };

/* struct WSATimeout */

struct WSATimeout
 {
  unsigned sec;
  unsigned usec;
 };

/* struct WSAOverlapped */

struct WSAOverlapped
 {
  file_len_t internal;
  file_len_t offset;
  handle_t h_event;
 };

/* struct WSABuf */

struct WSABuf
 {
  ulen_t len;
  void_ptr ptr;
 };

/*--------------------------------------------------------------------------------------*/
/* Socket functions                                                                     */
/*--------------------------------------------------------------------------------------*/

/* WSAStartup() */

error_t WIN32_API WSAStartup(WSAVersion version, WSAInfo *info);

/* WSACleanup() */

negbool_t WIN32_API WSACleanup(void);

/* WSAGetLastError() */

error_t WIN32_API WSAGetLastError(void);

/* WSASocketW() */

socket_t WIN32_API WSASocketW(options_t address_family,
                              options_t type,
                              options_t protocol,
                              WSAProtocolInfo *,
                              WSAGroup,
                              flags_t flags);

/* bind() */

negbool_t WIN32_API bind(socket_t sock,
                         const_void_ptr address,
                         ulen_t address_len);

/* closesocket() */

negbool_t WIN32_API closesocket(socket_t sock);

/* sendto() */

ulen_t WIN32_API sendto(socket_t sock,
                        const_void_ptr data,
                        ulen_t data_len,
                        flags_t,
                        const_void_ptr address,
                        ulen_t address_len);

/* recvfrom() */

ulen_t WIN32_API recvfrom(socket_t sock,
                          void_ptr buf,
                          ulen_t buf_len,
                          flags_t,
                          void_ptr address,
                          ulen_t *address_len);

/* select() */

ulen_t WIN32_API select(int,
                        WSASockSet *read_set,
                        WSASockSet *write_set,
                        WSASockSet *error_set,
                        const WSATimeout *timeout);

/* WSASendTo() */

negbool_t WIN32_API WSASendTo(socket_t sock,
                              WSABuf *buf,
                              ulen_t buf_len,
                              ulen_t *ret_len,
                              flags_t,
                              const_void_ptr address,
                              ulen_t address_len,
                              WSAOverlapped *olap,
                              proc_t);

/* WSARecvFrom() */

negbool_t WIN32_API WSARecvFrom(socket_t sock,
                                WSABuf *buf,
                                ulen_t buf_len,
                                ulen_t *ret_len,
                                flags_t *ret_flags,
                                void_ptr address,
                                ulen_t *address_len,
                                WSAOverlapped *olap,
                                proc_t);

/* WSAGetOverlappedResult() */

bool_t WIN32_API WSAGetOverlappedResult(socket_t sock,
                                        WSAOverlapped *olap,
                                        ulen_t *ret_len,
                                        bool_t wait_flag,
                                        flags_t *ret_flags);

/* WSACreateEvent() */

handle_t WIN32_API WSACreateEvent(void);

/* WSACloseEvent() */

bool_t WIN32_API WSACloseEvent(handle_t h_event);

/* WSASetEvent() */

bool_t WIN32_API WSASetEvent(handle_t h_event);

/* WSAResetEvent() */

bool_t WIN32_API WSAResetEvent(handle_t h_event);

/* WSAWaitForMultipleEvents() */

options_t WIN32_API WSAWaitForMultipleEvents(ulen_t hcount,
                                             handle_t hlist[/* hcount */],
                                             bool_t wait_all,
                                             timeout_t timeout,
                                             bool_t);

} // extern "C"

} // namespace Win32

#endif


