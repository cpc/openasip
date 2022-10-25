# syntax=docker/dockerfile:1
FROM ubuntu

RUN rm -f /etc/apt/apt.conf.d/docker-clean; echo 'Binary::apt::APT::Keep-Downloaded-Packages "true";' > /etc/apt/apt.conf.d/keep-cache

RUN --mount=type=cache,target=/var/cache/apt,sharing=locked \
  --mount=type=cache,target=/var/lib/apt,sharing=locked \
  apt update && apt upgrade -y

RUN --mount=type=cache,target=/var/cache/apt,sharing=locked \
  --mount=type=cache,target=/var/lib/apt,sharing=locked \
  apt install tzdata -y

ENV TZ="America/Winnipeg"

RUN --mount=type=cache,target=/var/cache/apt,sharing=locked \
  --mount=type=cache,target=/var/lib/apt,sharing=locked \
  apt install -y build-essential libboost-all-dev autoconf automake git cmake subversion latex2html libffi-dev

RUN --mount=type=cache,target=/var/cache/apt,sharing=locked \
  --mount=type=cache,target=/var/lib/apt,sharing=locked \
  apt install -y libwxgtk3.0-gtk3-dev tcl8.6-dev libedit-dev libsqlite3-dev sqlite3 libxerces-c-dev g++ make subversion

WORKDIR /root

RUN git clone https://github.com/cpc/tce.git tce-devel

RUN cd tce-devel/tce && tools/scripts/install_llvm_14.sh $HOME/local

CMD while true; do sleep 10; done;
