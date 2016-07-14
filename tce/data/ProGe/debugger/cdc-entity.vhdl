-------------------------------------------------------------------------------
-- Title      : cdc entity declaration
-- Project    : tta
-------------------------------------------------------------------------------
-- File       : cdc-entity.vhdl
-- Author     : Tommi Zetterman  <tommi.zetterman@nokia.com>
-- Company    : Nokia Research Center
-- Created    : 2013-03-14
-- Last update: 2013-10-24
-- Platform   :
-- Standard   : VHDL'87
-------------------------------------------------------------------------------
-- Description: Clock domain crossing fpga_interface <--> debugger
-------------------------------------------------------------------------------
-- Copyright (c) 2013 Nokia Research Center
-------------------------------------------------------------------------------
-- Revisions  :
-- Date        Version  Author  Description
-- 2013-03-14  1.0      zetterma	Created
-------------------------------------------------------------------------------

library ieee;
use ieee.std_logic_1164.all;

entity cdc is
  generic (
    data_width_g : integer := 32;
    addr_width_g : integer := 8
    );
  port(
    nreset    : in std_logic;
    -- fpga if
    clk_fpga  : in std_logic;
    wen_fpga  : in std_logic;
    ren_fpga  : in std_logic;
    addr_fpga : in std_logic_vector(addr_width_g-1 downto 0);
    din_fpga  : in std_logic_vector(data_width_g-1 downto 0);
    dout_fpga : out std_logic_vector(data_width_g-1 downto 0);
    dv_fpga   : out std_logic;
    -- debugger if
    clk_dbg   : in std_logic;
    we_dbg    : out std_logic;
    re_dbg    : out std_logic;
    addr_dbg  : out std_logic_vector(addr_width_g-1 downto 0);
    din_dbg   : out std_logic_vector(data_width_g-1 downto 0);
    dout_dbg  : in std_logic_vector(data_width_g-1 downto 0);
    -- status
    busy      : out std_logic
    );
end entity;
