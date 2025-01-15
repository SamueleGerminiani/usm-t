input_mount_volume=$1
output_mount_volume=$2
com=$3
docker run --cpus 1 -e MINED_ASSERTIONS_FILE=$MINED_ASSERTIONS_FILE -u root --rm -it -v $input_mount_volume:/input:rw -v $output_mount_volume:/output:rw samger/samples2ltl:latest $com
