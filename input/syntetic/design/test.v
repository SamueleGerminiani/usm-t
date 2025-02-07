module test(
clock,a_1,b_1,a_2,b_2,a_3,b_3,c_1,c_2,c_3);
input clock,a_1,b_1,a_2,b_2,a_3,b_3;
output c_1,c_2,c_3;
spec0 spec_sbm0(.clock(clock), .a_1(a_1), .b_1(b_1), .c_1(c_1));
spec1 spec_sbm1(.clock(clock), .a_2(a_2), .b_2(b_2), .c_2(c_2));
spec2 spec_sbm2(.clock(clock), .a_3(a_3), .b_3(b_3), .c_3(c_3));
endmodule
