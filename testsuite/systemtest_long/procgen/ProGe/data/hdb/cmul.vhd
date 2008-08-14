-- bug fix 21.11.2005 : -added scaling of 2 before addition, and at the path of
--                       multiplication of 1
-- new_implmentation 18.7.2005 : -added one pipeline stage between multipliers
--                               -and adders.
-- bug fix 18.7.2005 : -moved register from output of multiplier to output
--                     -of unit, to correspond opther units.
-- bug fix 8.8.2005 : - removed one extra register. now latency is always 3,
--                      not 4 when starting multiplication. 
-- bug fix 16.9.04 : - constant value "dout8" corrected
--                   - bus tap dimensions after multiplication corrected

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;

entity cmul is
  generic (
    DataW   :     integer := 32;
    HalfW   :     integer := 16);
  port(
    clk     : in  std_logic;
    --enable  : IN     std_logic;
    operand : in  std_logic_vector (DataW-1 downto 0);
    rstx    : in  std_logic;
    trigger : in  std_logic_vector (DataW-1 downto 0);
    result  : out std_logic_vector (DataW-1 downto 0)
    );

-- Declarations

end cmul;

architecture struct of cmul is

  -- Architecture declarations

  -- Internal signal declarations
  signal dout  : std_logic_vector(HalfW-1 downto 0);
  signal dout1 : std_logic_vector(HalfW-1 downto 0);
  signal dout2 : std_logic_vector(HalfW-1 downto 0);
  signal dout3 : std_logic_vector(HalfW-1 downto 0);
  signal dout4 : std_logic_vector(HalfW-1 downto 0);
  signal dout5 : std_logic_vector(HalfW-1 downto 0);
  signal dout6 : std_logic_vector(DataW-1 downto 0);
  signal dout7 : std_logic_vector(DataW-1 downto 0);
  signal dout8 : std_logic_vector(DataW-1 downto 0);
  signal eq    : std_logic;
  signal i1    : std_logic_vector(HalfW-1 downto 0);
  signal i2    : std_logic_vector(HalfW-1 downto 0);
  signal prod  : std_logic_vector(DataW-1 downto 0);
  signal prod1 : std_logic_vector(DataW-1 downto 0);
  signal prod2 : std_logic_vector(DataW-1 downto 0);
  signal prod3 : std_logic_vector(DataW-1 downto 0);
  signal r1    : std_logic_vector(HalfW-1 downto 0);
  signal r2    : std_logic_vector(HalfW-1 downto 0);

  -- ModuleWare signal declarations(v1.4) for instance 'I15' of 'adff'
  --SIGNAL mw_I15reg_cval : std_logic_vector(DataW-1 DOWNTO 0);

  -- ModuleWare signal declarations(v1.4) for instance 'I7' of 'split'
  signal mw_I7temp_din : std_logic_vector(DataW-1 downto 0);

  -- ModuleWare signal declarations(v1.4) for instance 'I8' of 'split'
  signal mw_I8temp_din : std_logic_vector(DataW-1 downto 0);


begin

  -- ModuleWare code(v1.4) for instance 'I14' of 'add'
  I14combo_proc        : process (dout2, dout3)
    variable temp_din0 : std_logic_vector(HalfW downto 0);
    variable temp_din1 : std_logic_vector(HalfW downto 0);
    variable sum       : signed(HalfW downto 0);
    variable carry     : std_logic;
  begin
    temp_din0 := dout2(HalfW-1) & dout2;
    temp_din1 := dout3(HalfW-1) & dout3;
    carry     := '0';
    sum       := signed(temp_din0) + signed(temp_din1) + carry;
    dout5 <= conv_std_logic_vector(sum(HalfW-1 downto 0), HalfW);
  end process I14combo_proc;

  -- ModuleWare code(v1.4) for instance 'I15' of 'adff'
  result <= dout7;

  -- ModuleWare code(v1.4) for instance 'I16' of 'cmp'
  I16combo_proc  : process (trigger, dout8)
    variable leq : std_logic;

  begin
    leq   := '0';
    if (signed(trigger) = signed(dout8)) then
      leq := '1';
    end if;
    eq <= not(leq);
  end process I16combo_proc;

  -- ModuleWare code(v1.4) for instance 'I17' of 'constval'
  dout8 <= "01111111111111110000000000000000";

  -- ModuleWare code(v1.4) for instance 'I19' of 'merge'
  dout6 <= dout4 & dout5;

  -- ModuleWare code(v1.4) for instance 'I0' of 'mult'
  I0combo_proc : process (r1, r2)
    --VARIABLE temp_in_prod : signed(DataW-1 DOWNTO 0);
  begin
    --temp_in_prod := (signed(r1) * signed(r2));
    --prod <= std_logic_vector(temp_in_prod);
    prod <= sxt(conv_std_logic_vector(signed(r1) * signed(r2), prod'length)(DataW-1 downto 1), prod'length);
    --prod <= sxt(conv_std_logic_vector(signed(r1) * signed(r2),prod'length-1),prod'length);
  end process I0combo_proc;

  -- ModuleWare code(v1.4) for instance 'I1' of 'mult'
  I1combo_proc : process (i1, i2)
    --VARIABLE temp_in_prod : signed(DataW-1 DOWNTO 0);
  begin
    --temp_in_prod := (signed(i1) * signed(i2));
    --prod1 <= std_logic_vector(temp_in_prod);
    --prod1 <= sxt(conv_std_logic_vector(signed(i1) * signed(i2),prod1'length-1),prod1'length);
    prod1 <= sxt(conv_std_logic_vector(signed(i1) * signed(i2), prod1'length)(DataW-1 downto 1), prod1'length);
  end process I1combo_proc;

  -- ModuleWare code(v1.4) for instance 'I2' of 'mult'
  I2combo_proc : process (r1, i2)
    --VARIABLE temp_in_prod : signed(DataW-1 DOWNTO 0);
  begin
    --temp_in_prod := (signed(r1) * signed(i2));
    --prod2 <= std_logic_vector(temp_in_prod);
    --prod2 <= sxt(conv_std_logic_vector(signed(r1) * signed(i2),prod2'length-1),prod2'length);
    prod2 <= sxt(conv_std_logic_vector(signed(r1) * signed(i2), prod2'length)(DataW-1 downto 1), prod2'length);
  end process I2combo_proc;

  -- ModuleWare code(v1.4) for instance 'I3' of 'mult'
  I3combo_proc : process (i1, r2)
    --VARIABLE temp_in_prod : signed(DataW-1 DOWNTO 0);
  begin
    --temp_in_prod := (signed(i1) * signed(r2));
    --prod3 <= std_logic_vector(temp_in_prod);
    --prod3 <= sxt(conv_std_logic_vector(signed(i1) * signed(r2),prod3'length-1),prod3'length);
    prod3 <= sxt(conv_std_logic_vector(signed(i1) * signed(r2), prod3'length)(DataW-1 downto 1), prod3'length);
  end process I3combo_proc;

  -- ModuleWare code(v1.4) for instance 'I18' of 'mux'
  I18combo_proc : process(operand, dout6, eq)
  begin
    case eq is
      -- if the operand is passed through straighlty
      -- operand must be divided by 2(extend sign and shift one to left)      
      when '0'|'L' => dout7 <=
                        operand(dataw-1 downto dataw-1) &
                        operand(dataw-1 downto dataw/2+1) &
                        operand(dataw/2-1 downto dataw/2-1)&
                        operand(dataw/2-1 downto 1);
      when '1'|'H' => dout7 <= dout6;
      when others  => dout7 <= (others => 'X');
    end case;
  end process I18combo_proc;

  -- ModuleWare code(v1.4) for instance 'I7' of 'split'
  mw_I7temp_din <= trigger;
  I7combo_proc     : process (mw_I7temp_din)
    variable itemp : std_logic_vector(DataW-1 downto 0);
  begin
    itemp := mw_I7temp_din(DataW-1 downto 0);
    i1          <= itemp(HalfW-1 downto 0);
    r1          <= itemp(DataW-1 downto HalfW);
  end process I7combo_proc;

  -- ModuleWare code(v1.4) for instance 'I8' of 'split'
  mw_I8temp_din <= operand;
  I8combo_proc     : process (mw_I8temp_din)
    variable itemp : std_logic_vector(DataW-1 downto 0);
  begin
    itemp := mw_I8temp_din(DataW-1 downto 0);
    i2          <= itemp(HalfW-1 downto 0);
    r2          <= itemp(DataW-1 downto HalfW);
  end process I8combo_proc;

  -- ModuleWare code(v1.4) for instance 'I13' of 'sub'
  I13combo_proc        : process (dout, dout1)
    variable temp_din0 : std_logic_vector(HalfW downto 0);
    variable temp_din1 : std_logic_vector(HalfW downto 0);
    variable diff      : signed(HalfW downto 0);
    variable borrow    : std_logic;
  begin
    temp_din0 := dout(HalfW-1) & dout;
    temp_din1 := dout1(HalfW-1) & dout1;
    borrow    := '0';
    diff      := signed(temp_din0) - signed(temp_din1) - borrow;
    dout4 <= conv_std_logic_vector(diff(HalfW-1 downto 0), HalfW);
  end process I13combo_proc;

  -- ModuleWare code(v1.4) for instance 'I9' of 'tap'
  dout <= prod(DataW-2 downto HalfW-1);

  -- ModuleWare code(v1.4) for instance 'I10' of 'tap'
  dout1 <= prod1(DataW-2 downto HalfW-1);

  -- ModuleWare code(v1.4) for instance 'I11' of 'tap'
  dout2 <= prod2(DataW-2 downto HalfW-1);

  -- ModuleWare code(v1.4) for instance 'I12' of 'tap'
  dout3 <= prod3(DataW-2 downto HalfW-1);

  -- Instance port mappings.

end struct;

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;

entity cmul_pipeline_2 is
  generic (
    DataW   :     integer := 32;
    HalfW   :     integer := 16);
  port(
    clk     : in  std_logic;
    enable  : in  std_logic;
    operand : in  std_logic_vector (DataW-1 downto 0);
    rstx    : in  std_logic;
    trigger : in  std_logic_vector (DataW-1 downto 0);
    result  : out std_logic_vector (DataW-1 downto 0)
    );

-- Declarations

end cmul_pipeline_2;

architecture struct of cmul_pipeline_2 is

  -- Architecture declarations

  -- Internal signal declarations
  signal dout        : std_logic_vector(HalfW-1 downto 0);
  signal dout1       : std_logic_vector(HalfW-1 downto 0);
  signal dout2       : std_logic_vector(HalfW-1 downto 0);
  signal dout3       : std_logic_vector(HalfW-1 downto 0);
  signal dout4       : std_logic_vector(HalfW-1 downto 0);
  signal dout5       : std_logic_vector(HalfW-1 downto 0);
  signal dout6       : std_logic_vector(DataW-1 downto 0);
  signal dout7       : std_logic_vector(DataW-1 downto 0);
  signal dout8       : std_logic_vector(DataW-1 downto 0);
  signal eq          : std_logic;
  signal i1          : std_logic_vector(HalfW-1 downto 0);
  signal i2          : std_logic_vector(HalfW-1 downto 0);
  signal prod        : std_logic_vector(DataW-1 downto 0);
  signal prod1       : std_logic_vector(DataW-1 downto 0);
  signal prod2       : std_logic_vector(DataW-1 downto 0);
  signal prod3       : std_logic_vector(DataW-1 downto 0);
  signal r1          : std_logic_vector(HalfW-1 downto 0);
  signal r2          : std_logic_vector(HalfW-1 downto 0);
  signal operand_reg : std_logic_vector(DataW-1 downto 0);
  signal temp        : std_logic_vector(DataW-1 downto 0);
  signal temp1       : std_logic_vector(DataW-1 downto 0);
  signal temp2       : std_logic_vector(DataW-1 downto 0);
  signal temp3       : std_logic_vector(DataW-1 downto 0);


  -- ModuleWare signal declarations(v1.4) for instance 'I15' of 'adff'
  --SIGNAL mw_I15reg_cval : std_logic_vector(DataW-1 DOWNTO 0);

  -- ModuleWare signal declarations(v1.4) for instance 'I7' of 'split'
  signal mw_I7temp_din : std_logic_vector(DataW-1 downto 0);

  -- ModuleWare signal declarations(v1.4) for instance 'I8' of 'split'
  signal mw_I8temp_din : std_logic_vector(DataW-1 downto 0);


begin

  -- ModuleWare code(v1.4) for instance 'I14' of 'add'
  I14combo_proc        : process (dout2, dout3)
    variable temp_din0 : std_logic_vector(HalfW downto 0);
    variable temp_din1 : std_logic_vector(HalfW downto 0);
    variable sum       : signed(HalfW downto 0);
    variable carry     : std_logic;
  begin
    temp_din0 := dout2(HalfW-1) & dout2;
    temp_din1 := dout3(HalfW-1) & dout3;
    carry     := '0';
    sum       := signed(temp_din0) + signed(temp_din1) + carry;
    dout5 <= conv_std_logic_vector(sum(HalfW-1 downto 0), HalfW);
  end process I14combo_proc;

  -- ModuleWare code(v1.4) for instance 'I15' of 'adff'
  result <= dout7;

  -- ModuleWare code(v1.4) for instance 'I16' of 'cmp'
  --I16combo_proc : PROCESS (trigger, dout8)
  --VARIABLE leq : std_logic;

  --BEGIN
  --   leq := '0';
  --   IF (signed(trigger) = signed(dout8)) THEN
  --       leq := '1';
  --   END IF;
  --   eq <= NOT(leq);
  --END PROCESS I16combo_proc;

  -- ModuleWare code(v1.4) for instance 'I17' of 'constval'
  dout8 <= "01111111111111110000000000000000";

  -- ModuleWare code(v1.4) for instance 'I19' of 'merge'
  dout6 <= dout4 & dout5;

  -- ModuleWare code(v1.4) for instance 'I0' of 'mult'
  I0combo_proc : process (r1, r2)
    --VARIABLE temp_in_prod : std_logic_vector(DataW-1 DOWNTO 0);
  begin
    --temp_in_prod := conv_std_logic_vector(signed(r1) * signed(r2),temp_in_prod'length);
    prod <= sxt(conv_std_logic_vector(signed(r1) * signed(r2), prod'length)(DataW-1 downto 1), prod'length);
  end process I0combo_proc;

  -- ModuleWare code(v1.4) for instance 'I1' of 'mult'
  I1combo_proc : process (i1, i2)
    --VARIABLE temp_in_prod : std_logic_vector(DataW-1 DOWNTO 0);
  begin
    --temp_in_prod := conv_std_logic_vector(signed(i1) * signed(i2),temp_in_prod'length);
    --prod1 <= sxt(temp_in_prod(DataW-1 downto 0),prod1'length);
    prod1 <= sxt(conv_std_logic_vector(signed(i1) * signed(i2), prod1'length)(DataW-1 downto 1), prod1'length);
  end process I1combo_proc;

  -- ModuleWare code(v1.4) for instance 'I2' of 'mult'
  I2combo_proc : process (r1, i2)
    --VARIABLE temp_in_prod : signed(DataW-1 DOWNTO 0);
  begin
    --temp_in_prod := (signed(r1) * signed(i2));
    --prod2 <= std_logic_vector(temp_in_prod);
    --prod2 <= sxt(conv_std_logic_vector(signed(r1) * signed(i2),prod2'length-1),prod2'length);
    prod2 <= sxt(conv_std_logic_vector(signed(r1) * signed(i2), prod2'length)(DataW-1 downto 1), prod2'length);
  end process I2combo_proc;

  -- ModuleWare code(v1.4) for instance 'I3' of 'mult'
  I3combo_proc : process (i1, r2)
    --VARIABLE temp_in_prod : signed(DataW-1 DOWNTO 0);
  begin
    --temp_in_prod := (signed(i1) * signed(r2));
    --prod3 <= std_logic_vector(temp_in_prod);
    --prod3 <= sxt(conv_std_logic_vector(signed(i1) * signed(r2),prod3'length-1),prod3'length);
    prod3 <= sxt(conv_std_logic_vector(signed(i1) * signed(r2), prod3'length)(DataW-1 downto 1), prod3'length);
  end process I3combo_proc;

  -- ModuleWare code(v1.4) for instance 'I18' of 'mux'
  I18combo_proc : process(operand_reg, dout6, eq)
  begin
    case eq is
      -- if the operand is passed through straighlty
      -- real part and imaginary part of
      -- operand must be divided by 2(extend sign and shift one to left)
      when '0'|'L' =>
        dout7 <= operand_reg(dataw-1 downto dataw-1) &
                 operand_reg(dataw-1 downto dataw/2+1) &
                 operand_reg(dataw/2-1 downto dataw/2-1) &
                 operand_reg(dataw/2-1 downto 1);
      when '1'|'H' => dout7 <= dout6;
      when others  => dout7 <= (others => 'X');
    end case;
  end process I18combo_proc;

  -- ModuleWare code(v1.4) for instance 'I7' of 'split'
  mw_I7temp_din <= trigger;
  I7combo_proc     : process (mw_I7temp_din)
    variable itemp : std_logic_vector(DataW-1 downto 0);
  begin
    itemp := mw_I7temp_din(DataW-1 downto 0);
    i1          <= itemp(HalfW-1 downto 0);
    r1          <= itemp(DataW-1 downto HalfW);
  end process I7combo_proc;

  -- ModuleWare code(v1.4) for instance 'I8' of 'split'
  mw_I8temp_din <= operand;
  I8combo_proc     : process (mw_I8temp_din)
    variable itemp : std_logic_vector(DataW-1 downto 0);
  begin
    itemp := mw_I8temp_din(DataW-1 downto 0);
    i2          <= itemp(HalfW-1 downto 0);
    r2          <= itemp(DataW-1 downto HalfW);
  end process I8combo_proc;

  -- ModuleWare code(v1.4) for instance 'I13' of 'sub'
  I13combo_proc        : process (dout, dout1)
    variable temp_din0 : std_logic_vector(HalfW downto 0);
    variable temp_din1 : std_logic_vector(HalfW downto 0);
    variable diff      : signed(HalfW downto 0);
    variable borrow    : std_logic;
  begin
    temp_din0 := dout(HalfW-1) & dout;
    temp_din1 := dout1(HalfW-1) & dout1;
    borrow    := '0';
    diff      := signed(temp_din0) - signed(temp_din1) - borrow;
    dout4 <= conv_std_logic_vector(diff(HalfW-1 downto 0), HalfW);
  end process I13combo_proc;

  pipeline_stage : process (clk, rstx)
  begin  -- process pipeline_stage
    if rstx = '0' then                  -- asynchronous reset (active low)
      dout          <= (others => '0');
      dout1         <= (others => '0');
      dout2         <= (others => '0');
      dout3         <= (others => '0');
      operand_reg   <= (others => '0');
      eq            <= '0';
    elsif clk'event and clk = '1' then  -- rising clock edge
      if (enable = '1' or enable = 'H') then
        dout        <= prod(DataW-2 downto HalfW-1);
        dout1       <= prod1(DataW-2 downto HalfW-1);
        dout2       <= prod2(DataW-2 downto HalfW-1);
        dout3       <= prod3(DataW-2 downto HalfW-1);
        operand_reg <= operand;
        if (signed(trigger) = signed(dout8)) then
          eq        <= '0';
        else
          eq        <= '1';
        end if;
      end if;
    end if;
  end process pipeline_stage;
  -- ModuleWare code(v1.4) for instance 'I9' of 'tap'
  --dout <= prod(DataW-2 DOWNTO HalfW-1);

  -- ModuleWare code(v1.4) for instance 'I10' of 'tap'
  --dout1 <= prod1(DataW-2 DOWNTO HalfW-1);

  -- ModuleWare code(v1.4) for instance 'I11' of 'tap'
  --dout2 <= prod2(DataW-2 DOWNTO HalfW-1);

  -- ModuleWare code(v1.4) for instance 'I12' of 'tap'
  --dout3 <= prod3(DataW-2 DOWNTO HalfW-1);

  -- Instance port mappings.

end struct;
-------------------------------------------------------------------------------
-- 3 clock cycles
-------------------------------------------------------------------------------
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;

entity fu_cmul_always_3 is

  generic (
    DataW : integer := 32;
    HalfW : integer := 16);

  port (
    clk    : in  std_logic;
    rstx   : in  std_logic;
    t1data : in  std_logic_vector(DataW-1 downto 0);
    o1data : in  std_logic_vector(DataW-1 downto 0);
    t1load : in  std_logic;
    o1load : in  std_logic;
    r1data : out std_logic_vector(DataW-1 downto 0);
    glock  : in  std_logic);

end fu_cmul_always_3;

architecture rtl of fu_cmul_always_3 is

  component cmul_pipeline_2

    generic (
      DataW : integer;
      HalfW : integer);

    port (
      clk     : in  std_logic;
      rstx    : in  std_logic;
      enable  : in  std_logic;
      trigger : in  std_logic_vector (DataW-1 downto 0);
      operand : in  std_logic_vector (DataW-1 downto 0);
      result  : out std_logic_vector (DataW-1 downto 0));

  end component;

  signal t1reg         : std_logic_vector (DataW-1 downto 0);
  signal o1reg         : std_logic_vector (DataW-1 downto 0);
  signal r1reg         : std_logic_vector (DataW-1 downto 0);
  signal result_en_reg : std_logic_vector(1 downto 0);
  signal control       : std_logic_vector(1 downto 0);
  signal r1            : std_logic_vector(DataW-1 downto 0);
begin  -- rtl

  fu_core : cmul_pipeline_2
    generic map (
      DataW   => DataW,
      HalfW   => HalfW)
    port map (
      clk     => clk,
      rstx    => rstx,
      enable  => result_en_reg(0),
      operand => o1reg,
      trigger => t1reg,
      result  => r1);

  control <= o1load&t1load;

  regs : process (clk, rstx)
  begin  -- process regs
    if rstx = '0' then                  -- asynchronous reset (active low)
      t1reg         <= (others => '0');
      o1reg         <= (others => '0');
      result_en_reg <= (others => '0');
      r1reg         <= (others => '0');

    elsif clk'event and clk = '1' then  -- rising clock edge
      if (glock = '0') then

        case control is
          when "11"   =>
            t1reg        <= t1data;
            o1reg        <= o1data;
          when "10"   =>
            o1reg        <= o1data;
          when "01"   =>
            t1reg        <= t1data;
          when others => null;
        end case;
        -- update result only when a new operation was triggered
        --result_en_reg <= t1load;
        -- update result eneable register when t1load is active
        -- add one register for pipeline register control and
        -- second register for result control
        result_en_reg(1) <= result_en_reg(0);
        result_en_reg(0) <= t1load;

        if result_en_reg(1) = '1' then
          r1reg <= r1;
        end if;
      end if;

    end if;
  end process regs;

  r1data <= r1reg;
  --r1data <= r1;

end rtl;

-------------------------------------------------------------------------------
-- 2 clock cycles
-------------------------------------------------------------------------------

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;

entity fu_cmul_always_2 is

  generic (
    DataW : integer := 32;
    HalfW : integer := 16);

  port (
    clk    : in  std_logic;
    rstx   : in  std_logic;
    t1data : in  std_logic_vector(DataW-1 downto 0);
    o1data : in  std_logic_vector(DataW-1 downto 0);
    t1load : in  std_logic;
    o1load : in  std_logic;
    r1data : out std_logic_vector(DataW-1 downto 0);
    glock  : in  std_logic);

end fu_cmul_always_2;

architecture rtl of fu_cmul_always_2 is

  component cmul

    generic (
      DataW : integer;
      HalfW : integer);

    port (
      clk     : in  std_logic;
      rstx    : in  std_logic;
      --enable   : IN     std_logic;
      trigger : in  std_logic_vector (DataW-1 downto 0);
      operand : in  std_logic_vector (DataW-1 downto 0);
      result  : out std_logic_vector (DataW-1 downto 0));

  end component;

  signal t1reg         : std_logic_vector (DataW-1 downto 0);
  signal o1reg         : std_logic_vector (DataW-1 downto 0);
  signal r1reg         : std_logic_vector (DataW-1 downto 0);
  signal result_en_reg : std_logic;
  signal control       : std_logic_vector(1 downto 0);
  signal r1            : std_logic_vector(DataW-1 downto 0);
begin  -- rtl

  fu_core : cmul
    generic map (
      DataW   => DataW,
      HalfW   => HalfW)
    port map (
      clk     => clk,
      rstx    => rstx,
      --enable   => result_en_reg(1),
      operand => o1reg,
      trigger => t1reg,
      result  => r1);

  control <= o1load&t1load;

  regs : process (clk, rstx)
  begin  -- process regs
    if rstx = '0' then                  -- asynchronous reset (active low)
      t1reg <= (others => '0');
      o1reg <= (others => '0');
      r1reg <= (others => '0');

      result_en_reg <= '0';
    elsif clk'event and clk = '1' then  -- rising clock edge
      if (glock = '0') then

        case control is
          when "11"   =>
            t1reg <= t1data;
            o1reg <= o1data;
          when "10"   =>
            o1reg <= o1data;
          when "01"   =>
            t1reg <= t1data;
          when others => null;
        end case;

        -- update result only when a new operation was triggered
        result_en_reg <= t1load;

        if result_en_reg = '1' then
          r1reg <= r1;
        end if;
      end if;
    end if;
  end process regs;

  r1data <= r1reg;
  --r1data <= r1;

end rtl;
