
#Trace reader
SET(TR_SRC src/traceReader/interface/TraceReader.cc
	src/traceReader/vcdTraceReader/VCDtraceReader.cc
	src/traceReader/vcdTraceReader/VCDParser.cpp
	src/traceReader/vcdTraceReader/VCDScanner.cpp
	src/traceReader/vcdTraceReader/VCDFile.cpp
	src/traceReader/vcdTraceReader/VCDValue.cpp
	src/traceReader/vcdTraceReader/VCDFileParser.cpp
	src/traceReader/csvTraceReader/CSVtraceReader.cc
	)
add_library(traceReader ${TR_SRC} )
target_include_directories(traceReader PUBLIC 
	include/traceReader/interface/
	include/traceReader/vcdTraceReader/
	include/traceReader/csvTraceReader/
	../../csv-parser/include/
	)

target_link_libraries(traceReader csv varParser propositionParser miner_utils)
