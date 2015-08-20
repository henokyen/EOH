#!/bin/bash
rm -rf html/documentation/*
rm -rf html/*.html

#for file in *.qbk
#do
quickbook sferes2.qbk
#done
xsltproc --stringparam boost.defaults "Boost" --stringparam boost.libraries "../../libs/libraries.htm" --stringparam manifest "doc_HTML.manifest" --xinclude -o "html/" "./xsl/html.xsl" sferes2.xml

#for file in `find html/* -name '*.html'`
#do
#  echo $file
#  mv $file ${file}.old
#  touch $file
#  cat ${file}.old |sed 's/\.\.\/\.\.\//\.\.\//g'|sed 's/\.\.\/doc/doc/g'|sed 's/\.\.\/logo/logo/g' >> $file
#done

