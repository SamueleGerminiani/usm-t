cd $USMT_ROOT/tool/build/
#delete previous results if they exist
if [ -d "todaes2026_case_studies_results" ]; then
  rm -r todaes2026_case_studies_results
fi

mkdir todaes2026_case_studies_results
./usm-t --test ../../tests/todaes2026_case_studies/And_synthetic.xml --dump-to todaes2026_case_studies_results/
./usm-t --test ../../tests/todaes2026_case_studies/Next_synthetic.xml --dump-to todaes2026_case_studies_results/
./usm-t --max-mined 10000 --ltlf --test ../../tests/todaes2026_case_studies/Eventually_synthetic.xml --dump-to todaes2026_case_studies_results/
