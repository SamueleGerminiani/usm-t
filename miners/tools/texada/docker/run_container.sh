input_mount_volume=$1
output_mount_volume=$2
com=$3
docker run -e MINED_ASSERTIONS_FILE=$MINED_ASSERTIONS_FILE -u root --rm -it -v $input_mount_volume:/input:rw -v $output_mount_volume:/output:rw samger/texada:latest $com
