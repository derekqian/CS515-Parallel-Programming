all:
	latex summary
	dvips summary
	ps2pdf summary.ps
	cp summary.pdf ../../public_html/
	chmod 644 ../../public_html/summary.pdf

clean:
	rm -f summary.log
	rm -f summary.aux
	rm -f summary.dvi
	rm -f summary.ps
	rm -f summary.pdf
