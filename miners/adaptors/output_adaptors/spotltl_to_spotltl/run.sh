in_path=$1
out_path=$2

if [ -z "$in_path" ] || [ -z "$out_path" ]; then
    echo "Usage: run.sh <in_path> <out_path>"
    exit 1
fi

cat $in_path

#check that in_path is a file
if [ ! -f $in_path ]; then
  echo "Error: $in_path is not a file"
    exit 1
fi

cp $in_path $out_path
