all: clean quickbook xsltproc rsync

clean:
	rm -rf html/documentation/*
	rm -rf html/*.html

quickbook:
	quickbook sferes2.qbk

xsltproc:
	xsltproc --stringparam boost.defaults "Boost" --stringparam boost.libraries "../../libs/libraries.htm" --stringparam manifest "doc_HTML.manifest" --xinclude -o "html/" "./xsl/html.xsl" sferes2.xml

rsync:
	rsync -r ../../doc sferes2@chronos:~/
