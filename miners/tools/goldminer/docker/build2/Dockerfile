#Download base image ubuntu 23.04
FROM ubuntu:22.04
# Update Ubuntu Software repository
RUN apt update --fix-missing
ARG DEBIAN_FRONTEND=noninteractive
RUN apt install -y wget
RUN apt install -y python2
RUN apt install -y python2.7-dev
RUN apt install -y python-pip
RUN apt install -y git
RUN apt install -y uuid-dev pkg-config
RUN apt install -y build-essential
RUN apt install iverilog
RUN apt install -y vim
ENV PYTHON=python2

RUN git clone https://bitbucket.org/debjitp/goldminer.git
WORKDIR "/goldminer"

RUN apt install -y graphviz libgraphviz-dev
RUN pip2 install -r requirements.txt

RUN wget https://www.dropbox.com/s/e1wf6vh64v9tpf1/pyverilog_customized.tar.gz?dl=0 -O pyverilog.tar.gz
RUN tar -xzvf pyverilog.tar.gz -C /usr/lib/python2.7/

WORKDIR "/goldminer/src/vcd_parser"
RUN python2.7 setup.py build_ext --inplace
RUN ln -s ./vcd_parser/parse_timeframes.so
WORKDIR "/goldminer"
RUN mkdir work
WORKDIR "/goldminer/work"




# run the command
CMD ["bash"]

