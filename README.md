# CodeFU 2024 Hackathon
## ProjectName: AQI Estimator
## Team: Refactor

This project was created for the anual CodeFU hackaton organized by [Netcetera/CodeFu](https://www.netcetera.com/CodeFu)

### Project Description:
This project explores an innovative approach to measuring Air Quality Index (AQI) without relying on traditional sensor-based systems. The goal is to estimate AQI from photographic data, leveraging image properties for the calculation. This method is cost-effective as it eliminates the need for dedicated AQI sensors and associated hardware.

The envisioned use case is to utilize existing city surveillance cameras for capturing images, making use of already established infrastructure. This approach minimizes additional expenses while providing a scalable solution for urban AQI monitoring. By repurposing available resources, this method offers an accessible and efficient alternative to conventional AQI measurement techniques.



## Components

### Image Processor (Native C++ Library)
The native library is located in the `aqi-estimator.libraries/ImageProcessingPipeline` directory. You can find its documentation [here](aqi-estimator.libraries/ImageProcessingPipeline/README.md).

### Backend (Spring Boot API)
Documentation for the backend application can be found [here](backend/README.md).


### Backend (Flask APP)


---

## Getting Started

To learn how to set up the full project, refer to the individual component documentation linked above.

To start the backend application, you need to change the following properties in `aqi-estamation.server/src/main/resources/application.properties`:

refactor.pulse-eco.username=<YOUR_PULSE_ECO_USERNAME>
refactor.pulse-eco.password=<YOUR_PULSE_ECO_PASSWORD>

Then, execute 
```bash
mvn clean install
```
and 
```bash
java -jar aqi-estimator-server-0.0.1-SNAPSHOT.jar
```
