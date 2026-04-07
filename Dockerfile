# builds cmd and test
FROM ubuntu:24.04 AS builder

WORKDIR /app

RUN apt-get update && apt-get install -y build-essential cmake

COPY . . 
RUN cmake -B build && cmake --build build --target cpu3026_cmd cpu3026_test

# run docker with '--target test' to run test
FROM ubuntu:24.04 AS test

RUN useradd user
USER user

WORKDIR /app

COPY --from=builder /app/build/test/cpu3026_test /app/cpu3026_test
RUN /app/cpu3026_test

# main dockerfile output
FROM ubuntu:24.04 AS run

RUN useradd user
USER user

WORKDIR /app

COPY --from=builder /app/build/app/cmd/cpu3026_cmd /app/cpu3026_cmd

ENTRYPOINT ["/app/cpu3026_cmd"]