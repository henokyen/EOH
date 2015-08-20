AR = '/usr/bin/ar'
ARFLAGS = 'rcs'
BOOST_VERSION = '1_52'
CCDEFINES_SDL = ['_GNU_SOURCE=1', '_REENTRANT']
CCDEFINES_SDL_gfx = ['_GNU_SOURCE=1', '_REENTRANT']
CCFLAGS_MACBUNDLE = ['-fPIC']
CC_VERSION = ('4', '6', '4')
COMPILER_CXX = 'g++'
CPP = '/usr/bin/cpp'
CPPPATH_BOOST = '/home/henok/sferes/include'
CPPPATH_EIGEN2 = ['/home/henok/sferes/include/']
CPPPATH_ODE = ['/usr/local/include']
CPPPATH_SDL = ['/usr/include/SDL']
CPPPATH_SDL_gfx = ['/usr/include/SDL']
CPPPATH_ST = '-I%s'
CPPPATH_TBB = ['/user/include/tbb/include']
CXX = ['/usr/bin/g++']
CXXDEFINES_SDL = ['_GNU_SOURCE=1', '_REENTRANT']
CXXDEFINES_SDL_gfx = ['_GNU_SOURCE=1', '_REENTRANT']
CXXDEFINES_ST = '-D%s'
CXXFLAGS = ['-D_REENTRANT', '-Wall', '-fPIC', '-ftemplate-depth-1024', '-Wno-sign-compare', '-Wno-deprecated', '', '-Wno-unused', '-DUSE_SDL', '-DEIGEN2_ENABLED', '', '-DSFERES_ROOT="/home/henok/workspace/sferes"', '', '-DNDEBUG', '-O3', '-fomit-frame-pointer', '-finline-functions', '-ftracer', '-funroll-loops', '-fvariable-expansion-in-unroller', '-fstrict-aliasing', '-mfpmath=sse', '-march=core2', '-msse2']
CXXFLAGS_DEBUG = ['-g']
CXXFLAGS_RELEASE = ['-O2']
CXXLNK_SRC_F = ''
CXXLNK_TGT_F = ['-o', '']
CXX_NAME = 'gcc'
CXX_SRC_F = ''
CXX_TGT_F = ['-c', '-o', '']
DEST_BINFMT = 'elf'
DEST_CPU = 'x86_64'
DEST_OS = 'linux'
EIGEN2_FOUND = True
FULLSTATIC_MARKER = '-static'
HAVE_SDL = 1
HAVE_SDL_gfx = 1
LIBPATH_BOOST_FILESYSTEM = ['/home/henok/sferes/lib']
LIBPATH_BOOST_GRAPH = ['/home/henok/sferes/lib']
LIBPATH_BOOST_MPI = ['/home/henok/sferes/lib']
LIBPATH_BOOST_PROGRAM_OPTIONS = ['/home/henok/sferes/lib']
LIBPATH_BOOST_PYTHON = ['/home/henok/sferes/lib']
LIBPATH_BOOST_SERIALIZATION = ['/home/henok/sferes/lib']
LIBPATH_BOOST_SYSTEM = ['/home/henok/sferes/lib']
LIBPATH_BOOST_THREAD = ['/home/henok/sferes/lib']
LIBPATH_BOOST_UNIT_TEST_FRAMEWORK = ['/home/henok/sferes/lib']
LIBPATH_EIGEN2 = ['/home/henok/sferes/include/']
LIBPATH_ODE = ['/usr/local/lib']
LIBPATH_OPENGL = '/usr/X11R6/lib'
LIBPATH_SDL_gfx = ['/usr/lib']
LIBPATH_ST = '-L%s'
LIBPATH_TBB = ['/user/include/tbb/lib/intel64/cc4.1.0_libc2.4_kernel2.6.16.21']
LIB_BOOST_FILESYSTEM = ['boost_filesystem']
LIB_BOOST_GRAPH = ['boost_graph']
LIB_BOOST_MPI = ['boost_mpi']
LIB_BOOST_PROGRAM_OPTIONS = ['boost_program_options']
LIB_BOOST_PYTHON = ['boost_python']
LIB_BOOST_SERIALIZATION = ['boost_serialization']
LIB_BOOST_SYSTEM = ['boost_system']
LIB_BOOST_THREAD = ['boost_thread']
LIB_BOOST_UNIT_TEST_FRAMEWORK = ['boost_unit_test_framework']
LIB_BULLET = ['bulletdynamics', 'bulletcollision', 'bulletmath']
LIB_BZIP2 = 'bz2'
LIB_EFENCE = 'efence'
LIB_EIGEN2 = ''
LIB_GSL = ['gsl', 'gslcblas']
LIB_MPI = 'mpi'
LIB_ODE = ['ode']
LIB_OPENGL = ['GL', 'GLU', 'glut']
LIB_OSG = ['osg', 'osgDB', 'osgUtil', 'osgViewer', 'OpenThreads', 'osgFX', 'osgShadow']
LIB_PTMALLOC = 'ptmalloc3'
LIB_SDL = ['SDL']
LIB_SDL_gfx = ['SDL_gfx']
LIB_ST = '-l%s'
LIB_TBB = ['tbb']
LIB_TCMALLOC = 'tcmalloc'
LIB_ZLIB = 'z'
LINKFLAGS_MACBUNDLE = ['-bundle', '-undefined', 'dynamic_lookup']
LINK_CXX = ['/usr/bin/g++']
MPI_ENABLED = False
MPI_FOUND = False
PREFIX = '/usr/local'
RANLIB = '/usr/bin/ranlib'
RPATH_ST = '-Wl,-rpath,%s'
SFERES_ROOT = '/home/henok/workspace/sferes'
SHLIB_MARKER = '-Wl,-Bdynamic'
SONAME_ST = '-Wl,-h,%s'
STATICLIBPATH_ST = '-L%s'
STATICLIB_MARKER = '-Wl,-Bstatic'
STATICLIB_ST = '-l%s'
TBB_ENABLED = True
defines = {'HAVE_SDL': 1}
macbundle_PATTERN = '%s.bundle'
program_PATTERN = '%s'
shlib_CXXFLAGS = ['-fPIC', '-DPIC']
shlib_LINKFLAGS = ['-shared']
shlib_PATTERN = 'lib%s.so'
staticlib_LINKFLAGS = ['-Wl,-Bstatic']
staticlib_PATTERN = 'lib%s.a'
