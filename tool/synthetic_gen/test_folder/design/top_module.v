module top_module(clock,b_1,b_0,a_0,a_1,c_0,d_0,e_0);
input clock,b_1,b_0,a_0,a_1,c_0;
output d_0,e_0;
spec0 spec_sbm0(.clock(clock), .a_0(a_0), .b_0(b_0), .c_0(c_0), .d_0(d_0), .e_0(e_0));
spec1 spec_sbm1(.clock(clock), .a_1(a_1), .b_1(b_1), .c_0(c_0), .e_0(e_0), .d_0(d_0));
endmodule
