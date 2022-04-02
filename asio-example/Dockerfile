FROM ubuntu:latest

# Install necessary library
WORKDIR /root
RUN apt update && apt upgrade -y
RUN apt update && apt install --assume-yes software-properties-common
RUN add-apt-repository ppa:deadsnakes/ppa
RUN apt install --assume-yes git tmux tree wget unzip htop cmake unzip nano g++ python-dev vim

# install boost
RUN wget https://boostorg.jfrog.io/artifactory/main/release/1.77.0/source/boost_1_77_0.tar.gz -O /root/boost_1_77_0.tar.gz
WORKDIR /root
RUN apt install --assume-yes build-essential autotools-dev libicu-dev build-essential libbz2-dev libboost-all-dev
RUN tar -xvzf boost_1_77_0.tar.gz
WORKDIR /root/boost_1_77_0
RUN rm -rf /usr/lib/cmake/[Bb]oost*
RUN ./bootstrap.sh --prefix=/usr --with-python=python3
RUN ./b2 stage -j$(nproc) threading=multi link=shared
RUN ./b2 install threading=multi link=shared   
WORKDIR /root
RUN rm -rf /root/boost_1_77_0
RUN rm -rf /root/boost_1_77_0.tar.gz

WORKDIR /root