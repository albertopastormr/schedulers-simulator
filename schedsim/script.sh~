#!/bin/bash

LOG_DIR=$(pwd)/script_log_out
EPS_DIR=$(pwd)/script_eps_out

if [ -d $LOG_DIR ]; then
	$(rm -rf $LOG_DIR)
fi

mkdir $LOG_DIR

if [ -d $EPS_DIR ]; then
	$(rm -rf $EPS_DIR)
fi

mkdir $EPS_DIR

let example_filename # Nombre del fichero de ejemplo a ejecutar en el script
let maxCPUs=0 # Numero maximo de cpus a utilizar en la ejecucion del script

declare -a sched_names_array=("RR", "SJF", "FCFS", "PRIO")

read -p "Enter example filename --> " examples_filename

read -p "Enter CPU max --> " maxCPUs

for sched_name in "${sched_names_array[@]}" do

	# Directorios por cada scheduler para ficheros log y eps
	mkdir $LOG_DIR/$sched_name
	mkdir $EPS_DIR/$sched_name

	for cpus = 1 to maxCPUs do

		./schedsim -i $examples_filename -n $cpus -s $sched_name

		for i=1 to cpus do
			mv CPU_$i.log $LOG_DIR/$sched_name/CPU_$i.log
		done
	
		for i=1 to cpus do
			../gantt-gplot/generate_gantt_chart $LOG_DIR/$sched_name/CPU_$i.log
		done

		for i=1 to cpus do
			mv $LOG_DIR/$sched_name/CPU_$i.eps $EPS_DIR/$sched_name/CPU_$i.log
		done

	done
done

