/*BHEADER**********************************************************************
 * (c) 1995   The Regents of the University of California
 *
 * See the file COPYRIGHT_and_DISCLAIMER for a complete copyright
 * notice, contact person, and disclaimer.
 *
 * $Revision: 1.1.1.1 $
 *********************************************************************EHEADER*/
#include "parflow_config.h"

#ifdef AMPS_MALLOC_DEBUG
#include <dmalloc.h>
#else
#ifdef AMPS_MALLOC_EFENCE
#include <efence.h>
#else
#ifdef AMPS_VMALLOC
#include <vmalloc.h>
#else
#include <stdlib.h>
#endif
#endif
#endif

#include <strings.h>
#include <stdio.h>
#include <sys/times.h>
#include <mpi.h>

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE  1
#endif

#ifndef max
#define max(a,b)  (((a)<(b)) ? (b) : (a))
#endif
#ifndef min
#define min(a,b)  (((a)<(b)) ? (a) : (b))
#endif

/*===========================================================================*/
/** 

  Since {\em AMPS} supports a threaded execution environment global and
  static variables must be treated as thread local for most programs
  to function correctly.  \Ref{amps_ThreadLocal} is used to access variables
  that are global to a node.

  @memo Global variable accessor function
  @param arg  variable to access [IN]
  @return value of the variable
*/
#define amps_ThreadLocal(arg) arg

/*===========================================================================*/
/** 

  Since {\em AMPS} supports a threaded execution environment global and
  static variables must be treated as thread local for most programs to
  function correctly.  \Ref{amps_ThreadLocalDcl} is used declare a
  variable which should have ``node scope''.  Each node will have get
  it's own variable using the \Ref{amps_ThreadLocalDcl} macro.
  Normally all global and static variables need to be declared and
  accessed using the {\em AMPS} routines.

  The \Ref{amps_ThreadLocal} macro is used to access variables 
  created with \Ref{amps_ThreadLocalDcl}.

\begin{verbatim}

// An integer which is to be global (thread local)
amps_ThreadLocalDcl(int, mine);

void foo(void)
{
	// set a global variable 
	amps_ThreadLocal(mine) = 5;
}

\end{verbatim}

  @memo Global variable declaration
  @param type type of the variable to create [IN]
  @param arg  variable to create [IN]
  @return none
*/
#define amps_ThreadLocalDcl(type, arg) type arg

/*===========================================================================*/
/**
The \Ref{amps_CommWorld} communicator provides a global communication
context that includes all the nodes.  It is created by the
\Ref{amps_Init} function and destroyed by \Ref{amps_Finalize}.

{\large Notes:}

Currently there is only the global communication context. 

@memo Global communication context
*/
#define amps_CommWorld MPI_COMM_WORLD

extern int amps_rank;
extern int amps_size;

/*===========================================================================*/
/**

Returns the rank of the node within the {\bf comm} communicator
context.  Ranks range from 0 to {\bf amps\_Size(comm) - 1}.  Ranks are
used to identify nodes in a similar manner to node numbers in other
message passing systems.  A node may have several ranks, one for each
context that it is a member of.

{\large Example:}
\begin{verbatim}
int my_rank;
my_rank = amps_Rank(amps_CommWorld);
\end{verbatim}

{\large Notes:}

Currently there is only a single communicator, \Ref{amps_CommWorld}.

@memo Node's rank in a communicator
@param comm Communication context
@return current node's rank in the communication context
*/
#define amps_Rank(comm) amps_rank

/*===========================================================================*/
/**

Returns the number of nodes that are members of the {\bf comm}
communicator.

{\large Example:}
\begin{verbatim}
int num;
num = amps_Size(amps_CommWorld);
\end{verbatim}

{\large Notes:}

@memo Number of nodes in a communicator
@param comm Communication context
@return number of nodes in the communication context
*/
#define amps_Size(comm) amps_size

/*===========================================================================*/
/**

Completes processing for a node.  This should be used only in 
error conditions.

{\large Notes:}

This may not exit cleanly on all platforms, user may have to
manually kill of dangling nodes.

@memo Exit a process
@param code Return code for the process [IN]
@return Never returns
*/
#define amps_Exit(code) exit(code)

#define amps_SyncOp 0
#define amps_Max MPI_MAX
#define amps_Min MPI_MIN
#define amps_Add MPI_SUM


#define AMPS_PID 0

/* These are the built-in types that are supported */
#define AMPS_INVOICE_CHAR_CTYPE                1
#define AMPS_INVOICE_SHORT_CTYPE               2
#define AMPS_INVOICE_INT_CTYPE                 3
#define AMPS_INVOICE_LONG_CTYPE                4
#define AMPS_INVOICE_DOUBLE_CTYPE              5
#define AMPS_INVOICE_FLOAT_CTYPE               6
#define AMPS_INVOICE_LAST_CTYPE                7

/* Flags for use with user-defined flag                                      */
#define AMPS_INVOICE_OVERLAY                   1

/* Flags for use with Pfmp_Invoice flag field                                 */
#define AMPS_INVOICE_USER_TYPE                 1

/* Flags for use with data types */
#define AMPS_INVOICE_CONSTANT 0
#define AMPS_INVOICE_POINTER 1
#define AMPS_INVOICE_DATA_POINTER 2

/* SGS ?????? following are misleading, OVERLAYED indicates overlayed or
   malloced, NON_OVERLAYED indicates all malloced non actually overlayed */
#define AMPS_INVOICE_ALLOCATED 1
#define AMPS_INVOICE_OVERLAYED 2
#define AMPS_INVOICE_NON_OVERLAYED 4

#ifdef CASC_HAVE_GETTIMEOFDAY
typedef long amps_Clock_t;
#define AMPS_TICKS_PER_SEC 10000
typedef clock_t amps_CPUClock_t;
extern long AMPS_CPU_TICKS_PER_SEC;
#endif 

#ifdef CRAY_TIME
typedef long amps_Clock_t;
typedef clock_t amps_CPUClock_t;
#define amps_Clock() rtclock()
#define amps_CPUClock() cpused()
#define AMPS_TICKS_PER_SEC 1.5E8
#define AMPS_CPU_TICKS_PER_SEC 1.5E8
#endif

#ifdef AMPS_NX_CLOCK
typedef double amps_Clock_t;
#define AMPS_TICKS_PER_SEC 1
typedef double amps_CPUClock_t;
#define AMPS_CPU_TICKS_PER_SEC 1
#define amps_CPUClock() 1
#endif

/* Default case, if not using a more specialized clock */
#ifndef AMPS_TICKS_PER_SEC
typedef long amps_Clock_t;
typedef clock_t amps_CPUClock_t;
extern long AMPS_CPU_TICKS_PER_SEC;
#endif

typedef MPI_Comm amps_Comm;
typedef FILE *amps_File;

extern int amps_tid;
extern int amps_rank;
extern int amps_size;


/* This structure is used to keep track of the entries in an invoice         */
typedef struct amps_invoicestruct
{
    long    flags;     /* some flags for this invoice */
    
    long   combuf_flags; /* flags indicating state of the communications
			    buffer */
    void   *combuf;    /* pointer to the communications buffer
			  associated with this invoice                       */
    
    struct amps_invoice_entry *list;
    struct amps_invoice_entry *end_list;
    int num;          /* number of items in the list                        */

    amps_Comm comm;

    MPI_Datatype mpi_type;
    int mpi_type_commited;
    
} amps_InvoiceStruct;

typedef amps_InvoiceStruct *amps_Invoice;
/* Each entry in the invoice has one of these                                */
typedef struct amps_invoice_entry
{
    int     type;         /* type that this invoice points to */
    
    long    flags;        /* flags indicating state of the invoice           */
    
    int     data_type;    /* what type of pointer do we have                 */
    long    data_flags;   /* flags indicating state of the data pointer      */
    void   *data;

    void   *extra;
    
    int     len_type;
    int     len;
    int    *ptr_len;
    
    int     stride_type;
    int     stride;
    int    *ptr_stride;

    int     dim_type;
    int     dim;
    int    *ptr_dim;
    
    int     ignore;      /* do we ignore this invoice?                       */
    
    struct amps_invoice_entry *next;

} amps_InvoiceEntry;

typedef struct amps_buffer
{
    struct amps_buffer *next, *prev;
    
    char *buffer;

} amps_Buffer;

/*===========================================================================*/
/* Package structure is used by the Exchange functions.  Contains several    */
/* Invoices plus the src or dest rank.                                       */
/*===========================================================================*/


#ifdef AMPS_MPI_NOT_USE_PERSISTENT

typedef struct
{
   int            num_send;
   int           *dest;
   amps_Invoice  *send_invoices;
   
   int            num_recv;
   int           *src;
   amps_Invoice  *recv_invoices;
   
   int            recv_remaining;
   
   MPI_Request     *requests;
      
} amps_PackageStruct;

typedef amps_PackageStruct *amps_Package;

#else

typedef struct
{
   int            num_send;
   int           *dest;
   amps_Invoice  *send_invoices;
   MPI_Request   *send_requests;
   
   int            num_recv;
   int           *src;
   amps_Invoice  *recv_invoices;
   MPI_Request   *recv_requests;
   
   MPI_Status    *status;
   
   int commited;

} amps_PackageStruct;

typedef amps_PackageStruct *amps_Package;

#endif


typedef struct _amps_HandleObject
{
   int type;
   amps_Comm comm;
   int id;
   amps_Invoice invoice;
   amps_Package package;

} amps_HandleObject;

typedef amps_HandleObject *amps_Handle;

extern amps_Buffer *amps_BufferList;
extern amps_Buffer *amps_BufferListEnd;
extern amps_Buffer *amps_BufferFreeList;

/* ****************************************************************************
 *
 *   PACKING structures and defines
 *
 *****************************************************************************/
 
#define AMPS_PACKED 2

#define AMPS_IGNORE  -1

#define PACK_HOST_TYPE 1
#define PACK_NO_CONVERT_TYPE 2

/*---------------------------------------------------------------------------*/
/* General functions to call methods for specified type                      */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Functions to for align                                                    */
/*---------------------------------------------------------------------------*/
#define AMPS_ALIGN(type, src, dest, len, stride) \
    ((sizeof(type) - \
      ((unsigned long)(dest) % sizeof(type))) \
     % sizeof(type));

#define AMPS_CALL_CHAR_ALIGN(_comm, _src, _dest, _len, _stride) \
        AMPS_ALIGN(char, (_src), (_dest), (_len), (_stride))

#define AMPS_CALL_SHORT_ALIGN(_comm, _src, _dest, _len, _stride) \
        AMPS_ALIGN(short, (_src), (_dest), (_len), (_stride))

#define AMPS_CALL_INT_ALIGN(_comm, _src, _dest, _len, _stride) \
        AMPS_ALIGN(int, (_src), (_dest), (_len), (_stride))

#define AMPS_CALL_LONG_ALIGN(_comm, _src, _dest, _len, _stride) \
        AMPS_ALIGN(long, (_src), (_dest), (_len), (_stride))

#define AMPS_CALL_FLOAT_ALIGN(_comm, _src, _dest, _len, _stride) \
        AMPS_ALIGN(float, (_src), (_dest), (_len), (_stride))

#define AMPS_CALL_DOUBLE_ALIGN(_comm, _src, _dest, _len, _stride) \
        AMPS_ALIGN(double, (_src), (_dest), (_len), (_stride))

/*---------------------------------------------------------------------------*/
/* Functions to for sizeof                                                   */
/*---------------------------------------------------------------------------*/
#define AMPS_SIZEOF(len, stride, size) \
         (len)*(size)

#define AMPS_CALL_CHAR_SIZEOF(_comm, _src, _dest, _len, _stride) \
        AMPS_SIZEOF((_len),(_stride), sizeof(char))

#define AMPS_CALL_SHORT_SIZEOF(_comm, _src, _dest, _len, _stride) \
        AMPS_SIZEOF((_len),(_stride), sizeof(short))

#define AMPS_CALL_INT_SIZEOF(_comm, _src, _dest, _len, _stride) \
        AMPS_SIZEOF((_len),(_stride), sizeof(int))

#define AMPS_CALL_LONG_SIZEOF(_comm, _src, _dest, _len, _stride) \
        AMPS_SIZEOF((_len),(_stride), sizeof(long))

#define AMPS_CALL_FLOAT_SIZEOF(_comm, _src, _dest, _len, _stride) \
        AMPS_SIZEOF((_len),(_stride), sizeof(float))

#define AMPS_CALL_DOUBLE_SIZEOF(_comm, _src, _dest, _len, _stride) \
        AMPS_SIZEOF((_len),(_stride), sizeof(double))

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
#define AMPS_CONVERT_OUT(type, cvt, comm, src, dest, len, stride) \
{ \
      type *ptr_src, *ptr_dest; \
      if((char *)(src) != (char*)(dest)) \
	  if((stride) == 1) \
	      bcopy((src), (dest), (len)*sizeof(type)); \
	  else \
	      for(ptr_src = (type*)(src), ptr_dest = (type*)(dest); ptr_src < (type*)(src) + (len)*(stride);\
		  ptr_src += (stride), ptr_dest++) \
     bcopy((ptr_src), (ptr_dest), sizeof(type)); \
}

#define AMPS_CALL_CHAR_OUT(_comm, _src, _dest, _len, _stride) \
    AMPS_CONVERT_OUT(char, ctohc, (_comm), (_src), (_dest), (_len),(_stride)) 

#define AMPS_CALL_SHORT_OUT(_comm, _src, _dest, _len, _stride) \
    AMPS_CONVERT_OUT(short, ctohs, (_comm), (_src), (_dest), (_len),(_stride)) 

#define AMPS_CALL_INT_OUT(_comm, _src, _dest, _len, _stride) \
    AMPS_CONVERT_OUT(int, ctohi, (_comm), (_src), (_dest), (_len),(_stride)) 

#define AMPS_CALL_LONG_OUT(_comm, _src, _dest, _len, _stride) \
    AMPS_CONVERT_OUT(long, ctohl, (_comm), (_src), (_dest), (_len),(_stride)) 

#define AMPS_CALL_FLOAT_OUT(_comm, _src, _dest, _len, _stride) \
    AMPS_CONVERT_OUT(float, ctohf, (_comm), (_src), (_dest), (_len),(_stride)) 

#define AMPS_CALL_DOUBLE_OUT(_comm, _src, _dest, _len, _stride) \
    AMPS_CONVERT_OUT(double, ctohd, (_comm), (_src), (_dest), (_len),(_stride)) 

#define AMPS_CONVERT_IN(type, cvt, comm, src, dest, len, stride) \
{ \
    char *ptr_src, *ptr_dest; \
    if((src) != (dest)) \
	if((stride) == 1) \
	    bcopy((src), (dest), (len)*sizeof(type)); \
	else \
	    for(ptr_src = (char*)(src), (ptr_dest) = (char *)(dest); \
		(ptr_dest) < (char *)(dest) + (len)*(stride)*sizeof(type);\
		(ptr_src) += sizeof(type), (ptr_dest) += sizeof(type)*(stride)) \
	    bcopy((ptr_src), (ptr_dest), sizeof(type)); \
} 

#define AMPS_CALL_CHAR_IN(_comm, _src, _dest, _len, _stride) \
    AMPS_CONVERT_IN(char, htocc, (_comm), (_src), (_dest), (_len),(_stride)) 

#define AMPS_CALL_SHORT_IN(_comm, _src, _dest, _len, _stride) \
    AMPS_CONVERT_IN(short, htocs, (_comm), (_src), (_dest), (_len),(_stride)) 

#define AMPS_CALL_INT_IN(_comm, _src, _dest, _len, _stride) \
    AMPS_CONVERT_IN(int, htoci, (_comm), (_src), (_dest), (_len),(_stride)) 

#define AMPS_CALL_LONG_IN(_comm, _src, _dest, _len, _stride) \
    AMPS_CONVERT_IN(long, htocl, (_comm), (_src), (_dest), (_len),(_stride)) 

#define AMPS_CALL_FLOAT_IN(_comm, _src, _dest, _len, _stride) \
    AMPS_CONVERT_IN(float, htocf, (_comm), (_src), (_dest), (_len),(_stride)) 

#define AMPS_CALL_DOUBLE_IN(_comm, _src, _dest, _len, _stride) \
    AMPS_CONVERT_IN(double, htocd, (_comm), (_src), (_dest), (_len),(_stride)) 

#define AMPS_CHECK_OVERLAY(_type, _comm) 0

#define AMPS_CHAR_OVERLAY(_comm) \
    AMPS_CHECK_OVERLAY(char, _comm)

#define AMPS_SHORT_OVERLAY(_comm) \
    AMPS_CHECK_OVERLAY(short, _comm)

#define AMPS_INT_OVERLAY(_comm) \
    AMPS_CHECK_OVERLAY(int, _comm)

#define AMPS_LONG_OVERLAY(_comm) \
    AMPS_CHECK_OVERLAY(long, _comm)

#define AMPS_FLOAT_OVERLAY(_comm) \
    AMPS_CHECK_OVERLAY(float, _comm)

#define AMPS_DOUBLE_OVERLAY(_comm) \
    AMPS_CHECK_OVERLAY(double, _comm)

/*---------------------------------------------------------------------------*/
/* Macros for Invoice creation and deletion.                                 */
/*---------------------------------------------------------------------------*/

#define amps_append_invoice amps_new_invoice

/*---------------------------------------------------------------------------*/
/* Internal macros used to clear buffer and letter spaces.                   */
/*---------------------------------------------------------------------------*/

#define AMPS_CLEAR_INVOICE(invoice) \
    { \
	  (invoice) -> combuf_flags &= ~AMPS_INVOICE_ALLOCATED; \
	  amps_ClearInvoice(invoice); \
    }

#define AMPS_PACK_FREE_LETTER(comm, invoice,amps_letter) \
    if( (invoice) -> combuf_flags & AMPS_INVOICE_OVERLAYED) \
        (invoice) -> combuf_flags |= AMPS_INVOICE_ALLOCATED; \
    else \
    { \
        (invoice) -> combuf_flags &= ~AMPS_INVOICE_ALLOCATED; \
        amps_free((comm), (amps_letter)); \
    } \

/**

\Ref{amps_ISend} is similar to \Ref{amps_Send} but it is guaranteed
not to block if the message cannot be sent immediately.
\Ref{amps_Test} can be used to determine when the sending operation has
completed by giving it the {\bf handle} object that is returned from
\Ref{amps_ISend}.  Similarly, \Ref{amps_Wait} can be used to block
until an initiated send has completed.  Every \Ref{amps_ISend} must be
have an associated \Ref{amps_Wait} to finalize the send.  None of these
tests imply that the receiving side has received the message.  There are
no functions in {\em AMPS} to do synchronous communication (where a send
does not return until the receive has been completed).  The difference
between the two sending functions is whether they block waiting for the
local sending action to be completed.  On some systems \Ref{amps_Send}
will be synchronous with the receive but this is not guaranteed.  If there
is computation that can take place it is best to use \Ref{amps_ISend}
rather than \Ref{amps_Send} to attempt to overlap computation and
communication.

{\large Example:}
\begin{verbatim}amps_Invoice invoice;
amps_Handle  handle;
int me, i;
double d;

me = amps_Rank(amps_CommWorld);

invoice = amps_NewInvoice("%i%d", &i, &d);

handle = amps_ISend(amps_CommWorld, me+1, invoice);

// do some work

amps_Wait(handle);

amps_FreeInvoice(invoice);
\end{verbatim}

{\large Notes:}

On most ports \Ref{amps_ISend} and \Ref{amps_Send} are identical.

@memo Non-blocking send
@param comm Communication context [IN]
@param dest Destination rank [IN]
@param invoice Data to send [IN]
@return Error code
*/
#define amps_ISend(comm, dest, invoice) 0, amps_Send((comm), (dest), (invoice))

#define amps_new(comm, size) malloc(size)
#define amps_free(comm, buf) free((char*)buf)


#define amps_FreeHandle(handle) free((handle));

/**

\Ref{amps_Fclose} is used to close a distributed file.  Every file
that is opened should be closed.

{\large Example:}
\begin{verbatim}
amps_File file;

char *filename;
double d;

file = amps_Fopen(filename,"w");

amps_Fprintf(file, "%lf", d);

amps_Fclose(file);
\end{verbatim}

{\large Notes:}

@memo Close a distributed file
@param file file handle to close [IN]
@return Error code
*/
#define amps_Fclose(file)  fclose((file))

/**

The routine \Ref{amps_Fprintf} is used to output to a distributed file.
The arguments are similar to the standard C library {\bf fprintf} with
the \Ref{FILE} argument replaced by a \Ref{amps_File} argument.

{\large Example:}
\begin{verbatim}
amps_File file;

char *filename;
double d;

file = amps_Fopen(filename,"w");

amps_Fprintf(file, "%lf\n", &d);

amps_Fclose(file);
\end{verbatim}

{\large Notes:}

@memo Print to a distributed file
@param file Shared file handle [IN]
@param fmt Format string [IN]
@param ... Paramaters for the format string [IN]
@return Error code
*/
#define amps_Fprintf fprintf

/**

The function \Ref{amps_Fscanf} is used to read from a distributed file.
Each node's accesses to a distributed file are independent of each
other.  The arguments are similar to the standard C library function
{\bf fscanf} except that it works on \Ref{amps_File} instead of the
standard {\bf FILE} type.

{\large Example:}
\begin{verbatim}
amps_File file;

char *filename;
double d;

file = amps_Fopen(filename,"r");

amps_Fscanf(file, "%lf", &d);

amps_Fclose(file);
\end{verbatim}

{\large Notes:}

@memo Read from a distributed file
@param file Shared file handle [IN]
@param fmt Format string [IN]
@param ... Paramaters for the format string [IN]
@return Error code
*/
#define amps_Fscanf fscanf

/**

\Ref{amps_FFclose} is used to close a fixed file.  Every file
that is opened should be closed.

{\large Example:}
\begin{verbatim}
amps_File file;

char *filename;
double d;
long NotUsed;

file = amps_FFopen(amps_CommWorld, filename, "rb", NotUsed);

amps_ReadDouble(file, &d, 1);

amps_FFclose(file);
\end{verbatim}

{\large Notes:}

@memo Close a fixed file
@param file Fixed file handle to close
@return Error code
*/
#define amps_FFclose(file)  fclose((file))

/**

The collective operation \Ref{amps_Sync} is used to provide a barrier
mechanism.  All nodes making this call will block until every node has
called \Ref{amps_Sync}.

{\large Example:}
\begin{verbatim}
amps_Invoice invoice;
double       d;
int          i;

invoice = amps_NewInvoice("%i%d", &i, &d);

// find maximum of i and d on all nodes
amps_AllReduce(amps_CommWorld, invoice, amps_Max);

// find sum of i and d on all nodes
amps_AllReduce(amps_CommWorld, invoice, amps_Add);

amps_FreeInvoice(invoice);

amps_Sync(amps_CommAll);
\end{verbatim}

{\large Notes:}

Since \Ref{amps_Sync} is implemented on the barrier mechanism of the
machine the code is running on, one should be careful of the name "Sync".
This does not imply that all the node process are started simultaneously
after the call.  All \Ref{amps_Sync} guarantees is that no process is allowed
past the call until all nodes in the communicator have made the call.

@memo Synchronize nodes
@param comm Communication context
@return Error code
*/
#define amps_Sync(comm) MPI_Barrier((comm))

#define amps_Exit(code) exit(code)


/* ****************************************************************************
 * Read and Write routines to write to files in XDR format.
 *****************************************************************************/

#define amps_SizeofChar sizeof(char)
#define amps_SizeofShort sizeof(short)
#define amps_SizeofInt sizeof(int)
#define amps_SizeofLong sizeof(long)
#define amps_SizeofFloat sizeof(float)
#define amps_SizeofDouble sizeof(double)

/*===========================================================================*/
/**
@name amps\_WriteType

There are several routines to output to a distributed file using a
binary binary rather than ASCII.  {\bf type} can be replaced by Char,
Short, Int, Long, Float, or Double to indicate the type of data to
output.  The type is specified to allow data conversions.  Data is
written using XDR format (\cite{xdr.87}).  These functions are
similar in nature to the standard C library routine {\bf write}.

{\large Example:}
\begin{verbatim}
amps_File file;

char *filename;
double d[10];

file = amps_Fopen(filename,"wb");

amps_WriteDouble(file, d, 10);

amps_Fclose(file);
\end{verbatim}

{\large Notes:}

@memo Write to file in binary
@param file File handle [IN]
@param ptr Pointer to data of type {\bf Type} [IN]
@param len Number of items to write out [IN]
@return Error code
*/

/*===========================================================================*/
/**
@name amps\_ReadType

This set of functions is used to read binary data from a distributed file.
 {\bf type} can be replaced by Char, Short, Int, Long, Float, or Double
to indicate the type of data to output.  The type is specified in order
to do conversions.  Data is converted from {\em XDR} format (\cite{xdr.87}).
The arguments are similar to the standard C library function
{\bf read}.

{\large Example:}
\begin{verbatim}
amps_File file;

char *filename;
double d[10];

file = amps_Fopen(filename,"rb");

amps_ReadDouble(file, d, 10);

amps_Fclose(file);
\end{verbatim}

{\large Notes:}

@memo Read from a file in binary
@param file File handle [IN]
@param ptr Pointer to data of type {\bf Type} [OUT]
@param len Number of items to read [IN]
@return Error code
*/

/*---------------------------------------------------------------------------*/
/* The following routines are used to actually write data to a file.         */
/* We use XDR like representation for all values written.                    */
/*---------------------------------------------------------------------------*/
#ifndef CASC_HAVE_BIGENDIAN

/*---------------------------------------------------------------------------*/
/* On the nodes store numbers with wrong endian so we need to swap           */
/*---------------------------------------------------------------------------*/

#define amps_WriteChar(file, ptr, len) \
    fwrite( (ptr), sizeof(char), (len), (FILE *)(file) )

#define amps_WriteShort(file, ptr, len) \
    fwrite( (ptr), sizeof(short), (len), (FILE *)(file) )

void amps_WriteInt(amps_File file, int *ptr, int len);

#define amps_WriteLong(file, ptr, len) \
    fwrite( (ptr), sizeof(long), (len), (FILE *)(file) )

void amps_WriteDouble(amps_File file, double *ptr, int len);

#define amps_ReadChar(file, ptr, len) \
    fread( (ptr), sizeof(char), (len), (FILE *)(file) )

#define amps_ReadShort(file, ptr, len) \
    fread( (ptr), sizeof(short), (len), (FILE *)(file) )

void amps_ReadInt(amps_File file, int *ptr, int len);

#define amps_ReadLong(file, ptr, len) \
    fread( (ptr), sizeof(long), (len), (FILE *)(file) )

void amps_ReadDouble(amps_File file, double *ptr, int len);

#else

#ifdef AMPS_INTS_ARE_64

#define amps_WriteChar(file, ptr, len) \
    fwrite( (ptr), sizeof(char), (len), (FILE *)(file) )

#define amps_WriteShort(file, ptr, len) \
    fwrite( (ptr), sizeof(short), (len), (FILE *)(file) )

#define amps_WriteLong(file, ptr, len) \
    fwrite( (ptr), sizeof(long), (len), (FILE *)(file) )

#define amps_WriteFloat(file, ptr, len) \
    fwrite( (ptr), sizeof(float), (len), (FILE *)(file) )

#define amps_WriteDouble(file, ptr, len) \
    fwrite( (ptr), sizeof(double), (len), (FILE *)(file) )


#define amps_ReadChar(file, ptr, len) \
    fread( (ptr), sizeof(char), (len), (FILE *)(file) )

#define amps_ReadShort(file, ptr, len) \
    fread( (ptr), sizeof(short), (len), (FILE *)(file) )

#define amps_ReadLong(file, ptr, len) \
    fread( (ptr), sizeof(long), (len), (FILE *)(file) )

#define amps_ReadFloat(file, ptr, len) \
    fread( (ptr), sizeof(float), (len), (FILE *)(file) )

#define amps_ReadDouble(file, ptr, len) \
    fread( (ptr), sizeof(double), (len), (FILE *)(file) )

#else

#define amps_WriteChar(file, ptr, len) \
    fwrite( (ptr), sizeof(char), (len), (FILE *)(file) )

#define amps_WriteShort(file, ptr, len) \
    fwrite( (ptr), sizeof(short), (len), (FILE *)(file) )

#define amps_WriteInt(file, ptr, len) \
    fwrite( (ptr), sizeof(int), (len), (FILE *)(file) )

#define amps_WriteLong(file, ptr, len) \
    fwrite( (ptr), sizeof(long), (len), (FILE *)(file) )

#define amps_WriteFloat(file, ptr, len) \
    fwrite( (ptr), sizeof(float), (len), (FILE *)(file) )

#define amps_WriteDouble(file, ptr, len) \
    fwrite( (ptr), sizeof(double), (len), (FILE *)(file) )


#define amps_ReadChar(file, ptr, len) \
    fread( (ptr), sizeof(char), (len), (FILE *)(file) )

#define amps_ReadShort(file, ptr, len) \
    fread( (ptr), sizeof(short), (len), (FILE *)(file) )

#define amps_ReadInt(file, ptr, len) \
    fread( (ptr), sizeof(int), (len), (FILE *)(file) )

#define amps_ReadLong(file, ptr, len) \
    fread( (ptr), sizeof(long), (len), (FILE *)(file) )

#define amps_ReadFloat(file, ptr, len) \
    fread( (ptr), sizeof(float), (len), (FILE *)(file) )

#define amps_ReadDouble(file, ptr, len) \
    fread( (ptr), sizeof(double), (len), (FILE *)(file) )

#endif
#endif

/*===========================================================================*/
/**

\Ref{amps_TAlloc} is used to allocate memory for objects used
in the \Ref{amps_Packages}.  Since these objects must be in
shared memory they need to be allocated using this macro.
A space is allocated for {\bf size} number of {\bf type} objects.

{\large Example:}
\begin{verbatim}
\end{verbatim}

{\large Notes:}

@memo Allocate space for packages
@param type The C type name
@param count Number of items of type to allocate
@return Pointer to the allocated dataspace
*/
#define amps_TAlloc(type, count) ((count) ? (type *) malloc((unsigned int)(sizeof(type) * (count))) : NULL)

/*===========================================================================*/
/**

\Ref{amps_CTAlloc} is used to allocate memory for objects used
in the \Ref{amps_Packages}.  Since these objects must be in
shared memory they need to be allocated using this macro.
A space is allocated for {\bf size} number of {\bf type} objects.
This space is cleared (set to 0) before being returned to the user.

{\large Example:}
\begin{verbatim}
\end{verbatim}

{\large Notes:}

@memo Allocate space for packages and clear it
@param type The C type name
@param count Number of items of type to allocate
@return Pointer to the allocated dataspace
*/

#define amps_CTAlloc(type, count) \
((count) ? (type *) calloc((unsigned int)(count), (unsigned int)sizeof(type)) : NULL)

/**

\Ref{amps_TFree} is used to deallocate memory for objects that were
allocated by \Ref{amps_TAlloc} or \Ref{amps_CTAlloc}.

{\large Example:}
\begin{verbatim}
\end{verbatim}

{\large Notes:}

@memo Free memory allocated with \Ref{amps_TAlloc} or \Ref{amps_CTAlloc}
@param ptr Pointer to dataspace to free
@return Error code
*/

#define amps_TFree(ptr) if (ptr) free(ptr); else
/* note: the `else' is required to guarantee termination of the `if' */
 
#define amps_Error(name, type, comment, operation) 

#include "amps_proto.h"

#define AMPS_EXCHANGE_SPECIALIZED 1
#define AMPS_NEWPACKAGE_SPECIALIZED 1



