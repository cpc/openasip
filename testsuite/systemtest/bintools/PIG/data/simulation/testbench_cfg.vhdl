-------------------------------------------------------------------------------
-- Title      : Configuration for the testbench of TTA MOVE Processor
-- Project    : FlexDSP
-------------------------------------------------------------------------------
-- File       : testbench_cfg.vhdl
-- Author     : Jaakko Sertamo  <sertamo@vlad.cs.tut.fi>
-- Company    : TUT/IDCS
-- Created    : 2003-11-24
-- Last update: 2006-03-28
-- Platform   : 
-- Standard   : VHDL'87
-------------------------------------------------------------------------------
-- Description: Select correct architecture for moveproc
--              (single-port vs. dual-port data memory)
-------------------------------------------------------------------------------
-- Copyright (c) 2003 
-------------------------------------------------------------------------------
-- Revisions  :
-- Date        Version  Author  Description
-- 2003-11-24  1.0      sertamo	Created
-------------------------------------------------------------------------------

configuration testbench_cfg of testbench is
  for testbench
    for dut : moveproc
      use entity work.moveproc(structural);
      --use entity work.moveproc(structural_dp_dmem);
    end for;
  end for;
end testbench_cfg;


