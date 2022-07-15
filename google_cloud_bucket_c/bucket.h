#ifndef GOOGLE_CLOUD_BUCKET_H
#define GOOGLE_CLOUD_BUCKET_H

#ifdef __cplusplus
#include <cstdbool>
extern "C" {
#else
#include <gcloud_bucket_c_stdbool.h>
#endif

#include <gcloud_bucket_c_types_common.h>
#include <google_cloud_bucket_c_export.h>

struct BucketC {
  const char *kind;
  const char *id;

  const char *selfLink;
  unsigned long projectNumber;

  const char *name;

  time_t timeCreated;

  time_t updated;

  bool defaultEventBasedHold;

  /*"retentionPolicy": {
    "retentionPeriod": "unsigned long

                       ",
                       "effectiveTime": "datetime

                                        ",
                                        "isLocked": boolean


  },*/
  long metageneration;
  /*"acl": [
  bucketAccessControls Resource
  ],
  "defaultObjectAcl": [
  defaultObjectAccessControls Resource
  ],
  "iamConfiguration": {
    "publicAccessPrevention": string

        ,
        "uniformBucketLevelAccess": {
      "enabled": boolean

          ,
          "lockedTime":"datetime

                       ",
    },
  },
  "encryption": {
    "defaultKmsKeyName": string


  },
  "owner": {
    "entity": string

        ,
        "entityId": string


  },*/
  const char *location;

  const char *locationType;

  const char *rpo;

  /*
  "website": {
    "mainPageSuffix": string

        ,
        "notFoundPage": string


  },
  "logging": {
    "logBucket": string

        ,
        "logObjectPrefix": string


  },
  "versioning": {
    "enabled": boolean


  },
  "cors": [
  {
    "origin": [
    string


    ],
    "method": [
    string


    ],
    "responseHeader": [
    string


    ],
    "maxAgeSeconds": integer


  }
  ],
  "lifecycle": {
    "rule": [
    {
      "action": {
        "type": string

            ,
            "storageClass": string


      },
      "condition": {
        "age": integer

            ,
            "createdBefore": "date

                             ",
                             "customTimeBefore": "date

                                                 ",
                                                 "daysSinceCustomTime": integer

            ,
            "daysSinceNoncurrentTime": integer

            ,
            "isLive": boolean

            ,
            "matchesStorageClass": [
        string


        ],
        "noncurrentTimeBefore": "date

                                ",
                                "numNewerVersions": integer


      }
    }
    ]
  },
  "labels": {
    (key)

    : string


  },*/
  const char *storageClass;

  /*"billing": {
    "requesterPays": boolean


  },*/
  const char *etag;
};

struct StatusAndBucketCArray {
  int status;
  struct BucketC *buckets;
  size_t size;
};

extern GOOGLE_CLOUD_BUCKET_C_EXPORT struct StatusAndArrayCStrArray
list_bucket_names(const char *google_access_token,
                  const char *google_project_id);

extern GOOGLE_CLOUD_BUCKET_C_EXPORT struct StatusAndBucketCArray
list_buckets(const char *google_access_token, const char *google_project_id);

extern GOOGLE_CLOUD_BUCKET_C_EXPORT int
create_bucket(const char *google_access_token, const char *google_project_id,
              const char *google_bucket_name, const char *google_region);
extern GOOGLE_CLOUD_BUCKET_C_EXPORT int
add_file_to_bucket(const char *google_access_token,
                   const char *google_bucket_name, const char *object_name,
                   const char *file_name);
extern GOOGLE_CLOUD_BUCKET_C_EXPORT struct StatusAndArrayCStrArray
add_directory_to_bucket(const char *google_access_token,
                        const char *google_bucket_name,
                        const char *folder_path);

extern GOOGLE_CLOUD_BUCKET_C_EXPORT int
download_file(const struct configuration *, const char *, const char *);
extern GOOGLE_CLOUD_BUCKET_C_EXPORT struct StatusAndArrayCStrArray
storage(const struct configuration *, const char *, bool);

/* https://googleapis.dev/cpp/google-cloud-storage/2.0.0/classgoogle_1_1cloud_1_1storage_1_1Client.html#a8da09636f0cadae59892a4a3da3c3458
 */
extern GOOGLE_CLOUD_BUCKET_C_EXPORT int
upload_file_to_bucket(const char *google_access_token, const char *filename,
                      const char *bucket_name, const char *object_name);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /* !GOOGLE_CLOUD_BUCKET_H */
