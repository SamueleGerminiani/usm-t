module Next_top(clk,b_0,b_4,b_6,b_8,a_8,a_5,b_2,a_9,a_0,b_9,a_4,a_3,a_2,a_6,b_7,c_7,d_7,d_4,d_5,c_1,d_9,c_2,d_8,d_1,c_5,d_0,d_2,c_8,c_0,d_3);
input clk,b_0,b_4,b_6,b_8,a_8,a_5,b_2,a_9,a_0,b_9,a_4,a_3,a_2,a_6,b_7;
output c_7,d_7,d_4,d_5,c_1,d_9,c_2,d_8,d_1,c_5,d_0,d_2,c_8,c_0,d_3;
spec0 spec_sbm0(.clk(clk), .a_0(a_0), .b_0(b_0), .c_0(c_0), .d_0(d_0));
spec1 spec_sbm1(.clk(clk), .a_0(a_0), .a_5(a_5), .c_1(c_1), .d_1(d_1));
spec2 spec_sbm2(.clk(clk), .a_2(a_2), .b_2(b_2), .c_2(c_2), .d_2(d_2));
spec3 spec_sbm3(.clk(clk), .a_3(a_3), .a_8(a_8), .c_2(c_2), .d_3(d_3));
spec4 spec_sbm4(.clk(clk), .a_4(a_4), .b_4(b_4), .c_8(c_8), .d_4(d_4));
spec5 spec_sbm5(.clk(clk), .a_5(a_5), .b_9(b_9), .c_5(c_5), .d_5(d_5));
spec6 spec_sbm6(.clk(clk), .a_6(a_6), .b_6(b_6), .c_7(c_7), .d_4(d_4));
spec7 spec_sbm7(.clk(clk), .b_0(b_0), .b_7(b_7), .c_7(c_7), .d_7(d_7));
spec8 spec_sbm8(.clk(clk), .a_8(a_8), .b_8(b_8), .c_8(c_8), .d_8(d_8));
spec9 spec_sbm9(.clk(clk), .a_9(a_9), .b_9(b_9), .d_2(d_2), .d_9(d_9));
endmodule
