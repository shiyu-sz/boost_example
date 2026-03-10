FROM ubuntu:24.04

# 避免交互式提问
ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y git