-- this file provides the first example
-- that should be synthesizable after the
-- first milestone of this project
----
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

entity fulladd is
   port (Cin, x, y : in  std_logic;
         s, Cout   : out std_logic);
end fulladd;

architecture beh of fulladd is
begin
   add : process is
   begin
      s <= x xor y xor Cin;
      wait on x, y, Cin;
   end process;

   carry : process is
   begin
      Cout <= (x and y) or (Cin and x) or (Cin and y);
      wait on x, y, Cin;
   end process;
end beh;
