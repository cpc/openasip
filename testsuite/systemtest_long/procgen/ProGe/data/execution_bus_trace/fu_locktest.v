// Copyright (c) 2002-2014 Tampere University of Technology.
//
// This file is part of TTA-Based Codesign Environment (TCE).
// 
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.

module fu_locktest 
  (  
   input clk,
   input rstx,
   input glock,
   output glock_req,
   input[0 : 0] t1data,  
   input t1load
   );
   
   reg   enable_reg;
   reg   lock_reg;

   always@(posedge clk or negedge rstx)
     if(~rstx)// asynchronous reset (active low)
       begin
          enable_reg <= 0;
          lock_reg <= 0;
       end
     else
       begin
          if (~glock)
            if (t1load)
              if (t1data)
                enable_reg <= 1;
              else
                enable_reg <= 0;  

          if (enable_reg)
            if (lock_reg)
              lock_reg <= 0;
            else
              lock_reg <= 1;
          else
            lock_reg <= 0;
       end // else: !if(~rstx)

   assign glock_req = lock_reg;   
   
endmodule