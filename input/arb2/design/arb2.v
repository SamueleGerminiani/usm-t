module arb2(clk, rst, req1, req2, gnt1, gnt2);

input clk, rst;
input[31:0] req1, req2;
output[31:0] gnt1, gnt2;

reg[31:0] state;
reg[31:0] gnt1, gnt2;

always @ (posedge clk or posedge rst)
	if (rst)
		state <= 0;
	else
		state <= gnt1;

always @ (*)
	if (state)
	begin
		gnt1 = req1 & ~req2;
		gnt2 = req2;
	end
	else
	begin
		gnt1 = req1;
		gnt2 = req2 & ~req1;
	end

endmodule
