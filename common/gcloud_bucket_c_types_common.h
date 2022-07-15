/* TODO: Figure out where this goes, e.g., its own repo + separate vcpkg port */
#if !defined(GCLOUD_BUCKET_C_TYPES_COMMON_H) &&                                \
    !defined(GOOGLE_CLOUD_C_TYPES_COMMON_H)
#define GCLOUD_BUCKET_C_TYPES_COMMON_H

#ifdef __cplusplus
#include <cstdlib>
extern "C" {
#else
#include <gcloud_bucket_c_stdbool.h>
#include <stdlib.h>
#endif

#if defined(_MSC_VER)
#include <BaseTsd.h>
typedef SSIZE_T ssize_t;
#else
#include <sys/types.h>
#endif

#include <google_cloud_bucket_c_common_export.h>

struct ErrorOrType {
  int error;
  void *type;
};

struct ErrorOrIntArray {
  int error;
  int *array;
  size_t size;
};

struct StatusAndCstr {
  int status;
  const char *c_str;
  ssize_t size;
};

struct StatusAndCstrAndCStr {
  int status;
  const char *c_str0;
  ssize_t size0;
  const char *c_str1;
  ssize_t size1;
};

struct StatusAndArrayCStrArray {
  int status;
  char *const *c_str_arr;
  size_t size;
};

struct CStrArray {
  char *const *arr;
  size_t size;
};

struct InstanceIncomplete {
  const char *id, *creationTimestamp, *name, *description, *machineType,
      *statusMessage, *zone, *cpuPlatform, *hostname, *lastStartTimestamp,
      *lastStopTimestamp, *lastSuspendedTimestamp, *kind;
  // std::map<std::string, std::string> metadata;
  bool supportsPzs;
};

extern GOOGLE_CLOUD_BUCKET_C_COMMON_EXPORT const struct StatusAndCstr
    StatusAndCstrNull;

extern GOOGLE_CLOUD_BUCKET_C_COMMON_EXPORT const struct StatusAndArrayCStrArray
    StatusAndArrayCStrArrayNull;

extern GOOGLE_CLOUD_BUCKET_C_COMMON_EXPORT const struct StatusAndCstrAndCStr
    StatusAndCstrAndCStrNull;

extern GOOGLE_CLOUD_BUCKET_C_COMMON_EXPORT void
cleanup_struct_cstr_array(struct StatusAndArrayCStrArray *);

extern GOOGLE_CLOUD_BUCKET_C_COMMON_EXPORT void
cleanup_struct_cstr(struct StatusAndCstr *);

extern GOOGLE_CLOUD_BUCKET_C_COMMON_EXPORT void
cleanup_struct_cstr_cstr(struct StatusAndCstrAndCStr *);

extern GOOGLE_CLOUD_BUCKET_C_COMMON_EXPORT void
cleanup_struct_array(struct ErrorOrIntArray *);

struct configuration {
  const char *google_access_token;
  time_t google_access_token_expiry;
  const char *google_refresh_token;
  const char *google_project_id;
  const char *google_bucket_name;
  const char *google_region;
  const char *google_zone;
  const char *google_instance_name;
  const char *google_instance_public_ip;
  const char *folder_path;
};

extern GOOGLE_CLOUD_BUCKET_C_COMMON_EXPORT void
cleanup_struct_configuration(struct configuration *);

#ifdef __cplusplus
}
#endif

#endif /* !defined(GCLOUD_BUCKET_C_TYPES_COMMON_H) &&                          \
          !defined(GOOGLE_CLOUD_C_TYPES_COMMON_H) */
