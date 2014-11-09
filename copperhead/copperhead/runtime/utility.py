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
import cufunction


import pdb
import sys, os, os.path, shutil

def print_code(code, dest_path, name, ext):
    o_file = open(os.path.join(dest_path, name + ext), 'w')
    print >>o_file, code
    o_file.close()

def mkdir(dirname):
    exists = os.access(dirname, os.F_OK)
    if not exists:
        os.mkdir(dirname)

def save_code(objects, source_file=None):
    if source_file:
        dirname, ext = os.path.splitext(source_file)
        mkdir(dirname)
        shutil.copy(source_file, dirname)
    else:
        dirname = '.'
    for name, v in objects.items():
        if isinstance(v, cufunction.CuFunction):
            code = v.get_code()
            #There may be many variants, let's just grab the first
            implementations = code.values()
            if len(implementations) > 0:
                selected_impl = implementations[0]
                extensions = ('.py', '.cpp', '.cu')
                #make target directory
                dest_path = os.path.join(dirname, name)
                mkdir(dest_path)
                map(lambda x, y: print_code(x, dest_path, name, y),
                    selected_impl,
                    extensions)
            
         
