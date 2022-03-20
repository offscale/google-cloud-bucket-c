#include <iostream>
#include <utility>

#include <google/cloud/storage/client.h>
#include <google/cloud/storage/oauth2/service_account_credentials.h>
#include <google/cloud/storage/version.h>

#include "bucket.h"

#if defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__)
#include <string.h>
#define strdup _strdup
#define PATH_MAX MAX_PATH
#define NUM_FORMAT "zu"
#else
#define NUM_FORMAT "lu"
#include <dirent.h>
#endif

class OutOfBandCredentials
    : public google::cloud::storage::oauth2::Credentials {
public:
  explicit OutOfBandCredentials(
      const std::string &google_access_token,
      google::cloud::storage::oauth2::ServiceAccountCredentialsInfo info)
      : authorisation_header("Authorization: Bearer " + google_access_token),
        info_(std::move(info)) {}

  google::cloud::StatusOr<std::string> AuthorizationHeader() override {
    return authorisation_header;
  }

#if __cplusplus >= 201703L
  [[nodiscard]]
#endif
  google::cloud::StatusOr<std::vector<std::uint8_t>>
  SignBlob(google::cloud::storage::SigningAccount const &signing_account,
           std::string const &blob) const override {
    if ((signing_account.has_value() &&
         (signing_account.value() != info_.client_email)) ||
        true /* TODO */
    ) {
      return google::cloud::Status(
          google::cloud::StatusCode::kInvalidArgument,
          "The current_credentials cannot sign blobs for " +
              signing_account.value());
    }
    /* return google::cloud::storage::oauth2::internal::SignStringWithPem(
                     blob, info_.private_key,
                     google::cloud::storage::oauth2::JwtSigningAlgorithms::RS256);
     */
    return google::cloud::Status(google::cloud::StatusCode::kUnknown,
                                 "default");
  }

private:
  const std::string authorisation_header;
  const google::cloud::storage::oauth2::ServiceAccountCredentialsInfo info_;
};

extern int download_file(const struct configuration *config,
                         const char *object_name, const char *file_name) {
  static google::cloud::storage::Client client = google::cloud::storage::Client(
      std::make_unique<OutOfBandCredentials>(OutOfBandCredentials(
          config->google_access_token,
          google::cloud::storage::oauth2::ServiceAccountCredentialsInfo{
              /* TODO */})));
  const google::cloud::Status status =
      client.DownloadToFile(config->google_bucket_name, object_name, file_name);
  if (status.ok())
    return 200;
  else {
    std::cerr << status.message().c_str() << std::endl;
    return 404;
  }
}

int upload_file_to_bucket(const struct configuration *const config,
                          const char *object_name, const char *file_name,
                          std::vector<const char *> &uploaded,
                          google::cloud::storage::Client &client) {
  const google::cloud::StatusOr<google::cloud::storage::ObjectMetadata> status =
      client.UploadFile(file_name, config->google_bucket_name, object_name);
  if (status.ok()) {
    std::cout << "Successfully created object:\t" << status.value().id()
              << '\n';
    uploaded.push_back(strdup(file_name));
    return EXIT_SUCCESS;
  } else {
    std::cerr << "Error creating object:\t" << status.status().message()
              << std::endl;
    return EXIT_FAILURE;
  }
}

struct StatusAndArrayCStrArray list_bucket_names(const char *google_access_token,
                                     const char *google_project_id) {
  static google::cloud::storage::Client client = google::cloud::storage::Client(
      std::make_unique<OutOfBandCredentials>(OutOfBandCredentials(
          google_access_token,
          google::cloud::storage::oauth2::ServiceAccountCredentialsInfo{
              /* TODO */})));

  google::cloud::storage::ListBucketsReader bucketsReader =
      client.ListBucketsForProject(google_project_id);
  std::vector<const char *> bucketNames;
  for (const auto &bucket : bucketsReader) {
    /* bucket->label */
    if (!bucket) {
      fputs(bucket.status().message().c_str(), stderr);
      return {EXIT_FAILURE, NULL, 0};
    }
    printf("Bucket name: \"%s\"\t# of labels: %" NUM_FORMAT "\n",
           bucket->name().c_str(), bucket->labels().size());
    bucketNames.push_back(strdup(bucket->name().c_str()));
  }
  char **bucket_names_c_str =
      reinterpret_cast<char **>(malloc(sizeof(char *) * bucketNames.size()));
  for (size_t i = 0; i < bucketNames.size(); ++i)
    bucket_names_c_str[i] = strdup(bucketNames[i]);

  const struct StatusAndArrayCStrArray bucketNamesReturn = {
      EXIT_SUCCESS, bucket_names_c_str, bucketNames.size()};
  return bucketNamesReturn;
}

struct StatusAndBucketCArray list_buckets(const char *google_access_token,
                                          const char *google_project_id) {
  static google::cloud::storage::Client client = google::cloud::storage::Client(
      std::make_unique<OutOfBandCredentials>(OutOfBandCredentials(
          google_access_token,
          google::cloud::storage::oauth2::ServiceAccountCredentialsInfo{
              /* TODO */})));

  google::cloud::storage::ListBucketsReader bucketsReader =
      client.ListBucketsForProject(google_project_id);
  std::vector<struct BucketC> buckets;
  for (const auto &bucket : bucketsReader) {
    /* bucket->label */
    if (!bucket) {
      fputs(bucket.status().message().c_str(), stderr);
      return {EXIT_FAILURE, NULL, 0};
    }
    printf("Bucket name: \"%s\"\t# of labels: %" NUM_FORMAT "\n",
           bucket->name().c_str(), bucket->labels().size());
    struct BucketC bucket_c{};
    bucket_c.kind = bucket->kind().c_str();
    bucket_c.id = bucket->id().c_str();
    bucket_c.selfLink = bucket->self_link().c_str();
    bucket_c.projectNumber = bucket->project_number();
    bucket_c.name = bucket->name().c_str();
    bucket_c.updated = std::chrono::system_clock::to_time_t(bucket->updated());
    bucket_c.defaultEventBasedHold = bucket->default_event_based_hold();
    bucket_c.metageneration = bucket->metageneration();
    bucket_c.location = bucket->location().c_str();
    bucket_c.locationType = bucket->location_type().c_str();
    bucket_c.rpo = bucket->rpo().c_str();
    bucket_c.storageClass = bucket->storage_class().c_str();
    bucket_c.etag = bucket->etag().c_str();
    buckets.push_back(bucket_c);
  }
  struct BucketC *bucket_c_arr =
      reinterpret_cast<struct BucketC *>(malloc(sizeof(struct BucketC) * buckets.size()));
  for (size_t i = 0; i < buckets.size(); ++i)
    bucket_c_arr[i] = buckets[i];

  const struct StatusAndBucketCArray status_and_buckets = {
      EXIT_SUCCESS, bucket_c_arr, buckets.size()};
  return status_and_buckets;
}

int create_bucket(const char *google_access_token,
                  const char *google_project_id, const char *google_bucket_name,
                  const char *google_region) {
  static google::cloud::storage::Client client = google::cloud::storage::Client(
      std::make_unique<OutOfBandCredentials>(OutOfBandCredentials(
          google_access_token,
          google::cloud::storage::oauth2::ServiceAccountCredentialsInfo{
              /* TODO */})));

  const google::cloud::StatusOr<google::cloud::storage::BucketMetadata> bucket =
      client.CreateBucketForProject(
          google_bucket_name, google_project_id,
          google::cloud::storage::BucketMetadata()
              .set_location(google_region)
              .set_storage_class(
                  google::cloud::storage::storage_class::Regional()));
  if (bucket.ok()) {
    printf("Successfully created bucket:\t%s\n", bucket.value().id().c_str());
    return EXIT_SUCCESS;
  } else {
    fprintf(stderr, "Error creating bucket:\t%s\n",
            bucket.status().message().c_str());
    return EXIT_FAILURE;
  }
}

int add_file_to_bucket(const char *google_access_token,
                       const char *google_bucket_name, const char *object_name,
                       const char *file_name) {
  static google::cloud::storage::Client client = google::cloud::storage::Client(
      std::make_unique<OutOfBandCredentials>(OutOfBandCredentials(
          google_access_token,
          google::cloud::storage::oauth2::ServiceAccountCredentialsInfo{
              /* TODO */})));
  const google::cloud::StatusOr<google::cloud::storage::ObjectMetadata> status =
      client.UploadFile(file_name, google_bucket_name, object_name);
  if (status.ok()) {
    printf("Successfully created object:\t%s\n", status.value().id().c_str());
    return EXIT_SUCCESS;
  } else {
    fprintf(stderr, "Error creating object:\t%s\n",
            status.status().message().c_str());
    return EXIT_FAILURE;
  }
}

struct StatusAndArrayCStrArray
add_directory_to_bucket(const char *google_access_token,
                        const char *google_bucket_name,
                        const char *folder_path) {
  /* Consider: Concurrency + some sort of directory lock? -
   * And/or compression? - (zlib stuff is already done via the google cpp
   * client) And even split into multi files, to combine with concurrency? Or go
   * harder and implement à la rsync?
   * */
  char full_path[PATH_MAX];
  std::vector<const char *> uploaded;
#if defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__)
  {
    HANDLE hFind;
    WIN32_FIND_DATA FindFileData;
    char wild_folder_path[PATH_MAX];
    ZeroMemory(&wild_folder_path, sizeof(full_path));
    snprintf(wild_folder_path, PATH_MAX, "%s%s", folder_path, "\\*\0");

    if ((hFind = FindFirstFile(wild_folder_path, &FindFileData)) !=
        INVALID_HANDLE_VALUE) {
      do {
        ZeroMemory(&full_path, sizeof(full_path));
        snprintf(full_path, PATH_MAX, "%s\\%s", folder_path,
                 FindFileData.cFileName);
        add_file_to_bucket(google_access_token, google_bucket_name,
                           FindFileData.cFileName, full_path);
      } while (FindNextFile(hFind, &FindFileData));
      FindClose(hFind);
    }
  }
#else
  {
    DIR *d = opendir(folder_path);
    struct dirent *dir;
    if (d) {
      while ((dir = readdir(d)) != NULL)
        if (dir->d_type == DT_REG) {
          memset(&full_path, 0, sizeof(full_path));
          snprintf(full_path, PATH_MAX, "%s/%s", folder_path, dir->d_name);
          /* TODO: Handle errors from `add_file_to_bucket` */
          add_file_to_bucket(google_access_token, google_bucket_name,
                             dir->d_name, full_path);
        }
      closedir(d);
    }
  }
#endif
  char **uploaded_c_str =
      reinterpret_cast<char **>(malloc(sizeof(char *) * uploaded.size()));
  for (size_t i = 0; i < uploaded.size(); ++i)
    uploaded_c_str[i] = strdup(uploaded[i]);

  const struct StatusAndArrayCStrArray statusAndArrayCStrArray = {
      EXIT_SUCCESS, uploaded_c_str, uploaded.size()};
  return statusAndArrayCStrArray;
}

struct StatusAndArrayCStrArray storage(const struct configuration *const config,
                                       const char *kind, bool list_only) {
  static google::cloud::storage::Client client = google::cloud::storage::Client(
      std::make_unique<OutOfBandCredentials>(OutOfBandCredentials(
          config->google_access_token,
          google::cloud::storage::oauth2::ServiceAccountCredentialsInfo{
              /* TODO */})));
  if (strcmp(kind, "buckets") == 0 && list_only) {
    google::cloud::storage::ListBucketsReader bucketsReader =
        client.ListBucketsForProject(config->google_project_id);
    std::vector<const char *> bucketNames;
    for (const auto &bucket : bucketsReader) {
      /* bucket->label */
      if (!bucket) {
        fputs(bucket.status().message().c_str(), stderr);
        return {EXIT_FAILURE, NULL, 0};
      }
      printf("Bucket name: \"%s\"\t# of labels: %" NUM_FORMAT "\n",
             bucket->name().c_str(), bucket->labels().size());
      bucketNames.push_back(strdup(bucket->name().c_str()));
    }
    char **bucket_names_c_str =
        reinterpret_cast<char **>(malloc(sizeof(char *) * bucketNames.size()));
    for (size_t i = 0; i < bucketNames.size(); ++i)
      bucket_names_c_str[i] = strdup(bucketNames[i]);

    const struct StatusAndArrayCStrArray bucketNamesReturn = {
        EXIT_SUCCESS, bucket_names_c_str, bucketNames.size()};
    return bucketNamesReturn;
  } else if (!list_only) {
    if (strcmp(kind, "bucket_create") == 0) {
      const google::cloud::StatusOr<google::cloud::storage::BucketMetadata>
          bucket = client.CreateBucketForProject(
              config->google_bucket_name, config->google_project_id,
              google::cloud::storage::BucketMetadata()
                  .set_location(config->google_region)
                  .set_storage_class(
                      google::cloud::storage::storage_class::Regional()));
      struct StatusAndArrayCStrArray statusAndArrayCStrArray = {EXIT_SUCCESS,
                                                                NULL, 0};
      if (bucket.ok())
        printf("Successfully created bucket:\t%s\n",
               bucket.value().id().c_str());
      else {
        fprintf(stderr, "Error creating bucket:\t%s\n",
                bucket.status().message().c_str());
        statusAndArrayCStrArray.status = EXIT_FAILURE;
      }
      return statusAndArrayCStrArray;
    } else if (strcmp(kind, "bucket_put_from_folder") == 0) {
      /* Concurrency + some sort of directory lock? -
       * And/or compression? - (zlib stuff is already done via the google cpp
       * client) And even split into multi files, to combine with concurrency?
       * Or go harder and implement à la rsync?
       * */
      char full_path[PATH_MAX];
      std::vector<const char *> uploaded;
#if defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__)
      {
        HANDLE hFind;
        WIN32_FIND_DATA FindFileData;
        char wild_folder_path[PATH_MAX];
        strcpy_s(wild_folder_path, PATH_MAX, config->folder_path);
        strcat_s(wild_folder_path, PATH_MAX, "\\*\0");

        if ((hFind = FindFirstFile(wild_folder_path, &FindFileData)) !=
            INVALID_HANDLE_VALUE) {
          do {
            ZeroMemory(&full_path, sizeof(full_path));
            strcpy_s(full_path, PATH_MAX, config->folder_path);
            strcat_s(full_path, PATH_MAX, "\\");
            strcat_s(full_path, PATH_MAX, FindFileData.cFileName);
            upload_file_to_bucket(/* config */ config,
                                  /* object_name*/ FindFileData.cFileName,
                                  /* file_name */ full_path,
                                  /* uploaded */ uploaded,
                                  /* client */ client);
          } while (FindNextFile(hFind, &FindFileData));
          FindClose(hFind);
        }
      }
#else
      {
        DIR *d = opendir(config->folder_path);
        struct dirent *dir;
        if (d) {
          while ((dir = readdir(d)) != NULL)
            if (dir->d_type == DT_REG) {
              memset(&full_path, 0, sizeof(full_path));
              strcpy(full_path, config->folder_path);
              strcat(full_path, "/");
              strcat(full_path, dir->d_name);
              upload_file_to_bucket(/* config */ config,
                                    /* object_name*/ dir->d_name,
                                    /* file_name */ full_path,
                                    /* uploaded */ uploaded,
                                    /* client */ client);
            }
          closedir(d);
        }
      }
#endif
      char **uploaded_c_str =
          reinterpret_cast<char **>(malloc(sizeof(char *) * uploaded.size()));
      for (size_t i = 0; i < uploaded.size(); ++i)
        uploaded_c_str[i] = strdup(uploaded[i]);

      const struct StatusAndArrayCStrArray statusAndArrayCStrArray = {
          EXIT_SUCCESS, uploaded_c_str, uploaded.size()};
      return statusAndArrayCStrArray;
    } else {
      fprintf(stderr, "%s has not been implemented\n", kind);
      exit(EXIT_FAILURE);
    }
  }
  return StatusAndArrayCStrArrayNull;
}
