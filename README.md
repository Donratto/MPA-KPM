
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

## Network Parameters

- Number of gNodeBs:
- Number of UEs per gNnodeB:
- Number of traffic flows per gNodeB:
- 










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
