input_mount_volume=$1
output_mount_volume=$2
command_to_run_the_miner=$3
xml_exported_vars=$4

docker run \
--cpus $MAX_N_CORES \
-e MINED_ASSERTIONS_FILE=$MINED_ASSERTIONS_FILE $xml_exported_vars \
-u root --rm -it \
-v $input_mount_volume:/input:rw \
-v $output_mount_volume:/output:rw \
samger/texada:latest \
$command_to_run_the_miner

