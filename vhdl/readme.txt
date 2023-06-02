-------------------------------------------HOW TO ADD CORE WRAPPER TO YOUR VIVADO PROJECT-------------------------------------------

1: add all src files to your vivado project

2: if you wish to run a simulation, add the testbench file located in the tb folder to the project as simulation only 

3: open ip catalog and select "Distributed Memory Generator (8.0)"
	set component name to "s_box_dram_1";
	under "memory config" select Depth = 64, Data Width = 4, Memory Type = ROM;
	leave "Port Config" as it is;
	under "RST & Initialization" select Coefficients File = .../mem_init_files/s_box_v_1.coe

4: repeat step 3 seven more times to create s_box_dram_2 to s_box_dram_8 
	take care to select the correct .coe file for each memory.

5: open ip catalog and select "Binary Counter (12.0)"
	set component name to "c_counter_binary_0";
	under "Basic" select Output Width = 4;
	under "Control" select Clock Enable, Synchonous Clear

6: you're done, run a synthesis and, if you wish, a simulation.


-------------------------------------------           CORE IMPLEMENTATION NOTES           -------------------------------------------

Whenever the core is ready, conversion starts when the start control bit is set to 1.
Conversion is not stopped until it is done converting or a reset happens. IMPORTATANT: data and key should remain unchanged during conversion and should also be ready before start of conversion.
When the conversion is done, the output of the core wrapper remains unchanged and the done bit of the control output is asserted.
If a second conversion is to be done the reload bit should be asserted followed by the start bit, provided that key and data are ready. 

-------------------------------------------           REVISIION NOTES           -------------------------------------------

REV3_CHANGELOG: 
implemented the FSM for the CU in a way that vivado understands, note that the behavioural simulation does not work anymore whereas the post synthesis functional simulation works as intended. Previous implementation had an issue with the number of states generated, generating more than 100 states using ONE_HOT encoding.
REV4_CHANGELOG:
added clk to a process sensitivity list in CU, probably the cause for behavioural simulation not working in REV3

