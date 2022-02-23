#ifndef PP_BUCKET_H
#define PP_BUCKET_H

#ifdef	__cplusplus
#include <cstdbool>
extern "C" {
#else
#include <acquire_stdbool>
#endif

#include <google_cloud_bucket_c_export.h>
#include <types_common.h>

extern GOOGLE_CLOUD_BUCKET_C_EXPORT int download_file(const struct configuration *, const char *, const char *);
extern GOOGLE_CLOUD_BUCKET_C_EXPORT struct StatusAndArrayCStrArray storage(const struct configuration *, const char *, bool);

#ifdef	__cplusplus
} /*extern "C"*/
#endif

#endif /* PP_BUCKET_H */
