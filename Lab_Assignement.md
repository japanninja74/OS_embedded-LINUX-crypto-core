# EX1: Testing the platform on Vitis  
Under the `<name>_system > src` folder you can find the `helloworld.c` program whereas under the `<xsa_file_name> > hw > drivers > <ip_name> > src` you can find
the .h file that defines some useful read and write macros you should use and the address offsets.  
The base address can be found under `<xsa_file_name> > hw > <xsa_file_name>.xsa`, just scroll to the bottom of the list and you should see the name of your ip with the base address.  
At this point you have everything you need to create a test application to test the functionality of the hardware.  

When you're done creating the application create a new command shell console:
- connection type: serial port
- select `new...` under connection name and select the right usb port (the one connected to the pynq board)
  - if you don't see any usb ports it means that you need to change the permission of the usb device found under the `/dev` directory
- select ok

Make sure you have turned on the board and that you have unplugged the boot jumper from the board, this will make the board ready for flashing.  
At this point you can build the application and by clicking on the play icon.
# EX2: Creating the Driver  
In this exercise you are required to write a driver starting from a template that can be found under the `Templates` directory.  
In order to communicate with a memory mapped device it's not sufficient to access memory using pointers, if you do this using the base address the driver with segfault.  
This is because the memory of the device is CPU-memory whereas the driver is in a limited part of kernel memory.  
In order to link a piece of kernel memory to CPU-memory, use the following function:  
```console
void __iomem * ioremap(BASE_ADDRESS, SIZE)
```
`BASE_ADDRESS` of type `phys_address_t` being the base address of the core and `SIZE` of type `size_t` being the memory depth of the core in bytes (4 bytes for each register in the axi).  

# EX3: Creating the Test Application
