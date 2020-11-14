
FROM openjdk:8
WORKDIR /usr/src/E6
COPY . /usr/src/E6

RUN javac -d ./out/production/E6/ ./src/*.java
CMD ["java","-cp","out/production/E6","ServerImpl","1099","E6"]
