#Concedendo todas as permissoes na pasta
chown nobody -R /Share/ocr-server  && chmod 777 -R /Share/ocr-server

#Pre-requisitos que serao instalados via Aptitude
aptitude install g++ autoconf automake libtool autoconf-archive pkg-config libpng-dev libjpeg8-dev libtiff5-dev zlib1g-dev libicu-dev libpango1.0-dev libcairo2-dev libpng12-dev libjpeg62-dev libtiff4-dev zip unzip build-essential cmake libtool yasm pkg-config subversion git libgcj14 libtiff-dev libpng-dev libopenjpeg-dev libjpeg8-dev libjpeg-turbo8-dev libjpeg-dev libgif-dev zlib1g-dev libicu-dev libpango1.0-dev libcairo2-dev libfontconfig1-dev libgettextpo-dev libcurl-dev libnss3-dev libfile-find-rule-perl libfile-find-rule-perl-perl libtiff5 libpng12-0 libopenjpeg2 libjpeg-turbo8 libgif4 zlib1g libicu52 libpango1.0-0 libcairo2 fontconfig imagemagick gettext libwebp5 libgcj14 libtiff5 libpng12-0 libopenjpeg2 libjpeg8 libjpeg-turbo8 libjpeg8 zlib1g libpango1.0-0 libcairo2 libfontconfig1 libgettextpo0 ghostscript ocaml libgcj gcj-jdk sysv-rc-conf -y
apt-get remove ghostscript -y

#instalacao dos Pre-requisitos Perl
cpan install File::Touch module File::Find::Rule File::Basename File::Copy File::Path File::Touch Sys::Syslog Sys::Hostname IPC::Open3 IO::Select POSIX

#Instalacao do Ghostscript 9.22
cd /Share/ocr-server/pre-requisitos/ghostscript-9.22/ && ./autogen.sh && make all && make install

#Instalacao do Cpdf
cd ../cpdf/pre-requisitos/findlib-1.7.3 && ./configure && make && make install && cd ../camlpdf-2.2.1 && make && make install
cp /Share/ocr-server/ocr-server.git/usr/local/bin/cpdf /usr/local/bin/cpdf
chmod 777 -R /usr/local/bin/cpdf

#Instalacao do pdftk 2.02
cd /Share/ocr-server/pre-requisitos/pdftk-2.02-dist/pdftk && make all -f Makefile.Debian && make install -f Makefile.Debian

#Instacao do poppler-utils 0.42.0
cd ../../poppler-0.42.0 && ./configure && make && make install

#Instalacao do Leptonica 1.74.4
cd ../tesseract/leptonica/pre-requisitos/plotlib-1.2/src
make all
make install
cd ../prog
make all
make install
cd /Share/ocr-server/pre-requisitos/tesseract/leptonica/leptonica-1.74.4 && ./configure && make && make install

#instalacao do Tesseract 4.0.0
cd ../../tesseract-master && ./autogen.sh && ./configure && make && make install
cp /Share/ocr-server/pre-requisitos/tesseract/tesseract-master/tessdata/eng.traineddata /usr/local/share/tessdata/eng.traineddata && cp /Share/ocr-server/pre-requisitos/tesseract/tesseract-master/tessdata/por.traineddata /usr/local/share/tessdata/por.traineddata && cp /Share/ocr-server/pre-requisitos/tesseract/tesseract-master/tessdata/equ.traineddata /usr/local/share/tessdata/equ.traineddata && cp /Share/ocr-server/pre-requisitos/tesseract/tesseract-master/tessdata/osd.traineddata /usr/local/share/tessdata/osd.traineddata
/sbin/ldconfig

#Instalacao do OCR
cp /Share/ocr-server/ocr-server.git/usr/local/bin/ocr /usr/local/bin/ocr && cp /Share/ocr-server/ocr-server.git/etc/init.d/ocr-ubuntu /etc/init.d/ocr && mkdir -p /var/ocr-server/Entrada && mkdir -p /var/ocr-server/Erro && mkdir -p /var/ocr-server/Originais_Processados && mkdir -p /var/ocr-server/Saida && chown nobody -R /var/ocr-server/Entrada && chmod 777 /var/ocr-server/Entrada && chown nobody -R /var/ocr-server/Originais_Processados && chmod 777 /var/ocr-server/Originais_Processados && chown nobody -R /var/ocr-server/Erro && chmod 777 /var/ocr-server/Erro && chown nobody -R /var/ocr-server/Saida && chmod 777 /var/ocr-server/Saida

#Compartilhando as pastas
/Share/ocr-server/comp_ocr_folders.out
smbd -s --configfile=/etc/samba/smb.conf
