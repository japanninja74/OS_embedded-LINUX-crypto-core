LIBRARY ieee;
USE ieee.std_logic_1164.ALL;
 
ENTITY core_wrapper_tb IS
END core_wrapper_tb;
 
ARCHITECTURE behavior OF core_wrapper_tb IS 
 
    -- Component Declaration for the Unit Under Test (UUT)
 
    component CORE_WRAPPER is
	port (
		clk: in std_logic;
		rst: in std_logic;
		
		key: in std_logic_vector(63 downto 0); -- key MAP: (56,48,40,32,24,16,8,0) used for parity check, others used for key.
		input_word: in std_logic_vector(63 downto 0);
		control_word_in: in std_logic_vector(63 downto 0); --control MAP: (63 downto 3) unused, 2 reload, 1 mode, 0 start. mode is 0 to encrypt 1 to decrypt
		
		output_word: out std_logic_vector(63 downto 0);
		control_word_out: out std_logic_vector(63 downto 0) --control MAP: (63 downto 3) unused, 2 done, 1 busy, 0 ready.
	);
	end component;
    

   --Inputs
   signal clk : std_logic := '0';
   signal rst : std_logic := '1';
   signal key_in,blk_in,control_in,control_out,blk_out : std_logic_vector(63 downto 0) := (others => '0');

   -- Clock period definitions
   constant clk_period : time := 10 ns;
 
BEGIN
 
	-- Instantiate the Unit Under Test (UUT)
   uut: CORE_WRAPPER PORT MAP (
          clk => clk,
          rst => rst,
		  key => key_in,
          input_word => blk_in,
          control_word_in => control_in,
		  output_word => blk_out,
		  control_word_out => control_out
        );

   -- Clock process definitions
   clk_process :process
   begin
		clk <= '0';
		wait for clk_period/2;
		clk <= '1';
		wait for clk_period/2;
   end process;
 

   -- Stimulus process
   stim_proc: process
   begin
        wait for 150ns;		
		wait for clk_period/2 + clk_period;
		blk_in <= X"4E45565251554954";
		key_in <= X"4B41534849534142";
		control_in <= X"0000000000000001";
		wait for clk_period/2 + clk_period;
		rst <= '0';
      wait for clk_period*20;
		
		assert blk_out = X"763549D38B570C0E"
			report "ENCRYPT ERROR" severity FAILURE;
			
		wait for clk_period;

		control_in <= X"0000000000000004";
		blk_in <=  X"763549D38B570C0E";
		key_in <=  X"4B41534849534142";
		wait for clk_period;
		control_in <= X"0000000000000003";
      wait for clk_period*20;		

		assert blk_out = X"4E45565251554954"
			report "DECRYPT ERROR" severity FAILURE;

      wait;
   end process;

END;