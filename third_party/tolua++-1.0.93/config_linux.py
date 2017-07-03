
## This is the linux configuration file
# use 'scons -h' to see the list of command line options available

# Compiler flags (based on Debian's installation of lua)
#LINKFLAGS = ['-g']
#CCFLAGS = ['-I/home/niuyansheng/newServer/3Party/lua-5.1.4/include', '-O2', '-ansi', '-Wall', '-g']
#CCFLAGS = ['-I/home/niuyansheng/dev/server/ThirdParty/lua-5.1.4/include', '-O2', '-ansi', '-Wall', '-g']
CCFLAGS = ['-I/home/niuyansheng/dev/server/ThirdParty/LuaJIT-2.0.0-beta10/include', '-O2', '-ansi', '-Wall', '-g']
#CCFLAGS = ['-I/usr/include/lua50', '-g']

# this is the default directory for installation. Files will be installed on
# <prefix>/bin, <prefix>/lib and <prefix>/include when you run 'scons install'
#
# You can also specify this directory on the command line with the 'prefix'
# option
#
# You can see more 'generic' options for POSIX systems on config_posix.py

prefix = '/usr/local'

#LIBS = ['lua', 'm', 'dl']
#LIBPATH = ['/home/niuyansheng/dev/server/ThirdParty/lua-5.1.4/lib']
# libraries (based on Debian's installation of lua)
#LIBPATH = ['/home/niuyansheng/newServer/3Party/lua-5.1.4/lib']
LIBS = ['luajit-5.1', 'm', 'dl']
LIBPATH = ['/home/niuyansheng/dev/server/ThirdParty/LuaJIT-2.0.0-beta10/lib']
