
# MPA-KPM
**Ass**ignment 09 -- 5G NR Simulation in NS-3


# 5G-NSA Network Simulation Project

## 1. Objective
The objective of the project is to gain hands-on experience in simulating a 5G-NSA network using the Network Simulator 3 (NS-3) framework. Students will create a comprehensive network scenario, configure 5G NR parameters, and analyze network performance.

## 2. Task

### 2.1 Scenario Definition
Define a network scenario including, but not limited to, the following elements:
- At least two gNodeBs.
- A minimum of five User Equipment (UE) devices.
- At least one remote host (server) on the Internet.
- Appropriate IP addressing and routing configuration.

### 2.2 Mobility
Use the `GridScenarioHelper` to define the simulation scenario where the UEs are stationary.

### 2.3 NR Configuration
Set the configurations of the gNodeBs and UEs to satisfy the following requirements:
- Use the mmWave frequency band.
- Use at least two different numerologies for the bandwidth parts.
- Set appropriate Tx power for the gNodeBs.

### 2.4 Traffic Generation
Study the available applications in NS-3. Choose suitable applications to simulate the scenario where:
- Three UEs are downloading large files from the server.
- Two UEs are exchanging files between each other.

### 2.5 Simulation Run
- Execute the simulation with different sets of parameters and collect data on network performance.
- Run the simulation for a sufficient duration to observe network behavior.
- Capture and analyze key metrics, such as throughput, latency, and packet loss.
- Create radio environment maps using the `NrRadioEnvironmentMapHelper` in LENA-5G to visualize network coverage, signal quality, and interference.


# Analysis and Report

## Simulation & Network Parameters

### Given Parameters from the assigment:

- Number of gNodeBs: 2
- Number of UEs: 5
- Number of PGWs: 1
- Number of MMEs: 1
- Number of SGWs: 1
- Number of remote hosts / servers: 1

### Frequency selection:

Since using milimeter waves is required, our team decided to use the following frequencies:

- Frequency band 1: 26.5 Ghz 
- Frequency band 2:  27 Ghz

Dedicated band for using milimeter waves in Czechia is not available yet, therefore, our team used as a source of the information the following table:

[Czech Telecommunication Office](https://spektrum.ctu.gov.cz/kmitocty?filter%5BfrequencyFrom%5D=1&filter%5BfrequencyFromUnit%5D=GHz&filter%5BfrequencyTo%5D=102&filter%5BfrequencyToUnit%5D=GHz&filter%5BapplicationIds%5D%5B0%5D=47&filter%5BapplicationIds%5D%5B1%5D=182)



### Numerology

For setting an appropriate numerology value, determining the conditions and used frequency is necessary. In our usecase (using milimeter waves), higher subcarrier spacing is optimal. 

There are five numerology types:

| Numerology (n) | Subcarrier Spacing (Δf) | Typical Use Cases                          | Suitable Frequency Range |
|---------------------|----------------------------|-------------------------------------------|--------------------------|
| 0                   | 15 kHz                    | Voice, broadband, IoT                     | < 6 GHz                  |
| 1                   | 30 kHz                    | eMBB, IoT                                 | < 6 GHz                  |
| 2                   | 60 kHz                    | URLLC, high-data-rate eMBB                | < 6 GHz, mmWave          |
| 3                   | 120 kHz                   | High-speed, mmWave (short latency)        | mmWave                   |
| 4                   | 240 kHz                   | Future ultra-low-latency applications     | mmWave                   |

In this very usecase, we have decided to choose numerology number 3 and number 4. Those numerologies are suitable for milimeter wave technologies and provide more robust solution against Doppler effect in dense urban areas.

### TX Power

In our case the distance of the UE from gNodeB is about 4.24 meters. This distance is considered as a small cell. In small cells, TX power is used in the range of 23.1 dBm - 30 dBm. A more precise TX power value depends on the physical environmental conditions and the determination of the specific value is done by determining the loss rate.


### Bandwidth

There are no specific rules for setting the bandwidth value. However, what is known are the maximum bandwidth values for each numerology type. For both numerologies 3 and 4, bandwidth up to 400 MHz is available. The most commonly used bandwidth is 100 MHz. Therefore your team has decided to use this value as the default one for our project. 


### IP Addressing

| **Device**        | **IP Address Range** | **Assigned IPs**                     |
|--------------------|-----------------------|---------------------------------------|
| Remote Host        | `1.0.0.0/8`          | `1.0.0.2`                             |
| Packet Gateway (PGW)| `1.0.0.0/8`         | `1.0.0.1` <br> Additional: `14.0.0.5`, `7.0.0.1` (multiple interfaces) |
| User Equipment (UE)| `7.0.0.0/8`          | - UE 0: `7.0.0.2` <br> - UE 1: `7.0.0.3` <br> - UE 2: `7.0.0.4` <br> - UE 3: `7.0.0.5` <br> - UE 4: `7.0.0.6` |
| Base Stations (gNB)| `10.0.0.0/8`         | - gNB 0: `10.0.0.5` <br> - gNB 1: `10.0.0.9` |
| Mobility Management Entity (MME) | Not assigned         | Integrated into EPC (control plane only) |



### GridScenarioHelper

The GridScenarioHelper class in the script is used to create and configure a grid-based deployment of network nodes (gNBs and UEs) in a simulation scenario. It simplifies the process of arranging nodes in a structured, grid-like layout and managing their spatial properties. In case of our project, determining the position of UEs is not clear. To set their position, pseudorandom generator is used, so we can predict only a radius, in which they can be placed. However, in our case, the same random number generator is used each time, therefore UEs will end up always in the same position.

The following table shows the position of devices used in our scenario:

| **Device**        | **X Position**    | **Y Position**    |
|--------------------|-------------------|-------------------|
| gNB 0             | 10.0             | 0.0              | 
| gNB 1             | 0.0              | 0.0              | 
| UE 0              | 2.107819727      | 2.07726695       | 
| UE 1              | 12.09425054      | 2.351660078      | 
| UE 2              | 1.863272216      | 1.303696692      | 
| UE 3              | 12.68766751      | 0.8667490141     | 
| UE 4              | 1.530488989      | 1.397013425      | 
| PGW               | 10.0             | 5.0              | 
| SGW               | 5.0              | 5.0              | 
| MME               | 0.0              | 5.0              | 
| Remote Host       | 10.0             | 7.5              | 



![Visualisation of the output from GridScenarioHelper](https://github.com/Donratto/MPA-KPM/blob/main/Pictures/GridScenarioHelper.png)



### Simulation RUN

The simulation has been executed several times, always with a different sets of parameters.
The output of the simulation is a basic listing of the communication flow and important values that accompany the simulation, such as delay, throughput and packet loss, in the console bar.
For the purpose of this documentation, you can find a set of different outputs bellow.


The first simulation has the following set of parameters (possition of each device is as it is shown above):

- `simTime`: 1000 ms
- `AppStartTime`: 400 ms
- `numerologyBwp1`: 4
- `centralFrequencyBand1`: 26.5 GHz
- `bandwidthBand1`: 100 MHz
- `numerologyBwp2`: 2
- `centralFrequencyBand2`: 27 GHz
- `bandwidthBand2`: 100 MHz
- `totalTxPower`: 4


[Output 1](https://github.com/Donratto/MPA-KPM/blob/main/Outputs/output1.txt)


It has been discovered, that changing the simulation time has a signifficant effect only on the packet loss value. The difference between those two outputs is visible in the following output:

- `simTime`: 1000 ms

[Output 2](https://github.com/Donratto/MPA-KPM/blob/main/Outputs/output2.txt)

Changing the `totalTxPower` value while maintaining the same position of all devices has no effect on the output of the simulation.


The most significant differences can be observed in the following output, where we manually set the position of UEs. Their distance from the gNodeBs is decreased, therefore dramatic decrease in terms of throughput can be observed. Updated positions can be seen bellow:


![updated positions](https://github.com/Donratto/MPA-KPM/blob/main/Pictures/image.png)


[Output 3](https://github.com/Donratto/MPA-KPM/blob/main/Outputs/output3.txt)



In addition to the environment map, the NrRadioEnvironmentMapHelper can also be used to draw the Coverage area, Beamshape and Beamshape for UL. In Coverage area, Beamshape and Beamshape for UL we have graphically recorded the values of SNR (Signal-to-Noise Ratio), SINR (Signal-to-Interference-plus-Noise Ratio), IPSD (Impulse Signal Power Distribution) and SIR (Signal-to-Interference Ratio).

![Visualisation of the output from GridScenarioHelper](https://github.com/Donratto/MPA-KPM/blob/main/Pictures/GridScenarioHelper.png)

### Coverage area

![Visualisation of the output from GridScenarioHelper](https://github.com/Donratto/MPA-KPM/blob/main/Pictures/Coverage_area/CA_nr-rem-default-snr.png)
![Visualisation of the output from GridScenarioHelper](https://github.com/Donratto/MPA-KPM/blob/main/Pictures/Coverage_area/CA_nr-rem-default-sir.png)
![Visualisation of the output from GridScenarioHelper](https://github.com/Donratto/MPA-KPM/blob/main/Pictures/Coverage_area/CA_nr-rem-default-sinr.png)
![Visualisation of the output from GridScenarioHelper](https://github.com/Donratto/MPA-KPM/blob/main/Pictures/Coverage_area/CA_nr-rem-default-ipsd.png)

### Beamshape

![Visualisation of the output from GridScenarioHelper](https://github.com/Donratto/MPA-KPM/blob/main/Pictures/Beamshape/nr-rem-default-snr.png)
![Visualisation of the output from GridScenarioHelper](https://github.com/Donratto/MPA-KPM/blob/main/Pictures/Beamshape/nr-rem-default-sir.png)
![Visualisation of the output from GridScenarioHelper](https://github.com/Donratto/MPA-KPM/blob/main/Pictures/Beamshape/nr-rem-default-sinr.png)
![Visualisation of the output from GridScenarioHelper](https://github.com/Donratto/MPA-KPM/blob/main/Pictures/Beamshape/nr-rem-default-ipsd.png)

### BeamshapeUL

![Visualisation of the output from GridScenarioHelper](https://github.com/Donratto/MPA-KPM/blob/main/Pictures/Beamshape_ul/nr-rem-default-snr.png)
![Visualisation of the output from GridScenarioHelper](https://github.com/Donratto/MPA-KPM/blob/main/Pictures/Beamshape_ul/nr-rem-default-sir.png)
![Visualisation of the output from GridScenarioHelper](https://github.com/Donratto/MPA-KPM/blob/main/Pictures/Beamshape_ul/nr-rem-default-sinr.png)
![Visualisation of the output from GridScenarioHelper](https://github.com/Donratto/MPA-KPM/blob/main/Pictures/Beamshape_ul/nr-rem-default-ipsd.png)


###  Issues encountered during the simulation 

During this project, our team encountered several problems, mainly related to the NS3 tool. The lack of documentation in some parts of the project, along with difficulties in determining the causes of problems in the software, made certain aspects of this project very challenging. 

### Recommendations for improving network performance

