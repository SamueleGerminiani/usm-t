module AndChain_top(clk,b_2,c_2,c_0,b_0,a_3,c_3,a_0,b_4,a_1,a_2,b_3,c_4,c_1,a_4,b_1,d_0,e_4,f_1,f_4,f_2,e_3,f_3,d_1,d_4,e_1,f_0,e_0,d_2,d_3,e_2);
input clk,b_2,c_2,c_0,b_0,a_3,c_3,a_0,b_4,a_1,a_2,b_3,c_4,c_1,a_4,b_1;
output d_0,e_4,f_1,f_4,f_2,e_3,f_3,d_1,d_4,e_1,f_0,e_0,d_2,d_3,e_2;
spec0 spec_sbm0(.clk(clk), .a_0(a_0), .b_0(b_0), .c_0(c_0), .d_0(d_0), .e_0(e_0), .f_0(f_0));
spec1 spec_sbm1(.clk(clk), .a_1(a_1), .b_1(b_1), .c_1(c_1), .d_1(d_1), .e_1(e_1), .f_1(f_1));
spec2 spec_sbm2(.clk(clk), .a_2(a_2), .b_2(b_2), .c_2(c_2), .d_2(d_2), .e_2(e_2), .f_2(f_2));
spec3 spec_sbm3(.clk(clk), .a_3(a_3), .b_3(b_3), .c_3(c_3), .d_3(d_3), .e_3(e_3), .f_3(f_3));
spec4 spec_sbm4(.clk(clk), .a_4(a_4), .b_4(b_4), .c_4(c_4), .d_4(d_4), .e_4(e_4), .f_4(f_4));
endmodule
