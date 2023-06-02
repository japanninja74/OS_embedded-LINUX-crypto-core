library ieee;
use ieee.std_logic_1164.ALL;

entity CU is 
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
end CU;

architecture behaviour of CU is
	type CU_State is (IDLE,CONVERTING,FINISH);       -- STATES OF THE FSM
    signal current_State, next_State : CU_State;
	signal DONE_FLG,counter_en_buf: std_logic;
	
	begin
		STATE_MEMORY:
		process(CLK)
        begin
            if rising_edge(CLK) then
                if (rst = '1') then
                    current_State <= IDLE;
                else
                    case current_State is
                        when IDLE => 								-- RESET STATE
                            if (start = '1') then
						      current_State <= CONVERTING;
					        else
						      current_State <= IDLE;
					        end if;
					        done <= '0';
					        ready <= '1';
					        busy <= '0';
					        counter_en_buf <= '0';
					        counter_rst <= '1';
					        core_rst <= '1';

                        WHEN CONVERTING => 								-- CONVERSION ROUTINE	
                            if (DONE_FLG = '1') then
						      current_State <= FINISH;
					        else
						      current_State <= CONVERTING;
					        end if;
				             done <= '0';
				             ready <= '0';
			         		 busy <= '1';
				        	 counter_en_buf <= '1';
				        	 counter_rst <= '0';
				        	 core_rst <= '0';
					
				        WHEN FINISH => 								-- DONE 	
                            if (reload = '1') then
					           	current_State <= IDLE;
				           	else
				        		current_State <= FINISH;
				        	end if;
				        	done <= '1';
				        	ready <= '0';
				        	busy <= '0';
				        	counter_en_buf <= '0';
				        	counter_rst <= '1';
				        	core_rst <= '1';
                    END CASE; 
                end if;
            end if;
        end process STATE_MEMORY;
		
		DONE_LOGIC: 
		process(counter)
		begin
			if (counter = "1111") then --change if necessary to a different value
				DONE_FLG <= '1';
			else 
				DONE_FLG <= '0';
			end if;
		end process DONE_LOGIC;
		
		COUNTER_EN_DELAY: 
		process(clk)
		begin
		    if rising_edge(CLK) then 
                counter_en <= counter_en_buf;
            end if;
		end process;
        
        OUTPUT_PROCESS: 
        process(clk,current_State, start, reload, DONE_FLG, mode_input, core_input)
        begin
            if rising_edge(CLK) then
                if (rst = '1') then
                    register_output <= (others => '0');
                else 
                    if (current_State = IDLE) then
                        mode_output <= mode_input;
					    register_output <= (others => '0');
                    elsif (current_State = CONVERTING) then
                        register_output <= core_input;
                    end if;
                end if;
            end if;
        END process OUTPUT_PROCESS;
end behaviour;