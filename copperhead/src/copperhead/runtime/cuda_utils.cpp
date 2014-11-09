#include <boost/python.hpp>
#include <cuda.h>
#include <cuda_runtime.h>

using namespace boost::python;

namespace copperhead {
namespace cuda {

list cached_info;

void initialize() {
    int count;
    cudaGetDeviceCount(&count);
    for(int i = 0; i < count; i++) {
        struct cudaDeviceProp p;
        cudaGetDeviceProperties(&p, i);
        list li;
        li.append(p.major);
        li.append(p.minor);
        cached_info.append(::tuple(li));
    }
}

}
}

list get_cuda_info() {
    return copperhead::cuda::cached_info;
}

BOOST_PYTHON_MODULE(cuda_utils) {
    copperhead::cuda::initialize();
    def("get_cuda_info", &get_cuda_info);
}
