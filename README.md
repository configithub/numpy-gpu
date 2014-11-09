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

to install :
install CUDA 6.5 first, then :
(virtualenv nutshell; source nutshell/bin/activate)
pip install numpy codepy
cd copperhead
python setup.py build
python setup.py install

or if you trust it : source setup.sh

to test numpy with gpu :
cd copperhead/sample
(optirun) python axpy.py



