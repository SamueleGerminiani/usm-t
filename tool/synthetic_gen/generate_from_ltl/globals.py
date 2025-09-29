import os

root = os.environ["USMT_ROOT"]
yosis_prefix = root + '/tool/third_party/oss-cad-suite/bin/'
ltlsynt_prefix = root + '/tool/third_party/spot/bin/'

hif_temp_prefix = root + '/tool/synthetic_gen/.temp/hif_sim/'
hif_template_prefix = root + '/tool/synthetic_gen/hif_components/'

out_folder = root + '/tool/synthetic_gen/.temp/'
debug = False
clk_name = ""
top_module_name = ""
tracelnegth = 1000

#debug color
CDBG = '\033[43m'
#error color
CERR = '\033[41m'
#warning color
CWRN = '\033[208m'
#procedure step color
CSTP = '\033[42m'
#terminal color reset
CEND = '\033[0m'
