#!/bin/bash
#SBATCH -J serial_naive # nombre del job
#SBATCH -p investigacion # nombre de la particion 
#SBATCH -N 1 # numero de nodos
#SBATCH --tasks-per-node=1 # numero de tasks por nodo
#SBATCH --output=log_naive # numero de tasks por nodo

# module load openmpi/2.1.6 # carga el modulo de openmpi version 2.1.6
srun serial_naive # siendo prueba_mpi el nombre del programa mpi
# module unload openmpi/2.1.6 