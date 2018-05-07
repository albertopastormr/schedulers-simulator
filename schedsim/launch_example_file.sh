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

declare -a sched_names_array=("RR" "MULTIRR" "SJF" "FCFS" "PRIO")

read -p "Enter example filename --> " examples_filename

read -p "Enter CPU max --> " maxCPUs

chmod +x ../gantt-gplot/gantt.py

for sched_name in "${sched_names_array[@]}"
do

	echo SCHEDULER $sched_name RUNNING FILE $examples_filename 
	# Directorios por cada scheduler para ficheros log y eps
	mkdir $LOG_DIR/$sched_name
	mkdir $EPS_DIR/$sched_name

	for (( num_cpus=1 ; num_cpus <= maxCPUs ; num_cpus++ ))
	do
		./schedsim -i $examples_filename -n $num_cpus -s $sched_name
		# Movimiento de los ficheros .log generados por el simulador (genera 1 fichero por CPU)
		for (( i=0; i < num_cpus ; i++ ))
		do
			mv CPU_$i.log $LOG_DIR/$sched_name/num_ex_${num_cpus}"_num_CPU_n$i.log"
		done
		# Generacion de grafica por cada .log desarrollado por el simulador
		for (( i=0; i < num_cpus ; i++ ))
		do
			cd ../gantt-gplot
			./generate_gantt_chart $LOG_DIR/$sched_name/num_ex_${num_cpus}"_num_CPU_n$i.log"
		done
		# Movimiento de los ficheros .eps generados
		for (( i=0; i < num_cpus ; i++ ))
		do	
			mv $LOG_DIR/$sched_name/num_ex_${num_cpus}"_num_CPU_n$i.eps" $EPS_DIR/$sched_name/num_ex_${num_cpus}"_num_CPU_n$i.eps"
		done
		cd ../schedsim

	done
done

