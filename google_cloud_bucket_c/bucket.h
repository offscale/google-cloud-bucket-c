#ifndef PP_BUCKET_H
#define PP_BUCKET_H

#ifdef	__cplusplus
#include <cstdbool>
extern "C" {
#else
#include <gcloud_bucket_c_stdbool.h>
#endif

#include <google_cloud_bucket_c_export.h>
#include <gcloud_bucket_c_types_common.h>

extern GOOGLE_CLOUD_BUCKET_C_EXPORT StatusAndArrayCStrArray list_buckets(const char *google_access_token, const char *google_project_id);
extern GOOGLE_CLOUD_BUCKET_C_EXPORT int create_bucket(const char *google_access_token, const char *google_project_id, const char *google_bucket_name, const char *google_region);
extern GOOGLE_CLOUD_BUCKET_C_EXPORT int add_file_to_bucket(const char *google_access_token, const char *google_bucket_name, const char *object_name, const char *file_name);

extern GOOGLE_CLOUD_BUCKET_C_EXPORT int download_file(const struct configuration *, const char *, const char *);
extern GOOGLE_CLOUD_BUCKET_C_EXPORT struct StatusAndArrayCStrArray storage(const struct configuration *, const char *, bool);

#ifdef	__cplusplus
} /*extern "C"*/
#endif

#endif /* PP_BUCKET_H */
