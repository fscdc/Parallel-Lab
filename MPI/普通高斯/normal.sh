#!/bin/sh
#PBS -N mpi-serial
#PBS -l nodes=4

LOG_DIR="/home/s2112213/parallel_FSC/MPI"

pssh -h $PBS_NODEFILE mkdir -p $LOG_DIR 1>&2
scp master:$LOG_DIR/mpi-serial $LOG_DIR
pscp -h $PBS_NODEFILE $LOG_DIR/mpi-serial $LOG_DIR 1>&2
$LOG_DIR/mpi-serial