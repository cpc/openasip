-------------------------------------------------------------------------------
-- Title      : Processor based on TTA MOVE architecture template
-- Project    : FlexDSP
-------------------------------------------------------------------------------
-- File       : moveproc_arch.vhdl
-- Author     : Jaakko Sertamo  <sertamo@vlad.cs.tut.fi>
-- Company    : TUT/IDCS
-- Created    : 2003-11-26
-- Last update: 2006-04-11
-- Platform   : 
-- Standard   : VHDL'87
-------------------------------------------------------------------------------
-- Description: architecture for processor with a core with single port data
-- memory/cache (structural) and with a core with dual-port data memory/cache
-- (structural_dp_dmem)
-------------------------------------------------------------------------------
-- Copyright (c) 2003 
-------------------------------------------------------------------------------
-- Revisions  :
-- Date        Version  Author  Description
-- 2003-11-26  1.0      sertamo	Created
-------------------------------------------------------------------------------

use work.toplevel_params.all;
use work.testbench_constants.all;

architecture structural of moveproc is

  
  component toplevel
    port (
      clk : in std_logic;
      rstx : in std_logic;
      busy : in std_logic;
      imem_en_x : out std_logic;
      imem_addr : out std_logic_vector(IMEMADDRWIDTH-1 downto 0);
      imem_data : in std_logic_vector(IMEMWIDTHINMAUS*IMEMMAUWIDTH-1 downto 0);
      pc_init : in std_logic_vector(IMEMADDRWIDTH-1 downto 0);
      fu_ldst_data_in : in std_logic_vector(fu_ldst_dataw-1 downto 0);
      fu_ldst_data_out : out std_logic_vector(fu_ldst_dataw-1 downto 0);
      fu_ldst_addr : out std_logic_vector(fu_ldst_addrw_plus_2-2-1 downto 0);
      fu_ldst_mem_en_x : out std_logic_vector(0 downto 0);
      fu_ldst_wr_en_x : out std_logic_vector(0 downto 0);
      fu_ldst_wr_mask_x : out std_logic_vector(fu_ldst_dataw-1 downto 0));   
  end component;

  component synch_sram
    generic (
      INITFILENAME : string;
      DATAW        : integer;
      ADDRW        : integer);
    port (
      clk      : in  std_logic;
      d        : in  std_logic_vector(DATAW-1 downto 0);
      addr     : in  std_logic_vector(ADDRW-1 downto 0);
      en_x     : in  std_logic;
      wr_x     : in  std_logic;
      bit_wr_x : in  std_logic_vector(DATAW-1 downto 0);
      q        : out std_logic_vector(DATAW-1 downto 0));
  end component;

  component synch_sram_3MAU
    generic (
      INITFILENAME : string;
      MAUW        : integer;
      ADDRW        : integer);
    port (
      clk      : in  std_logic;
      d        : in  std_logic_vector(3*MAUW-1 downto 0);
      addr     : in  std_logic_vector(ADDRW-1 downto 0);
      en_x     : in  std_logic;
      wr_x     : in  std_logic;
      bit_wr_x : in  std_logic_vector(3*MAUW-1 downto 0);
      q        : out std_logic_vector(3*MAUW-1 downto 0));
  end component;

  component mem_arbiter
    generic (
      PORTW     : integer;
      ADDRWIDTH : integer);
    port (
      d_1        : in  std_logic_vector(PORTW-1 downto 0);
      d_2        : in  std_logic_vector(PORTW-1 downto 0);
      d          : out std_logic_vector(PORTW-1 downto 0);
      addr_1     : in  std_logic_vector(ADDRWIDTH-1 downto 0);
      addr_2     : in  std_logic_vector(ADDRWIDTH-1 downto 0);
      addr       : out std_logic_vector(ADDRWIDTH-1 downto 0);
      en_1_x     : in  std_logic;
      en_2_x     : in  std_logic;
      en_x       : out std_logic;
      wr_1_x     : in  std_logic;
      wr_2_x     : in  std_logic;
      wr_x       : out std_logic;
      bit_wr_1_x : in  std_logic_vector(PORTW-1 downto 0);
      bit_wr_2_x : in  std_logic_vector(PORTW-1 downto 0);
      bit_wr_x   : out std_logic_vector(PORTW-1 downto 0);
      mem_busy   : out std_logic);
  end component;

  component imem_arbiter
  generic (
    PORTW     : integer := 32;
    ADDRWIDTH : integer := 11);

  port (
    d_2 : in  std_logic_vector(PORTW-1 downto 0);
    d   : out std_logic_vector(PORTW-1 downto 0);

    addr_1 : in  std_logic_vector(ADDRWIDTH-1 downto 0);
    addr_2 : in  std_logic_vector(ADDRWIDTH-1 downto 0);
    addr   : out std_logic_vector(ADDRWIDTH-1 downto 0);

    en_1_x : in  std_logic;
    en_2_x : in  std_logic;
    en_x   : out std_logic;

    wr_2_x : in  std_logic;
    wr_x   : out std_logic;

    bit_wr_2_x : in  std_logic_vector(PORTW-1 downto 0);
    bit_wr_x   : out std_logic_vector(PORTW-1 downto 0);

    mem_busy : out std_logic);
  end component;
  
  signal q_a        : std_logic_vector(DMEMDATAWIDTH-1 downto 0);  
  signal d_a        : std_logic_vector(DMEMDATAWIDTH-1 downto 0);
  signal addr_a     : std_logic_vector(DMEMADDRWIDTH-1 downto 0);
  signal en_a_x     : std_logic;
  signal wr_a_x     : std_logic;
  signal bit_wr_a_x : std_logic_vector(DMEMDATAWIDTH-1 downto 0);
  
  signal dmem_q        : std_logic_vector(DMEMDATAWIDTH-1 downto 0);
  signal dmem_d        : std_logic_vector(DMEMDATAWIDTH-1 downto 0);
  signal dmem_addr     : std_logic_vector(DMEMADDRWIDTH-1 downto 0);
  signal dmem_en_x     : std_logic;
  signal dmem_wr_x     : std_logic;
  signal dmem_bit_wr_x : std_logic_vector(DMEMDATAWIDTH-1 downto 0);

  signal imem_data    : std_logic_vector(IMEMWIDTHINMAUS*IMEMMAUWIDTH-1 downto 0);
  signal imem_addr : std_logic_vector(IMEMADDRWIDTH-1 downto 0);
  signal imem_en_x    : std_logic;

  signal imem_d_arb : std_logic_vector(IMEMWIDTHINMAUS*IMEMMAUWIDTH-1 downto 0);
  signal imem_addr_arb : std_logic_vector(IMEMADDRWIDTH-1 downto 0);
  signal imem_en_arb_x : std_logic;
  signal imem_wr_arb_x : std_logic;
  signal imem_bit_wr_arb_x : std_logic_vector(IMEMWIDTHINMAUS*IMEMMAUWIDTH-1 downto 0);

begin                                 --structural

  core : toplevel
    port map (
      clk              => clk,
      rstx             => rst_x,
      busy             => '0', 
      imem_en_x         => imem_en_x,
      imem_addr         => imem_addr,
      imem_data         => imem_data,
      pc_init          => pc_init,
      fu_ldst_data_in   => dmem_q,
      fu_ldst_data_out  => dmem_d,
      fu_ldst_addr      => dmem_addr,
      fu_ldst_mem_en_x(0)  => dmem_en_x,
      fu_ldst_wr_en_x(0)   => dmem_wr_x,
      fu_ldst_wr_mask_x => dmem_bit_wr_x);

  datamem : synch_sram
    generic map (
      -- pragma translate_off
      INITFILENAME => DMEM_INIT_FILE,
      -- pragma translate_on
      DATAW        => DMEMDATAWIDTH,
      ADDRW        => DMEMADDRWIDTH)
    port map (
      d        => d_a,
      addr     => addr_a,
      en_x     => en_a_x,
      wr_x     => wr_a_x,
      bit_wr_x => bit_wr_a_x,
      clk      => clk,
      q        => q_a);  

  dmem_q   <= q_a;
  data_out <= q_a;
  
  dmem_arbiter : mem_arbiter
    generic map (
      PORTW     => DMEMDATAWIDTH,
      ADDRWIDTH => DMEMADDRWIDTH)
    port map (
      d_1 => dmem_d,
      d_2 => dmem_ext_data,
      d   => d_a,

      addr_1 => dmem_addr,
      addr_2 => dmem_ext_addr,
      addr   => addr_a,

      en_1_x => dmem_en_x,
      en_2_x => dmem_ext_en_x,
      en_x   => en_a_x,

      wr_1_x => dmem_wr_x,
      wr_2_x => dmem_ext_wr_x,
      wr_x   => wr_a_x,

      bit_wr_1_x => dmem_bit_wr_x,
      bit_wr_2_x => dmem_ext_bit_wr_x,
      bit_wr_x   => bit_wr_a_x,
      mem_busy   => dmem_busy);

  instrmem_arbiter : imem_arbiter
    generic map (
      PORTW     => IMEMWIDTHINMAUS*IMEMMAUWIDTH,
      ADDRWIDTH => IMEMADDRWIDTH)
    port map (
      d_2       => imem_ext_data,
      d         => imem_d_arb,

      addr_1 => imem_addr,
      addr_2 => imem_ext_addr,
      addr   => imem_addr_arb,

      en_1_x => imem_en_x,
      en_2_x => imem_ext_en_x,
      en_x   => imem_en_arb_x,

      wr_2_x => imem_ext_wr_x,
      wr_x   => imem_wr_arb_x,

      bit_wr_2_x => imem_ext_bit_wr_x,
      bit_wr_x   => imem_bit_wr_arb_x,
      mem_busy   => imem_busy);

  instmem : synch_sram_3MAU
    generic map (
      -- pragma translate_off
      INITFILENAME => IMEM_INIT_FILE,
      -- pragma translate_on;
      MAUW  => IMEMMAUWIDTH,
      ADDRW => IMEMADDRWIDTH)
    port map (
      clk  => clk,
      d => imem_d_arb,
      addr => imem_addr_arb,      
      en_x => imem_en_arb_x,
      wr_x => imem_wr_arb_x,
      bit_wr_x => imem_bit_wr_arb_x,
      q => imem_data);

end structural;
