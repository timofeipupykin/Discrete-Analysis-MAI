build_cmake:
	cmake -B build

build: build_cmake
	cmake --build build

run: build
	./build/lab${lab}/lab${lab}

test: build
	./build/lab${lab}/lab${lab} < tests.txt

clear:
	rm -rf build

check:
	valgrind --leak-check=full --log-file=valgrind_report.txt ./build/lab${lab}/lab${lab}



lab ?= 1

filename = lab${lab}/report/report

pdf:
	pdflatex ${filename}.tex

text: html
	html2text -width 100 -style pretty ${filename}/${filename}.html | sed -n '/./,$$p' | head -n-2 >${filename}.txt

html:
	@#latex2html -split +0 -info "" -no_navigation ${filename}
	htlatex ${filename}

ps:	dvi
	dvips -t letter ${filename}.dvi

dvi:
	latex ${filename}
	bibtex ${filename}||true
	latex ${filename}
	latex ${filename}

clean:
	rm -f ${filename}.{ps,pdf,log,aux,out,dvi,bbl,blg}