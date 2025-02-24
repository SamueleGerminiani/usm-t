module top_module(clk,a_0,b_0,a_1,b_1,c_0,d_0,c_1,d_1);
input clk,a_0,b_0,a_1,b_1;
output c_0,d_0,c_1,d_1;
spec0 spec_sbm0(.clk(clk), .a_0(a_0), .b_0(b_0), .c_0(c_0), .d_0(d_0));
spec1 spec_sbm1(.clk(clk), .a_1(a_1), .b_1(b_1), .c_1(c_1), .d_1(d_1));
endmodule
