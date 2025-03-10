module NextAndChainOvl_top(clk,b_2,b_1,c_0,c_3,a_4,b_4,a_0,c_1,b_0,b_3,c_2,f_3,e_1,d_2,d_4,e_0,f_0,d_0);
input clk,b_2,b_1,c_0,c_3,a_4,b_4,a_0,c_1,b_0,b_3,c_2;
output f_3,e_1,d_2,d_4,e_0,f_0,d_0;
spec0 spec_sbm0(.clk(clk), .a_0(a_0), .b_0(b_0), .c_0(c_0), .d_0(d_0), .e_0(e_0), .f_0(f_0));
spec1 spec_sbm1(.clk(clk), .c_0(c_0), .b_1(b_1), .c_1(c_1), .e_0(e_0), .e_1(e_1), .d_0(d_0));
spec2 spec_sbm2(.clk(clk), .c_0(c_0), .b_2(b_2), .c_2(c_2), .d_2(d_2), .e_0(e_0), .d_0(d_0));
spec3 spec_sbm3(.clk(clk), .a_0(a_0), .b_3(b_3), .c_3(c_3), .e_1(e_1), .e_0(e_0), .f_3(f_3));
spec4 spec_sbm4(.clk(clk), .a_4(a_4), .b_4(b_4), .b_2(b_2), .d_4(d_4), .e_0(e_0), .d_0(d_0));
endmodule
