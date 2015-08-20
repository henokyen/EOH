#! /usr/bin/env python
# encoding: utf-8
# JB Mouret - 2009

"""
Quick n dirty eigen3 detection
"""

import os, glob, types
from waflib.Configure import conf


def options(opt):
        #print "calling eigen opt"
	opt.add_option('--eigen', type='string', help='path to eigen', dest='eigen')


@conf
def check_eigen(conf):
        #print "calling eigen config"
	if conf.options.eigen:
		conf.env.INCLUDES_EIGEN = [conf.options.eigen]
		conf.env.LIBPATH_EIGEN = [conf.options.eigen]
	else:
		conf.env.INCLUDES_EIGEN = ['/home/hmengist/sferes/include/']
	res = conf.find_file('Eigen/Core', conf.env.INCLUDES_EIGEN)

	return 1


