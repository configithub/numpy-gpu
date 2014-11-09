numpy-gpu
=========

Using numpy on a nvidia GPU (using Copperhead).
dep:
GCC 4.8
CUDA 6.5


to make it work I had to :
  - change all usage of move() function in copperhead source to std::move() to avoid the confusion with boost::move()
  - remove a restriction on the GCC version somewhere in cuda or thrust include files
  - add an include on thrust/retag somewhere in copperhead source
  - source env.sh
