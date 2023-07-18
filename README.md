# embedded-LINUX-crypto-core
## introduction
This repo has been created by group #5 of the OS course.  
This repo describes the complete workflow in order to get an embedded system working on the Pynq Z2, starting from complete scratch.  
The flow is organized as a laboratory experience where you'll learn how to create,package and finally boot an embedded linux image that supports a custom hardware application.  
<br>
**COLLABORATORS:**  
Giuseppe La Capra  
Lorenzo Crupi  
Andrea Comberiati  
Pietro Furbatto  
Jessica Marossero  
Filippo Marostica  
Fabrizio Cavallone  
# GETTING STARTED
## pre-requisites
In order to insure compatibility the following steps are required:  
- if you want to build your own BSP check the vivado version used by the [pynq project](https://github.com/xilinx/pynq). At the time of writing the compatible version is the 2022.1 but this **could change in the future**. The pre-packaged BSP supports version 2022.1 
- the same version should be used for the petalinux software, note that [petalinux only supports some linux distros](https://docs.xilinx.com/r/2022.1-English/ug1144-petalinux-tools-reference-guide/Installation-Requirements "petalinux 2022.1 installation requirements"). For petalinux 2022.1 ubuntu 20.04 was used.  
  - make sure you read the petalinux installation requirements carefully, expecially the free space requirement and the `bin/sh` shell requirement.  
  - make sure to install the required packages referenced in the excel file located in the [petalinux release notes webpage](https://support.xilinx.com/s/article/000033799?language=en_US) **for your installed version**.  
<br>

## Installing the software
- download and install the required version of vitis and vivado from the [xilinx website](https://www.xilinx.com/support/download/index.html/content/xilinx/en/downloadNav/vivado-design-tools/2022-1.html).
- download and install the required version of petalinux from the [xilinx website](https://www.xilinx.com/support/download/index.html/content/xilinx/en/downloadNav/embedded-design-tools/2022-1.html).  
<br>

## Laboratory experience flow
- you will first need to either build your own or download the BSP
- once you have the BSP you will need to create the custom hardware platform in vivado using the provided VHDL files
- after exporting the hardware platform, you will need to test the hardware in baremetal using vitis
- you will then need to create a new petalinux project using the bsp and the hardware platform
- after that you will build the kernel
- once you are done building you will need to package the boot components and load them into an sd card
- after booting from the sd card and making sure that the linux image works correctly, you will be asked to create a custom linux driver
- you will then be walked through the steps required to load the driver and test it using a custom user application
<br>

# The pynq-z2 BSP
In order to create a petalinux project we first need a BSP.  
The BSP for the pynq board is not provided by the board maker but you can create it using petalinux from the [pynq project](https://github.com/xilinx/pynq).  
In order to save time, a version of the bsp can be downloaded [here](https://mega.nz/file/NHNHzTYB#UCP6dy67P80xzV5r_mvvZvwBLSUyGVFZmOuWuZBe2bQ). This version only works with petalinux 2022.1.  

## Creating the BSP from scratch  
- open a new terminal on your desktop
- install the git application 
  ```console
  sudo apt install git
  ```
- clone the pynq project repository to your local machine 
  ```console
  git clone https://github.com/Xilinx/PYNQ.git
  ```
- move to the `boards/pynq_z2/base` folder in the newly created PYNQ folder.  
  ```console
  cd PYNQ/boards/Pynq-Z2/base/
  ```
- source the vivado enviroment variables. Replace `<your vivado install dir>` with the installion directory of the vivado tool  
  ```console
  source <your vivado install dir>/settings64.sh
  ```
- run the `make` command  
  ```console
  make
  ```
  - please note that this may take a **very long time**, just let it run.
- once the program finishes you can close the terminal.
- open a new terminal in the desktop
- source the petalinux enviroment variables. Replace `<your petalinux install dir>` with the installion directory of the petalinux tool
  ```console
  source <your petalinux install dir>/settings.sh
  ```
- create a new project from template
  ```console 
  petalinux-create --type project --template zynq --name PYNQ_Z2
  ```
- move to the newly created folder
  ```console
  cd PYNQ_Z2/
  ```
- run the `petalinux-config` command specifing the hardware platform
  ```console
  petalinux-config --get-hw-description ../PYNQ/boards/Pynq-Z2/base/base.xsa
  ```
  - this will open a configuration tab. You don't need to modify anything at this stage so you can close and save by navigating to the `<exit>` option.
- at this point you can replace the files located in `PYNQ_Z2/project-spec/metauser/recipes-bsp` with the ones found inside `PYNQ/boards/Pynq-Z2/petalinux_bsp/meta-user/recipes-bsp`.
  - this step will modify the `system-user.dtsi` file and adds some board specific patches for U-boot.
- run the `petalinux-package` command to package the BSP
  ```console
  petalinux-package --bsp -p ../PYNQ_Z2 --output pynq_z2_bsp
  ```
- you have successfully created your bsp which can be found in the project directory.  
<br>

# Creating the hardware platform
In the vhdl folder you can find everything you need to use the crypto core. There you will also find a readme file with the steps needed to instantiate all the required IPs.  
  
- open vivado and create a new project 
- when promped to add sources, leave it empty but make sure to select the target language as vhdl
- when promped to select the default xilinx part go to `boards` and select the pynq-z2 board by tul.com
  - if you don't have the pynq-z2 board as a choice click on `refresh` and download the board  

At this point you will need to add a custom IP to the IP Catalog which will be your HW core.  

## Creating the IP from scratch
Creating the IP from scratch is too convoluted to be explained on the main readme so another readme with a comprehensive step-by-step guide is located in the `ip` folder of this repository.  

## Packaging the platform
At this point you should see your IP in the IP Catalong under `user repository`:
- create a new block diagram
- add the `zynq7000` IP by selecting the `+` icon
- add your custom IP the same way
- an option should appear to run automated connections, click on it
- at this point all blocks should result connected and some more blocks should have been created, namely the AXI interconnect and the clock/reset control block.
- click on the block diagram and select `create HDL wrapper`
- click on . and select `generate bitstream`
- click on . and select `export hardware platform`  
<br>
# Testing the hardware platform in baremetal  
- open vitis and press `launch`
- select `create application project`
- select next and then select `create a new platform from hardware (XSA)`
- browse to the location of the .xsa file and select it
- press next
- give the application project a name, select next until you can select finish

At this point you should have a basic hello world project open on vitis.  
Please refer to the `Lab_Assignment.md` file for further information.  
<br>
# Creating the petalinux project
- open a new terminal in the desktop
- source the petalinux enviroment variables. Replace `<your petalinux install dir>` with the installion directory of the petalinux tool
  ```console
  source <your petalinux install dir>/settings.sh
  ```
- create a new project from bsp
  ```console 
  petalinux-create -t project -s <path-to-bsp> --name PROGETTO
  ```
- move to the newly created folder
  ```console
  cd PROGETTO/
  ```
- run the `petalinux-config` command specifing the .xsa file you created earlier
  ```console
  petalinux-config --get-hw-description <path to .xsa file>
  ```
  - this will open a configuration tab, do not close this tab because you need to modify the kernel bootargs.
- navigate to `DTG settings` and press select
- navigate to the last entry and copy the contents, press ok when you are done
- navigate to `generate boot args automatically` and press N to remove the selection
  - this will delete the bootargs that where automatically generated (the ones you copied earlier)
- navigate to `user set kernel bootargs` and paste the bootargs you copied
- add `clk_ignore_unused` to the bootargs after the console bootarg
  - it should look something like this
    ```console
    console=ttyPS0,115200 clk_ignore_unused earlycon root=/dev/ram0 rw 
    ```
  - this step is needed because normally the linux Common Clock Framework will deactivate unused clocks, including the axi clock, making the hw unusable
- click ok followed by exit unitil you can exit the configurator
- make sure you click yes when promped to save and exit.
- you have successfully created your petalinux project.
<br>
# Adding custom modules and applications
In order to make the hw usable we will create a custom kernel module (acting as a driver) and a user application (acting as a test application).  
This two pieces of software will be embedded into the linux image meaning that we will later be able to find them in the filesystem and run them.  
It is done this way because the linux image does not have any compilers and therefore we cannot compile software on the board.  
Technically it is possible to cross compile using Vitis and upload the apps using an ethernet connection but we weren't able to make it work.  

In order to create a new app or module:
- navigate to the project's directory
- source the petalinux enviroment variables if you haven't done it already
  ```console
  source <your petalinux install dir>/settings.sh
  ```
- create a new app:
  ```console 
  petalinux-create -t apps --template c --name <appname> --enable
  ```
  - please only use lower case letters and no `_` in the appname or follow [this](https://docs.xilinx.com/r/2022.1-English/ug1144-petalinux-tools-reference-guide/Recipe-Name-has-_-or-Uppercase-Letters-or-Starts-with-an-Uppercase-Letter) if you do.
- create a new kernel module:
  ```console 
  petalinux-create -t modules --name <modulename> --enable
  ```
  - please only use lower case letters and no `_` in the modulename or follow [this](https://docs.xilinx.com/r/2022.1-English/ug1144-petalinux-tools-reference-guide/Recipe-Name-has-_-or-Uppercase-Letters-or-Starts-with-an-Uppercase-Letter) if you do.

You can now find the apps and modules under the `PROGETTO/project-spec/meta-user/recipes-apps` and `PROGETTO/project-spec/meta-user/recipes-modules` directories.  
Here you can modify the .c file related to the app/module to your liking.  
NOTE: do not modify the names of the .c files. The apps and modules are compiled during the building of the image; in order to avoid having to build multiple times because of typos and programming mistakes, check if the .c files compile using gcc on your machine.  
<br>
# Building and Packaging
<br>

# Booting and Testing
