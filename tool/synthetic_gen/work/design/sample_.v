module sample_(clk,a_2,a_1,b_2,b_1,b_0,a_0,c_1,c_0,c_2,d_0,d_2,d_1);
input clk,a_2,a_1,b_2,b_1,b_0,a_0;
output c_1,c_0,c_2,d_0,d_2,d_1;
spec0 spec_sbm0(.clk(clk), .a_0(a_0), .b_0(b_0), .c_0(c_0), .d_0(d_0));
spec1 spec_sbm1(.clk(clk), .a_1(a_1), .b_1(b_1), .c_1(c_1), .d_1(d_1));
spec2 spec_sbm2(.clk(clk), .a_2(a_2), .b_2(b_2), .c_2(c_2), .d_2(d_2));
endmodule
