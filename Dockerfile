FROM ubuntu:22.04

RUN apt-get update && apt-get install -y \
    build-essential g++ python3 cmake git libsqlite3-dev libxml2-dev \
    libgtk-3-dev wget && rm -rf /var/lib/apt/lists/*

WORKDIR /opt
RUN git clone https://gitlab.com/nsnam/ns-3-dev.git ns-3
WORKDIR /opt/ns-3
RUN ./ns3 configure --enable-examples --enable-tests && ./ns3 build

ENV NS3_HOME=/opt/ns-3
ENV PATH="$NS3_HOME:$PATH"
WORKDIR /workspace
