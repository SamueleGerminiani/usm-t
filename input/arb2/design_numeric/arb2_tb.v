`timescale 1ns / 1ps

module arb2_tb;

  // Testbench signals
  reg clk;
  reg rst;
  reg [31:0] req1;
  reg [31:0] req2;
  wire[31:0] gnt1;
  wire[31:0] gnt2;

  // Instantiate the design under test (DUT)
  arb2 uut (
    .clk(clk),
    .rst(rst),
    .req1(req1),
    .req2(req2),
    .gnt1(gnt1),
    .gnt2(gnt2)
  );

  // Clock generation
  initial begin
    clk = 0;
    forever #5 clk = ~clk; // 10ns clock period
  end

  // Test sequence
  initial begin
    // Initialize inputs
    rst = 1;
    req1 = 0;
    req2 = 0;

    // Apply reset
    #10;
    rst = 0;

    // Generate random requests for 1000 clock cycles
    repeat (1000) begin
      req1 = $random % 2;
      req2 = $random % 2;
      #10; // Wait for one clock cycle
    end

    // End simulation
    $stop;
  end

  // Monitor signals
  initial begin
    $monitor($time, " clk=%b rst=%b req1=%b req2=%b gnt1=%b gnt2=%b", clk, rst, req1, req2, gnt1, gnt2);
  end

endmodule

