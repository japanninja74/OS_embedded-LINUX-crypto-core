# Testing the platform on Vitis  
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
# Creating the Driver
# Creating the Test Application
