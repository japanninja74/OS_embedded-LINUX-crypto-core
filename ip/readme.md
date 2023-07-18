- go to `tools>create and package new ip`
- select next, select `create a new AXI4 peripheral`
- name it `crypto_core_IP`
  ![](https://github.com/Giuseppe-La-Capra/embedded-LINUX-crypto-core/blob/37948db52606f45b5e17723d4aa23545a7f579c7/ip/Screenshot%20from%202023-06-04%2000-59-53.png)
- when promped to specify the AXI proprieties, change the number of registers to 5
  ![](https://github.com/Giuseppe-La-Capra/embedded-LINUX-crypto-core/blob/7282e9dd73bcdb285fd12fccb9a02565a35a1cb0/ip/Screenshot%20from%202023-06-04%2001-00-10.png)
- select `edit ip` and then `finish`
  ![](https://github.com/Giuseppe-La-Capra/embedded-LINUX-crypto-core/blob/7282e9dd73bcdb285fd12fccb9a02565a35a1cb0/ip/Screenshot%20from%202023-06-04%2001-00-25.png)
- add all files from the `vhdl/src/` folder as sources
  ![](https://github.com/Giuseppe-La-Capra/embedded-LINUX-crypto-core/blob/7282e9dd73bcdb285fd12fccb9a02565a35a1cb0/ip/Screenshot%20from%202023-06-04%2001-01-00.png)
- at this point you can follow steps 3 to 6 in the `readme.txt` located in the `vhdl/` folder to add all the required IPs
- once you have added all IPs you can modify the vhdl file to include the core 
- connect the core to the registers
  - the details of how to modify the IP to add the core can be found in the . folder
- run a synthesis to save the design and create the netlist  
