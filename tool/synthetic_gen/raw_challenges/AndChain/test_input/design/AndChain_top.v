module AndChain_top(clk,b_0,a_4,a_8,c_2,c_0,a_0,c_3,b_1,a_7,b_9,a_6,c_5,e_0,d_0,f_0);
input clk,b_0,a_4,a_8,c_2,c_0,a_0,c_3,b_1,a_7,b_9,a_6,c_5;
output e_0,d_0,f_0;
spec0 spec_sbm0(.clk(clk), .a_0(a_0), .b_0(b_0), .c_0(c_0), .d_0(d_0), .e_0(e_0), .f_0(f_0));
spec1 spec_sbm1(.clk(clk), .c_0(c_0), .b_1(b_1), .b_0(b_0), .d_0(d_0), .f_0(f_0), .e_0(e_0));
spec2 spec_sbm2(.clk(clk), .a_0(a_0), .b_1(b_1), .c_2(c_2), .f_0(f_0), .d_0(d_0), .e_0(e_0));
spec3 spec_sbm3(.clk(clk), .b_1(b_1), .c_0(c_0), .c_3(c_3), .f_0(f_0), .e_0(e_0), .d_0(d_0));
spec4 spec_sbm4(.clk(clk), .a_4(a_4), .a_0(a_0), .c_0(c_0), .f_0(f_0), .d_0(d_0), .e_0(e_0));
spec5 spec_sbm5(.clk(clk), .a_4(a_4), .b_1(b_1), .c_5(c_5), .e_0(e_0), .f_0(f_0), .d_0(d_0));
spec6 spec_sbm6(.clk(clk), .a_6(a_6), .c_5(c_5), .a_0(a_0), .d_0(d_0), .f_0(f_0), .e_0(e_0));
spec7 spec_sbm7(.clk(clk), .a_7(a_7), .c_5(c_5), .a_6(a_6), .d_0(d_0), .f_0(f_0), .e_0(e_0));
spec8 spec_sbm8(.clk(clk), .a_8(a_8), .c_2(c_2), .b_0(b_0), .e_0(e_0), .d_0(d_0), .f_0(f_0));
spec9 spec_sbm9(.clk(clk), .a_6(a_6), .b_9(b_9), .b_1(b_1), .f_0(f_0), .e_0(e_0), .d_0(d_0));
endmodule
