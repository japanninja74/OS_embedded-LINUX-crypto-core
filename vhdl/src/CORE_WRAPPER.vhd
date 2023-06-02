library ieee;
use ieee.std_logic_1164.ALL;

entity CORE_WRAPPER is
port (
	clk: in std_logic;
	rst: in std_logic;
	
	key: in std_logic_vector(63 downto 0); -- key MAP: (56,48,40,32,24,16,8,0) used for parity check, others used for key.
	input_word: in std_logic_vector(63 downto 0);
	control_word_in: in std_logic_vector(63 downto 0); --control MAP: (63 downto 3) unused, 2 reload, 1 mode, 0 start. mode is 0 to encrypt 1 to decrypt
	
	output_word: out std_logic_vector(63 downto 0);
	control_word_out: out std_logic_vector(63 downto 0) --control MAP: (63 downto 3) unused, 2 done, 1 busy, 0 ready.
);
end CORE_WRAPPER;

architecture structure of CORE_WRAPPER is

	component CU is --control unit
	port (
		clk: in std_logic;
		rst: in std_logic;
		
		start: in std_logic;
		reload: in std_logic;
		counter: in std_logic_vector(3 downto 0);
		core_input: in std_logic_vector(63 downto 0);
		mode_input: in std_logic;
		
		done: out std_logic;
		ready: out std_logic;
		busy: out std_logic;
		
		counter_rst: out std_logic;
		counter_en: out std_logic;
		
		core_rst: out std_logic;
		mode_output: out std_logic;
		
		register_output: out std_logic_vector(63 downto 0)
	);
	end component;
	
	component des_loop is --crypto core
	port(clk :  in std_logic;
		  rst : in std_logic;
		  mode : in std_logic; -- 0 encrypt, 1 decrypt
		  key_in : in std_logic_vector(63 downto 0);
		  blk_in : in std_logic_vector(63 downto 0);
		  blk_out : out std_logic_vector(63 downto 0));
	end component;
	
	--add counter IP component here
	COMPONENT c_counter_binary_0
	PORT (
		CLK : IN STD_LOGIC;
		CE : IN STD_LOGIC;
		SCLR : IN STD_LOGIC;
		Q : OUT STD_LOGIC_VECTOR(3 DOWNTO 0) 
	);
	END COMPONENT;
	--
	
	signal counter_reset,counter_enable,core_mode,core_reset : std_logic;
	signal counter_output: std_logic_vector(3 downto 0);
	signal core_output: std_logic_vector(63 downto 0);
	
	begin
	
	CORE_INST: des_loop port map(clk,core_reset,core_mode,key,input_word,core_output);
	
	COUNTER_INST: c_counter_binary_0 port map(clk,counter_enable,counter_reset,counter_output);
	
	CU_INST: CU 
	port map(
		clk => clk,
		rst => rst,
		start => control_word_in(0),
		reload => control_word_in(2),
		counter => counter_output,
		core_input => core_output,
		mode_input => control_word_in(1),
		done => control_word_out(2),
		ready => control_word_out(0),
		busy => control_word_out(1),
		counter_rst => counter_reset,
		counter_en => counter_enable,
		core_rst => core_reset,
		mode_output => core_mode,
		register_output => output_word
	);
	
	control_word_out(63 downto 3) <= (others => '0');

end structure;