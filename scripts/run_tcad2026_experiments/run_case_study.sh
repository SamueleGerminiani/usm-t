cd $USMT_ROOT/tool/build/
#delete previous results if they exist
#if [ -d "tcad2026_case_studies_results" ]; then
#  rm -r tcad2026_case_studies_results
#fi
#
#mkdir tcad2026_case_studies_results
./usm-t --test ../../tests/tcad2026_case_studies/And_synthetic.xml --dump-to tcad2026_case_studies_results/
#./usm-t --test ../../tests/tcad2026_case_studies/Next_synthetic.xml --dump-to tcad2026_case_studies_results/
#./usm-t --test ../../tests/tcad2026_case_studies/Eventually_synthetic.xml --dump-to tcad2026_case_studies_results/
