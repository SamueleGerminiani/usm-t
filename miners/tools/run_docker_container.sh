docker_image_name=$1
input_mount_volume=$2
output_mount_volume=$3
command_to_run_the_miner=$4
xml_exported_vars=$5

docker run \
--cpus $MAX_N_CORES \
-e MINED_SPECIFICATIONS_FILE=$MINED_SPECIFICATIONS_FILE $xml_exported_vars \
-u root --rm -it \
-v $input_mount_volume:/input:rw \
-v $output_mount_volume:/output:rw \
$docker_image_name \
$command_to_run_the_miner
