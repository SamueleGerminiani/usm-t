#Download base image ubuntu 23.04
FROM ubuntu:22.04
# Update Ubuntu Software repository
RUN apt update
ARG DEBIAN_FRONTEND=noninteractive
RUN apt install -y git
RUN apt install -y uuid-dev pkg-config
RUN apt install -y build-essential
RUN apt install -y wget unzip cmake python3 python3-distutils python3-apt
ENV PYTHON=python3

RUN git clone https://github.com/ModelInference/texada.git
WORKDIR "/texada"
ENV TEXADA_HOME=/texada
ENV SPOT_LIB=$TEXADA_HOME/third_party/spot/lib/
ENV BOOST_LIB=$TEXADA_HOME/third_party/boost/lib/
ENV SPOT_INCL=$TEXADA_HOME/third_party/spot/include
ENV BOOST_INCL=$TEXADA_HOME/third_party/boost/include/
ENV GTEST_LIB=$TEXADA_HOME/third_party/googletest/install/lib/
ENV GTEST_INCL=$TEXADA_HOME/third_party/googletest/install/include/
ENV LD_LIBRARY_PATH=$LD_LIBRARY_PATH:third_party/spot/lib/

RUN git checkout spot-2.x
RUN mkdir third_party
WORKDIR "/texada/third_party"
RUN pwd
RUN wget http://www.lrde.epita.fr/dload/spot/spot-2.12.1.tar.gz
RUN tar -xvf spot-2.12.1.tar.gz && rm spot-2.12.1.tar.gz
WORKDIR "/texada/third_party/spot-2.12.1"
RUN ./configure --disable-python --prefix=$(pwd)/../spot
RUN make install -j16
RUN rm -rf spot-2.12.1 
WORKDIR "/texada/third_party"

RUN wget --no-check-certificate https://boostorg.jfrog.io/artifactory/main/release/1.83.0/source/boost_1_83_0.tar.gz
RUN tar -xvf boost_1_83_0.tar.gz && rm boost_1_83_0.tar.gz
WORKDIR "/texada/third_party/boost_1_83_0"
RUN bash bootstrap.sh
RUN ./b2 --prefix=$(pwd)/../boost/ install
WORKDIR "/texada/third_party"
RUN rm -rf boost_1_83_0


RUN git clone https://github.com/google/googletest.git
WORKDIR "/texada/third_party/googletest"
RUN mkdir build
WORKDIR "/texada/third_party/googletest/build"
RUN cmake -DCMAKE_INSTALL_PREFIX=$(pwd)/../install/ ..
RUN make install -j16
WORKDIR "/texada"

RUN make -j16

ENV LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$TEXADA_HOME/third_party/spot/lib/:$TEXADA_HOME/third_party/boost/lib/:$TEXADA_HOME/third_party/googletest/install/lib/




# run the command
CMD ["bash"]

