# Use a suitable base image for building the shared library
FROM amazonlinux:2 AS native-build

# Install necessary libraries and build tools
RUN yum update -y && yum install -y \
    gcc \
    gcc-c++ \
    make \
    pkgconfig \
    opencv-devel \
    java-21-amazon-corretto-devel \
    glibc-devel \
    libgcc \
    && yum clean all

# Set the working directory for library compilation
WORKDIR /native

# Copy only the relevant source files for the library
COPY aqi-estimator.libraries/ImageProcessingPipeline/src/unix/dcp ./dcp
COPY aqi-estimator.libraries/ImageProcessingPipeline/src/unix/pipeline ./pipeline
COPY aqi-estimator.libraries/ImageProcessingPipeline/src/unix/native ./native

# Compile the shared library
RUN g++ -std=c++17 -shared -fPIC -z noexecstack \
    -I/usr/lib/jvm/java/include \
    -I/usr/lib/jvm/java/include/linux \
    -I/usr/include/opencv4 \
    ./native/Java_ImagePipeline_ExtractEnhachedMetadata.cpp \
    ./dcp/hazeremoval.cpp \
    ./dcp/guidedfilter.cpp \
    ./pipeline/imagepipeline.cpp \
    -o ./libImagePipeline.so \
    `pkg-config --cflags --libs opencv4`

# Application build stage
FROM maven:3-amazoncorretto-21 AS build

# Set the working directory for the application
WORKDIR /app

# Copy the pom.xml and the source code into the container
COPY . .

# Perform Maven clean install (skip tests)
RUN mvn clean install -DskipTests

# Application runtime stage
FROM amazonlinux:2

# Install necessary runtime libraries
RUN yum update -y && yum install -y \
    java-21-amazon-corretto \
    libc6 \
    libstdc++ \
    opencv \
    && yum clean all

# Set the working directory for the application
WORKDIR /app

# Copy the JAR file from the build stage into the runtime stage
COPY --from=build /app/target/aqi-estimator-server-0.0.1-SNAPSHOT.jar /app/

# Copy the compiled shared library to the target folder
COPY --from=native-build /native/libImagePipeline.so /app/src/main/resources/native/libImagePipeline.so

# Expose the port the Spring Boot app runs on
EXPOSE 8080

# Run the Spring Boot application with the correct path for native libraries
CMD ["java", "-Djava.library.path=src/main/resources/native", "-jar", "aqi-estimator-server-0.0.1-SNAPSHOT.jar"]
