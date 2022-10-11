library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;

entity altera_jtag_uart_stdout_always_1 is
  generic (
    dataw : integer := 8);
  port (
    t1data   : in  std_logic_vector(dataw-1 downto 0);
    t1load   : in  std_logic;
    clk      : in  std_logic;
    rstx     : in  std_logic;
    glock    : in  std_logic;
    lock_req : out std_logic
    );
end altera_jtag_uart_stdout_always_1;


architecture rtl of altera_jtag_uart_stdout_always_1 is

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

  constant avalon_dataw : integer := 32;

  signal write_n_r   : std_logic;
  signal writedata_r : std_logic_vector(dataw-1 downto 0);
  signal writedata_w : std_logic_vector(avalon_dataw-1 downto 0);
  signal lock_req_r  : std_logic;
  signal can_write_r : std_logic;

  signal wait_w      : std_logic;
  signal can_write_w : std_logic;
  signal cs_w        : std_logic;

  signal irq_dummy   : std_logic;
  signal rd_dummy    : std_logic_vector(avalon_dataw-1 downto 0);
  signal avail_dummy : std_logic;
  
begin  -- rtl

  uart : jtag_uart
    port map (
      av_address     => '0',            -- addr 0 is for data register
      av_chipselect  => cs_w,
      av_read_n      => '1',            -- write only uart
      av_write_n     => write_n_r,
      av_writedata   => writedata_w,
      clk            => clk,
      rst_n          => rstx,
      av_irq         => irq_dummy,
      av_readdata    => rd_dummy,
      av_waitrequest => wait_w,
      dataavailable  => avail_dummy,
      readyfordata   => can_write_w
      );

  main : process (clk, rstx)
  begin  -- process main
    if rstx = '0' then                  -- asynchronous reset (active low)
      write_n_r   <= '1';
      writedata_r <= (others => '0');
      lock_req_r  <= '0';
      
    elsif clk'event and clk = '1' then  -- rising clock edge
      
      if lock_req_r = '1' then
        if write_n_r = '0' then
          -- case: waiting for write to succeed
          if wait_w = '0' then
            -- wait request is deasserted, write has happened
            lock_req_r <= '0';
            write_n_r  <= '1';
          else
            -- wait request is still active
            lock_req_r <= '1';
            write_n_r  <= '0';
          end if;
        else
          -- case: waiting for space in write fifo (write_n_r = '0')
          if can_write_w = '1' then
            -- space in fifo, assert write signal
            lock_req_r <= '1';
            write_n_r  <= '0';
          else
            -- still waiting
            lock_req_r <= '1';
            write_n_r  <= '1';
          end if;
        end if;
        
      elsif glock = '0' then
        -- default
        write_n_r <= '1';

        if t1load = '1' then
          -- can we write?
          if can_write_w = '1' then
            write_n_r   <= '0';
            writedata_r <= t1data;
            -- lock until write succeeds
            lock_req_r  <= '1';
          else
            write_n_r   <= '1';
            writedata_r <= t1data;
            -- lock until write can be performed
            lock_req_r  <= '1';
          end if;
        end if;
      end if;  -- lock_req_r
    end if;
  end process main;

  lock_req    <= lock_req_r;
  writedata_w <= EXT(writedata_r, writedata_w'length);
  cs_w        <= not(write_n_r);
  
end rtl;
