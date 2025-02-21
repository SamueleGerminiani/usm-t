| To Fix | Status | Notes |
|---|---|---|
| Where is the output? add --output option (or similar to specify the output directory) |  | output directory is specified in the config, otherwise is default to "./synthetic_gen_output" |
| Separate the faulty and golden traces in the output directory (see the arb2 input) |  |  |
| Remove empty lines in expected specificatins |  |  |
| Length of traces input parameter missing |  |  |
| Remove the configuration file, there are too few parameters to justify it, use command line arguments instead. Also use command line arguments to specify the templates (is this a good idea?) |  |  |
| Add tests for most combinations of input parameters, including the different templates |  |  |
| New feature: decide how many specifications in the same module (max 5 per module: ltl2synth explodes) |  |  |
| New feature: need to automatically instrument usm-t with the generated synthetic test. Furthermore, we need to generate several synthetic tests, especially if HIF performance issues are not solvable. Use the "usm-t/miners/usmt_default_config.xml" to generate the synthetic test xml file. |  |  |
| Performance fix: why is HIF so slow? Is because it is compiled in debug mode? Recompile HIF in release mode. |  |  |
