#!/bin/bash
#SBATCH -J serial_naive # nombre del job
#SBATCH -N 1 # numero de nodos
#SBATCH --tasks-per-node=8 # numero de tasks por nodo
#SBATCH --output=log_naive # numero de tasks por nodo

module load gcc/5.5.0
srun serial_naive # siendo prueba_mpi el nombre del programa mpi
module unload gcc/5.5.0