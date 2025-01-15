message("-- Including examples...")


set(WORK_DIR ${CMAKE_BINARY_DIR})
set(EXAMPLE_DIR ${CMAKE_BINARY_DIR}/../examples)



#examples
#add_test(NAME vendingMachine COMMAND harm --vcd ${EXAMPLE_DIR}/vendingMachine/vendingMachine.vcd --clk clk --conf ${EXAMPLE_DIR}/vendingMachine/vendingMachineConfig.xml --vcd-ss machine_bench::machine_ WORKING_DIRECTORY ${WORK_DIR})
