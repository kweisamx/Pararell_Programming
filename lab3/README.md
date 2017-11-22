# MPI_train

please use the command to build mpi code

```
mpicc -o prime prime.c -lm
mpicc -o integrate integrate.c -lm
```

run with

```
mpiexec -n 1 -host {yourHostIP} ./prime 1234567
mpiexec -n 1 -host {yourHostIP} ./integrate 1234
```

if you have many machine 

```
mpiexec -n 3 -host {yourHostIP},{slave1},{slave2} ./prime 1234567
```

Enjoy!
