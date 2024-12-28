
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
- Number of UEs per gNnodeB: 5

### Frequency selection:

Since using milimeter waves is required, our team decided to use the following frequencies:

- Frequency band 1: 26.5 Ghz 
- Frequency band 2:  27 Ghz

Dedicated band for using milimeter waves in Czechia is not available yet, therefore, our team used as a source of the information the following table:

[ctu_table](https://spektrum.ctu.gov.cz/kmitocty?filter%5BfrequencyFrom%5D=1&filter%5BfrequencyFromUnit%5D=GHz&filter%5BfrequencyTo%5D=102&filter%5BfrequencyToUnit%5D=GHz&filter%5BapplicationIds%5D%5B0%5D=47&filter%5BapplicationIds%5D%5B1%5D=182)



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

In this very usecase, we have decided to choose numerology number 3 and number 4. Those numerologies are suitable for milimeter waves technologies and provide more robust solution against Doppler effect in dense urban areas.

### TX Power

In our case, where the distance of the UE from gNodeB is about 20 meters, it is a small cell. In small cells, TX power is used in the range of 23.1 dBm - 30 dBm. A more precise TX power value depends on the physical environmental conditions and the determination of the specific value is done by determining the loss rate.






### Bandwith

There are no specific rules for setting the bandwith value. However, what is known are the maximum bandwith values for each numerology type. For both numerologies 3 and 4, bandwith up to 400 MHz is available. The most commonly used bandwith is 100 MHz. Therefore your team has decided to use this value as the default one for our project. 

- Bandwith: 1 GHz










# NS3

[ns3 examples](https://cttc-lena.gitlab.io/nr/html/group__examples.html)




# Git
[git cheatsheet](https://training.github.com/downloads/github-git-cheat-sheet/)
## usage
#### updates local
```
git fetch
git merge
```
**or**
```
git pull
```
#### updates remote 
```
git add <file>|*
git commit -m "<message>"
git push
```
## access
```
sudo apt install git
sudo apt install gh

gh auth login

cd home/student/lena5g/ns-3-dev/scratch/
mkdir MPA-KPM
cd MPA-KPM

git config user.name "<name>"

gh repo clone Donratto/MPA-KPM
```
