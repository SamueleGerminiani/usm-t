module AndChainOvl_top(clk,b_2,c_0,a_3,c_2,b_1,c_1,b_4,b_3,a_4,a_0,b_0,d_2,f_0,e_0,d_4,e_3,d_0,d_1);
input clk,b_2,c_0,a_3,c_2,b_1,c_1,b_4,b_3,a_4,a_0,b_0;
output d_2,f_0,e_0,d_4,e_3,d_0,d_1;
spec0 spec_sbm0(.clk(clk), .a_0(a_0), .b_0(b_0), .c_0(c_0), .d_0(d_0), .e_0(e_0), .f_0(f_0));
spec1 spec_sbm1(.clk(clk), .b_0(b_0), .b_1(b_1), .c_1(c_1), .d_1(d_1), .e_0(e_0), .f_0(f_0));
spec2 spec_sbm2(.clk(clk), .b_1(b_1), .b_2(b_2), .c_2(c_2), .d_2(d_2), .d_0(d_0), .e_0(e_0));
spec3 spec_sbm3(.clk(clk), .a_3(a_3), .b_3(b_3), .c_1(c_1), .e_0(e_0), .e_3(e_3), .f_0(f_0));
spec4 spec_sbm4(.clk(clk), .a_4(a_4), .b_4(b_4), .c_0(c_0), .d_4(d_4), .d_1(d_1), .e_3(e_3));
endmodule
