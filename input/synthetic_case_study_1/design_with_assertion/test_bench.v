`timescale 1ns/1ns

module test_bench();

reg DEFAULT_RESET;
reg a_5_1;
reg a_5_0;
reg a_1_1;
reg a_1_0;
reg clock;
reg a_4_0;
reg a_4_1;
reg a_3_1;
reg a_3_0;
reg a_10_0;
reg a_10_1;
reg a_2_0;
reg a_2_1;
reg a_8_1;
reg a_7_1;
reg a_7_0;
reg a_8_0;
reg a_9_1;
reg a_9_0;
reg a_6_0;
reg a_6_1;

wire c_10_0;
wire c_4_0;
wire c_4_1;
wire c_8_0;
wire c_1_1;
wire c_1_0;
wire c_8_1;
wire c_10_1;
wire c_3_1;
wire c_3_0;
wire c_7_1;
wire c_7_0;
wire c_2_0;
wire c_2_1;
wire c_5_1;
wire c_5_0;
wire c_6_0;
wire c_6_1;
wire c_9_1;
wire c_9_0;

//test test_ (
//	.a_5_1(a_5_1),
//	.a_5_0(a_5_0),
//	.a_1_1(a_1_1),
//	.a_1_0(a_1_0),
//	.clock(clock),
//	.a_4_0(a_4_0),
//	.a_4_1(a_4_1),
//	.a_3_1(a_3_1),
//	.a_3_0(a_3_0),
//	.a_10_0(a_10_0),
//	.a_10_1(a_10_1),
//	.a_2_0(a_2_0),
//	.a_2_1(a_2_1),
//	.a_8_1(a_8_1),
//	.a_7_1(a_7_1),
//	.a_7_0(a_7_0),
//	.a_8_0(a_8_0),
//	.a_9_1(a_9_1),
//	.a_9_0(a_9_0),
//	.a_6_0(a_6_0),
//	.a_6_1(a_6_1),
//	.c_10_0(c_10_0),
//	.c_4_0(c_4_0),
//	.c_4_1(c_4_1),
//	.c_8_0(c_8_0),
//	.c_1_1(c_1_1),
//	.c_1_0(c_1_0),
//	.c_8_1(c_8_1),
//	.c_10_1(c_10_1),
//	.c_3_1(c_3_1),
//	.c_3_0(c_3_0),
//	.c_7_1(c_7_1),
//	.c_7_0(c_7_0),
//	.c_2_0(c_2_0),
//	.c_2_1(c_2_1),
//	.c_5_1(c_5_1),
//	.c_5_0(c_5_0),
//	.c_6_0(c_6_0),
//	.c_6_1(c_6_1),
//	.c_9_1(c_9_1),
//	.c_9_0(c_9_0));

test test_ (
	.clock(clock),
	.a_1_1(a_1_1),
	.a_1_0(a_1_0),
	.c_1_1(c_1_1),
	.c_1_0(c_1_0));

	initial begin
		a_5_1 = 0;
		a_5_0 = 0;
		a_1_1 = 0;
		a_1_0 = 0;
		a_4_0 = 0;
		a_4_1 = 0;
		a_3_1 = 0;
		a_3_0 = 0;
		a_10_0 = 0;
		a_10_1 = 0;
		a_2_0 = 0;
		a_2_1 = 0;
		a_8_1 = 0;
		a_7_1 = 0;
		a_7_0 = 0;
		a_8_0 = 0;
		a_9_1 = 0;
		a_9_0 = 0;
		a_6_0 = 0;
		a_6_1 = 0;
		$dumpfile("test.vcd");
		$dumpvars(0, test_bench.test_);
		clock = 1;
		#20 $finish;
	end

	always begin
		#1 clock = ~clock;
	end

	always begin
      //wait for negedge clock;
      @(negedge clock);
		a_5_1 = $random;
		a_5_0 = $random;
		a_1_1 = $random;
		a_1_0 = $random;
		a_4_0 = $random;
		a_4_1 = $random;
		a_3_1 = $random;
		a_3_0 = $random;
		a_10_0 = $random;
		a_10_1 = $random;
		a_2_0 = $random;
		a_2_1 = $random;
		a_8_1 = $random;
		a_7_1 = $random;
		a_7_0 = $random;
		a_8_0 = $random;
		a_9_1 = $random;
		a_9_0 = $random;
		a_6_0 = $random;
		a_6_1 = $random;
	end

endmodule
