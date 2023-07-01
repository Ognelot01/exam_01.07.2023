FROM ubuntu:18.04

#="Tunyan_Ararat_221_353"

ENV TZ=Europe/Moscow     
      RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone     
      RUN apt-get update     
      RUN apt-get install qt5-default -y     
      RUN apt-get install qtbase5-dev -y     
      RUN apt-get install qt5-qmake     
      RUN apt-get install build-essential -y .   
WORKDIR /221-353_Ararat_Tunyan
COPY . .
RUN qmake myShop.pro&&make
CMD ["./myShop"]