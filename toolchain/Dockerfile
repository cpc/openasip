# syntax=docker/dockerfile:1
FROM ubuntu:xenial

ENV TZ="Etc/UTC"

WORKDIR /root

EXPOSE 5900

RUN rm -f /etc/apt/apt.conf.d/docker-clean; echo 'Binary::apt::APT::Keep-Downloaded-Packages "true";' > /etc/apt/apt.conf.d/keep-cache

RUN --mount=type=cache,target=/var/cache/apt,sharing=locked \
  --mount=type=cache,target=/var/lib/apt,sharing=locked \
  apt update && apt upgrade -y

RUN --mount=type=cache,target=/var/cache/apt,sharing=locked \
  --mount=type=cache,target=/var/lib/apt,sharing=locked \
  apt install tzdata -y

RUN --mount=type=cache,target=/var/cache/apt,sharing=locked \
  --mount=type=cache,target=/var/lib/apt,sharing=locked \
  apt install -y build-essential libboost-all-dev autoconf automake git cmake subversion latex2html libffi-dev

RUN --mount=type=cache,target=/var/cache/apt,sharing=locked \
  --mount=type=cache,target=/var/lib/apt,sharing=locked \
  apt install -y libwxgtk3.0-gtk3-dev tcl8.6-dev libedit-dev libsqlite3-dev sqlite3 libxerces-c-dev g++ make subversion

RUN git clone https://github.com/cpc/tce.git tce-devel

RUN cd tce-devel/tce && tools/scripts/install_llvm_14.sh $HOME/local

COPY tce-env.sh /root/tce-env.sh
COPY tce-build.sh /root/tce-build.sh

RUN /root/tce-build.sh

RUN --mount=type=cache,target=/var/cache/apt,sharing=locked \
  --mount=type=cache,target=/var/lib/apt,sharing=locked \
  apt install -y vim tmux iproute2 xvfb x11vnc wget

RUN cd /root && wget http://openasip.org/tutorial_files/tce_tutorials.tar.gz

RUN --mount=type=cache,target=/var/cache/apt,sharing=locked \
  --mount=type=cache,target=/var/lib/apt,sharing=locked \
  apt install -y fluxbox

COPY start.sh /root/start.sh

CMD sh -c /root/start.sh
