#
#   Copyright 2012      NVIDIA Corporation
# 
#   Licensed under the Apache License, Version 2.0 (the "License");
#   you may not use this file except in compliance with the License.
#   You may obtain a copy of the License at
# 
#       http://www.apache.org/licenses/LICENSE-2.0
# 
#   Unless required by applicable law or agreed to in writing, software
#   distributed under the License is distributed on an "AS IS" BASIS,
#   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#   See the License for the specific language governing permissions and
#   limitations under the License.
# 
#
import backendcompiler as BC
import conversions

def execute(ast, M):
    assert(len(M.entry_points) == 1)
    entry_point = M.entry_points[0]
    backend_ast = conversions.front_to_back_node(ast)
    c = BC.Compiler(entry_point, M.tag)
    result = c(backend_ast)
    M.compiler_output = result
    M.wrap_info = (BC.hash(), (BC.wrap_result_type(), BC.wrap_name()),
                   zip(BC.wrap_arg_types(), BC.wrap_arg_names()))
    return []
