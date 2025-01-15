#Download base image ubuntu 23.04
FROM ubuntu:22.04
# Update Ubuntu Software repository
RUN apt update && apt upgrade -y
ARG DEBIAN_FRONTEND=noninteractive
RUN apt install -y git
RUN apt install -y uuid-dev pkg-config
RUN apt install -y build-essential
RUN apt install -y python3 python3-distutils python3-apt
RUN apt install -y python3-pip 

ENV PYTHON=python3

RUN git clone https://github.com/SamueleGerminiani/samples2ltl.git
WORKDIR "/samples2ltl"

RUN pip install -r requirements.txt
ARG CACHE_DATE=not_a_date
RUN git reset --hard
RUN git pull






# run the command
CMD ["bash"]

