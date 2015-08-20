#! /usr/bin/env python
# encoding: utf-8
# JB Mouret - 2009

"""
Quick n dirty eigen3 detection
"""

import os, glob, types
from waflib.Configure import conf


def options(opt):
        print "calling gsl opt"
	opt.add_option('--gsl', type='string', help='path to gsl', dest='gsl')


@conf
def check_gsl(conf):
        print "calling gsl config"
	if conf.options.gsl:
		conf.env.INCLUDES_GSL = [conf.options.gsl]
		conf.env.LIBPATH_GSL = [conf.options.gsl]
	else:
		conf.env.INCLUDES_GSL = ['/home/hmengist/sferes/include/']
	#res = conf.find_file('Eigen/Core', conf.env.INCLUDES_EIGEN)

	return 1


