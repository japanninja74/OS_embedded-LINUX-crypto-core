# embedded-LINUX-crypto-core
## introduction
This repo has been created by group #5 of the OS course.  
In this repo you can find everything needed to complete the laboratory experience, including the solutions.  
<br>
# GETTING STARTED
## pre-requisites
In order to insure compatibility the following steps are required:  
- check the vivado version used by the [pynq project](https://github.com/xilinx/pynq). At the time of writing the compatible version is the 2022.1 but this **could change in the future**.  
  - download and install the required version of the software from the [xilinx website](https://www.xilinx.com/support/download/index.html/content/xilinx/en/downloadNav/vivado-design-tools/2022-1.html).  
  - make sure you install both vitis and vivado since you will need to use both.
- the same version should be used for the petalinux software, note that [petalinux only supports some linux distros](https://docs.xilinx.com/r/2022.1-English/ug1144-petalinux-tools-reference-guide/Installation-Requirements "petalinux 2022.1 installation requirements"). For petalinux 2022.1 ubuntu 20.04 was used.  
  - make sure you read petalinux installation requirements carefully, expecially the free space requirement and the bin/sh shell requirement.  
  - download and install the petalinux software from the [xilinx website](https://www.xilinx.com/support/download/index.html/content/xilinx/en/downloadNav/embedded-design-tools/2022-1.html).  
  - make sure to install the required packages referenced in the excel file located in the [petalinux release notes webpage](https://support.xilinx.com/s/article/000033799?language=en_US) **for your installed version**.  
<br>

## The pynq-z2 BSP
In order to create a petalinux project we first need a BSP.  
The BSP for the pynq board is not provided by the board maker but you can create it using petalinux from the [pynq project](https://github.com/xilinx/pynq).  
In order to save time, a version of the bsp can be downloaded [here](https://mega.nz/file/NHNHzTYB#UCP6dy67P80xzV5r_mvvZvwBLSUyGVFZmOuWuZBe2bQ). This version only works with petalinux 2022.1.  

### Creating the BSP from scratch  
- install the git application 
  > sudo apt install git
- clone the pynq project repository to your local machine 
  > git clone https://github.com/Xilinx/PYNQ.git
- move to the boards folder in the newly created 
<br>

## creating the hardware project
In the vhdl folder you can find everything you need to use the crypto core. There you will also find a readme file with the steps needed to instantiate all the required IPs.  
