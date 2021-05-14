#!/bin/bash
#SBATCH -J serial_naive_4096 # nombre del job
#SBATCH -N 1 # numero de nodos
#SBATCH --tasks-per-node=1 # numero de tasks por nodo
#SBATCH --output=log_naive_4096 # numero de tasks por nodo

module load gcc/5.5.0
srun serial_naive_4096 # siendo prueba_mpi el nombre del programa mpi
module unload gcc/5.5.0