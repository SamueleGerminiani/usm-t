module top_module(
clock,a_0,b_0,a_1,b_1,a_2,b_2,a_3,b_3,a_4,b_4,a_5,b_5,c_0,c_1,c_2,c_3,c_4,c_5);
input clock,a_0,b_0,a_1,b_1,a_2,b_2,a_3,b_3,a_4,b_4,a_5,b_5;
output c_0,c_1,c_2,c_3,c_4,c_5;
spec0 spec_sbm0(.clock(clock), .a_0(a_0), .b_0(b_0), .c_0(c_0));
spec1 spec_sbm1(.clock(clock), .a_1(a_1), .b_1(b_1), .c_1(c_1));
spec2 spec_sbm2(.clock(clock), .a_2(a_2), .b_2(b_2), .c_2(c_2));
spec3 spec_sbm3(.clock(clock), .a_3(a_3), .b_3(b_3), .c_3(c_3));
spec4 spec_sbm4(.clock(clock), .a_4(a_4), .b_4(b_4), .c_4(c_4));
spec5 spec_sbm5(.clock(clock), .a_5(a_5), .b_5(b_5), .c_5(c_5));
endmodule
