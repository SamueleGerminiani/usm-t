verilog2hif rtl/* --output synthetic_case_study_1.hif.xml

ddt synthetic_case_study_1.hif.xml --toplevel test --output synthetic_case_study_1.ddt.hif.xml

a2tool synthetic_case_study_1.ddt.hif.xml --protocol CPP --toplevel test --output synthetic_case_study_1.ddt.a2t.hif.xml

muffin synthetic_case_study_1.ddt.a2t.hif.xml --fault STUCK_AT --clock clock --toplevel test --output synthetic_case_study_1.ddt.a2t.muffin.hif.xml

hif2sc synthetic_case_study_1.ddt.a2t.muffin.hif.xml --extensions cpp_hpp --directory injected


