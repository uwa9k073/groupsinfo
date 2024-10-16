FROM ghcr.io/userver-framework/ubuntu-22.04-userver-base-ci:latest AS builder

WORKDIR /app

COPY . .

RUN GIT_SSH_COMMAND="ssh -o StrictHostKeyChecking=no" git submodule update --init

RUN CC=clang-16 CXX=clang++-16 make build-release

FROM ghcr.io/userver-framework/ubuntu-22.04-userver-base-ci:latest AS runner

RUN mkdir -p /configs

COPY --from=builder /app/configs/* /configs/
COPY --from=builder /app/build_release/groupsinfo /bin/

CMD ["groupsinfo", "-c", "/configs/static_config.dev.yaml", "--config_vars", "/configs/config_vars.dev.yaml"]

