// Define statements for instructions
`define ADD		3'b000
`define ADDI	3'b001
`define NAND	3'b010
`define LUI		3'b011
`define SW		3'b100
`define LW		3'b101
`define BNE		3'b110
`define JALR	3'b111
//Define Statements for instruction formats
`define OP 15:13
`define RA 12:10
`define RB 9:7
`define RC 2:0
`define UI 9:0
`define SI 6:0

`define ZERO 16'd0
`define HALT 32'hE071

module RISC16(clk);
		input 	clk;
		reg	[15:0] rf[0:7]; 		// Each of the registers
		reg	[15:0] pc; 				// Program counter
		reg	[15:0] m[0:65535];	// Memorty, 64K 16 bit spaces
		reg	[15:0] instr;
		reg j;
		initial begin
				pc = 0;
				rf[0] = `ZERO;
				rf[1] = `ZERO;
				rf[2] = `ZERO;
				rf[3] = `ZERO;
				rf[4] = `ZERO;
				rf[5] = `ZERO;
				rf[6] = `ZERO;
				rf[7] = `ZERO;
				for(j=0;j<65535;j=j+1)
						m[j] = 0;
		end
		
		always @(negedge clk) begin
					rf[0] = `ZERO; // The zero register is always zero by convention
		end
		always @(posedge clk) begin
				instr = m[pc];
				case(instr[`OP])
					`ADD	: rf[instr[`RA]] <= rf[instr[`RB]]+rf[instr[`RC]];
					`ADDI	: rf[instr[`RA]] <= rf[instr[`RB]]+rf[instr[`SI]];
					`NAND	: rf[instr[`RA]] <= !(rf[instr[`RB]]&&rf[instr[`RC]]);
					`LUI	: rf[instr[`RA]] <= (32'hFFC0 && rf[instr[`UI]]);
					`SW	: m[rf[instr[`RB]]+rf[instr[`SI]]] <= rf[instr[`RA]];
					`LW	: rf[instr[`RA]] <= m[rf[instr[`RB]]+rf[instr[`SI]]];
					`BNE	: if(rf[instr[`RA]]) pc<=m[rf[instr[`RB]]];
					`JALR	: begin pc<=rf[instr[`RB]]; rf[instr[`RA]]<=(pc+1); end
				endcase									
		end
endmodule

//This is code that was given in the assignment
module top ();
	reg clk;
	RiSC cpu(clk);
	integer j;
	initial begin
		#1	$readmemh("init.dat",cpu.m);
		#1000 stop
	end
	
	always begin
		#5 clock = 0;
		#5 clock = 1;
		$display(“Register Contents:”);
		$display(“ r0 - %h”, cpu.rf[0]);
		$display(“ r1 - %h”, cpu.rf[1]);
		$display(“ r2 - %h”, cpu.rf[2]);
		$display(“ r3 - %h”, cpu.rf[3]);
		$display(“ r4 - %h”, cpu.rf[4]);
		$display(“ r5 - %h”, cpu.rf[5]);
		$display(“ r6 - %h”, cpu.rf[6]);
		$display(“ r7 - %h”, cpu.rf[7]);
	end
endmodule

	


