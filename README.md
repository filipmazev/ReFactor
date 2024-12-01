# CodeFU 2024 Hackathon
## ProjectName: AQI Estimator
## Team: Refactor

This project was created for the anual CodeFU hackaton organized by [Netcetera/CodeFu](https://www.netcetera.com/CodeFu)

### Project Description:
This project explores an innovative approach to measuring Air Quality Index (AQI) without relying on traditional sensor-based systems. The goal is to estimate AQI from photographic data, leveraging image properties for the calculation. This method is cost-effective as it eliminates the need for dedicated AQI sensors and associated hardware.

The envisioned use case is to utilize existing city surveillance cameras for capturing images, making use of already established infrastructure. This approach minimizes additional expenses while providing a scalable solution for urban AQI monitoring. By repurposing available resources, this method offers an accessible and efficient alternative to conventional AQI measurement techniques.


## Data Flow Chart
![Architecture Diagram](assets/data-flow-diagram.svg)



## Components

### Image Processor (Native C++ Library)
The native library is located in the `aqi-estimator.libraries/ImageProcessingPipeline` directory. You can find its documentation [here](aqi-estimator.libraries/ImageProcessingPipeline/README.md).


---
---

## Getting Started

### 1. Setting up the Angular front-end application

- First go inside the **aqi-estimator-client** folder and execute
```bash 
npm install
```
- Second start the app using:
```bash
npm start
```

### The application will start on `localhost:4200`

### 2. Setting up the Spring Boot API
- Open the application in your desired spring boot supported code editor/IDE
- Link the **libImagePipeline.so** or **ImageProcessingPipeline.dll** depending on system that to your project which is located in `./src/main/resources/native/`

- you need to change the following properties in `aqi-estamator.server/src/main/resources/application.properties`:

in order to be able to make successfull calls to the `pulse-eco API`

#### refactor.pulse-eco.username=<YOUR_PULSE_ECO_USERNAME>\
#### refactor.pulse-eco.password=<YOUR_PULSE_ECO_PASSWORD>

Then, execute 
```bash
mvn clean install
```
and 
```bash
java -jar aqi-estimator-server-0.0.1-SNAPSHOT.jar
```

#### The API will start on `localhost:8080`



### 3. Setting up the Spring Boot API

- go to `/aqi-estimator.microservices/aqi-estimator.regressor`
- run the following command:
```bash
pip install -r requirements.txt
```
- then start the Flask api using:
```bash
python app.py
```
#### The api should be started on `localhost:5000`