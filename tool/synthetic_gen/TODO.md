| To Fix | Status | Notes |
|---|---|---|
| Add tests for most combinations of input parameters, including the different templates |  |  |
| New feature: decide how many specifications in the same module (max 5 per module: ltl2synth explodes) |  |  |
| New feature: need to automatically instrument usm-t with the generated synthetic test. Furthermore, we need to generate several synthetic tests, especially if HIF performance issues are not solvable. Use the "usm-t/miners/usmt_default_config.xml" to generate the synthetic test xml file. |  |  |
| Performance fix: why is HIF so slow? Is because it is compiled in debug mode? Recompile HIF in release mode. |  |  |
| Add variables overlap support |  |  |