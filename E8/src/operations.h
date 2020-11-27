/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#ifndef _OPERATIONS_H_RPCGEN
#define _OPERATIONS_H_RPCGEN

#include <rpc/rpc.h>


#ifdef __cplusplus
extern "C" {
#endif


struct file_in {
	char *fileName;
};
typedef struct file_in file_in;

struct dir_in {
	char *dirName;
	int num;
};
typedef struct dir_in dir_in;

struct file_out {
	int chars;
	int strings;
	int rows;
};
typedef struct file_out file_out;

#define OPERATIONSPROG 0x20000013
#define OPERATIONSVERS 1

#if defined(__STDC__) || defined(__cplusplus)
#define FILE_SCAN 1
extern  file_out * file_scan_1(file_in *, CLIENT *);
extern  file_out * file_scan_1_svc(file_in *, struct svc_req *);
#define DIR_SCAN 2
extern  int * dir_scan_1(dir_in *, CLIENT *);
extern  int * dir_scan_1_svc(dir_in *, struct svc_req *);
extern int operationsprog_1_freeresult (SVCXPRT *, xdrproc_t, caddr_t);

#else /* K&R C */
#define FILE_SCAN 1
extern  file_out * file_scan_1();
extern  file_out * file_scan_1_svc();
#define DIR_SCAN 2
extern  int * dir_scan_1();
extern  int * dir_scan_1_svc();
extern int operationsprog_1_freeresult ();
#endif /* K&R C */

/* the xdr functions */

#if defined(__STDC__) || defined(__cplusplus)
extern  bool_t xdr_file_in (XDR *, file_in*);
extern  bool_t xdr_dir_in (XDR *, dir_in*);
extern  bool_t xdr_file_out (XDR *, file_out*);

#else /* K&R C */
extern bool_t xdr_file_in ();
extern bool_t xdr_dir_in ();
extern bool_t xdr_file_out ();

#endif /* K&R C */

#ifdef __cplusplus
}
#endif

#endif /* !_OPERATIONS_H_RPCGEN */
