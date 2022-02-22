FROM alpine

RUN apk add --no-cache gcc cmake make musl-dev linux-headers pkgconfig libcurl

COPY . /google-cloud-bucket-c

WORKDIR /google-cloud-bucket-c/build

RUN cmake -DCMAKE_BUILD_TYPE="Debug" .. && \
    cmake --build .
