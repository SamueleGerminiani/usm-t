module top_module(clock,c_0,c_1,a_1,a_0,b_0,e_0,d_0);
input clock,c_0,c_1,a_1,a_0,b_0;
output e_0,d_0;
spec0 spec_sbm0(.clock(clock), .a_0(a_0), .b_0(b_0), .c_0(c_0), .d_0(d_0), .e_0(e_0));
spec1 spec_sbm1(.clock(clock), .a_1(a_1), .a_0(a_0), .c_1(c_1), .d_0(d_0), .e_0(e_0));
endmodule
