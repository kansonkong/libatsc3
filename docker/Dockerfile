FROM archlinux/base
LABEL maintainer="Jon Ribeiro <contact@jonathas.com>"

USER root

RUN useradd -ms /bin/bash app && ln -s /usr/include/locale.h /usr/include/xlocale.h
RUN pacman -Syy && pacman -Sy tar sudo git gcc gawk make cmake --noconfirm

USER app
WORKDIR /home/app/src

ENTRYPOINT ["./build_linux.sh"]
