
library ieee;
use ieee.std_logic_1164.all;

package altera_jtag_uart_streamin_streamout_opcodes is

  constant OPC_STREAM_IN         : std_logic_vector(1 downto 0) := "00";
  constant OPC_STREAM_IN_STATUS  : std_logic_vector(1 downto 0) := "01";
  constant OPC_STREAM_OUT        : std_logic_vector(1 downto 0) := "10";
  constant OPC_STREAM_OUT_STATUS : std_logic_vector(1 downto 0) := "11";

  constant EMPTY_BUFFER   : std_logic_vector(1 downto 0) := "00";
  constant DATA_IN_BUFFER : std_logic_vector(1 downto 0) := "01";
  constant FULL_BUFFER    : std_logic_vector(1 downto 0) := "10";
  
end altera_jtag_uart_streamin_streamout_opcodes;

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use work.altera_jtag_uart_streamin_streamout_opcodes.all;

entity altera_jtag_uart_streamin_streamout_always_1 is
  generic (
    busw  : integer := 32;
    dataw : integer := 8);
  port (
    t1data   : in  std_logic_vector(dataw-1 downto 0);
    t1load   : in  std_logic;
    t1opcode : in  std_logic_vector(1 downto 0);
    r1data   : out std_logic_vector(busw-1 downto 0);
    clk      : in  std_logic;
    rstx     : in  std_logic;
    glock    : in  std_logic;
    lock_req : out std_logic
    );
end altera_jtag_uart_streamin_streamout_always_1;


architecture rtl of altera_jtag_uart_streamin_streamout_always_1 is

  component jtag_uart is
    port (
      -- inputs:
      signal av_address    : in std_logic;
      signal av_chipselect : in std_logic;
      signal av_read_n     : in std_logic;
      signal av_write_n    : in std_logic;
      signal av_writedata  : in std_logic_vector (31 downto 0);
      signal clk           : in std_logic;
      signal rst_n         : in std_logic;

      -- outputs:
      signal av_irq         : out std_logic;
      signal av_readdata    : out std_logic_vector (31 downto 0);
      signal av_waitrequest : out std_logic;
      signal dataavailable  : out std_logic;
      signal readyfordata   : out std_logic
      );
  end component;

  -- Index of read data valid bit in jtag uart data register
  constant RVALID_BIT   : integer := 15;
  constant avalon_dataw : integer := 32;

  type lock_state_vector is (
    nop,                                --  No operation
    read_in_progress,                   --  Read is on, waiting for data in
    read_wait_for_data,                 --  Read is on, but no data in fifo
    write_in_progress,                  --  Write is on, waiting to succeed
    write_wait_for_space                --  Write is on, but no space in fifo
    );

  signal lock_state_r : lock_state_vector;

  signal r1data_r    : std_logic_vector(dataw-1 downto 0);
  signal write_n_r   : std_logic;
  signal read_n_r    : std_logic;
  signal writedata_r : std_logic_vector(dataw-1 downto 0);
  signal lock_req_r  : std_logic;

  signal readdata_w  : std_logic_vector(avalon_dataw-1 downto 0);
  signal writedata_w : std_logic_vector(avalon_dataw-1 downto 0);
  signal can_write_w : std_logic;
  signal wait_w      : std_logic;
  signal can_read_w  : std_logic;
  signal cs_w        : std_logic;

  signal irq_dummy : std_logic;
  
begin  -- rtl

  assert dataw = 8 report "Generic dataw must be 8" severity failure;

  uart : jtag_uart
    port map (
      av_address     => '0',            -- addr 0 is for data register
      av_chipselect  => cs_w,
      av_read_n      => read_n_r,
      av_write_n     => write_n_r,
      av_writedata   => writedata_w,
      clk            => clk,
      rst_n          => rstx,
      av_irq         => irq_dummy,
      av_readdata    => readdata_w,
      av_waitrequest => wait_w,
      dataavailable  => can_read_w,
      readyfordata   => can_write_w
      );

  main : process (clk, rstx)
  begin  -- process main
    if rstx = '0' then                  -- asynchronous reset (active low)
      r1data_r     <= (others => '0');
      write_n_r    <= '1';
      read_n_r     <= '1';
      writedata_r  <= (others => '0');
      lock_req_r   <= '0';
      lock_state_r <= nop;
    elsif clk'event and clk = '1' then  -- rising clock edge
      
      if lock_req_r = '1' then
        case lock_state_r is
          
          when nop =>
            assert false report
              "No operation eventhough lock request is active!"
              severity error;
            
          when read_in_progress =>
            if wait_w = '0' and readdata_w(RVALID_BIT) = '1' then
              r1data_r     <= readdata_w(dataw-1 downto 0);
              lock_req_r   <= '0';
              read_n_r     <= '1';
              lock_state_r <= nop;
            end if;

          when read_wait_for_data =>
            if can_read_w = '1' then
              lock_req_r   <= '1';
              read_n_r     <= '0';
              lock_state_r <= read_in_progress;
            end if;

          when write_in_progress =>
            if wait_w = '0' then
              lock_req_r   <= '0';
              write_n_r    <= '1';
              lock_state_r <= nop;
            end if;

          when write_wait_for_space =>
            if can_write_w = '1' then
              lock_req_r   <= '1';
              write_n_r    <= '0';
              lock_state_r <= write_in_progress;
            end if;
            
          when others =>
            assert false report "Unknown lock state!" severity failure;
            lock_req_r <= '0';
        end case;
        
      elsif glock = '0' then

        -- default
        read_n_r  <= '1';
        write_n_r <= '1';

        if t1load = '1' then
          assert lock_state_r = nop report
            "New operation triggered while previous operation is still active"
            severity error;
          
          case t1opcode is
            when OPC_STREAM_IN =>
              if can_read_w = '1' then
                read_n_r     <= '0';
                lock_req_r   <= '1';
                lock_state_r <= read_in_progress;
              else
                read_n_r     <= '1';
                lock_req_r   <= '1';
                lock_state_r <= read_wait_for_data;
              end if;

            when OPC_STREAM_IN_STATUS =>
              if can_read_w = '1' then
                -- No distinction between a full buffer and a buffer with some
                -- data in it. Information is available in jtag uart data
                -- register (indices 31:16) but it would take multiple cycles
                -- to determine it. Just return 'data in buffer' to speed
                -- things up.
                r1data_r <= EXT(DATA_IN_BUFFER, r1data_r'length);
              else
                r1data_r <= EXT(EMPTY_BUFFER, r1data_r'length);
              end if;

            when OPC_STREAM_OUT =>
              if can_write_w = '1' then
                write_n_r    <= '0';
                writedata_r  <= t1data;
                lock_req_r   <= '1';
                lock_state_r <= write_in_progress;
              else
                write_n_r    <= '1';
                writedata_r  <= t1data;
                lock_req_r   <= '1';
                lock_state_r <= write_wait_for_space;
              end if;

            when OPC_STREAM_OUT_STATUS =>
              if can_write_w = '1' then
                -- Notice: there is no distinction between an empty buffer and
                -- a buffer with some data in it. The information could be read
                -- from the jtag uart control register (indices 31:16) but it
                -- would take multiple cycles. Just return buffer empty to
                -- speed things up
                r1data_r <= EXT(EMPTY_BUFFER, r1data_r'length);
              else
                r1data_r <= EXT(FULL_BUFFER, r1data_r'length);
              end if;
              
            when others => null;
          end case;
          
        end if;
      end if;  -- lock_req_r
    end if;
  end process main;

  lock_req    <= lock_req_r;
  r1data      <= EXT(r1data_r, r1data'length);
  writedata_w <= EXT(writedata_r, writedata_w'length);
  cs_w        <= not(write_n_r and read_n_r);
  
end rtl;
