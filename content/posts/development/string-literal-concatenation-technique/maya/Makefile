all:
	make format.exe
	./format.exe

	make format_alt.exe
	./format_alt.exe

	make url_generate.exe
	./url_generate.exe

	make url_generate_alt.exe
	./url_generate_alt.exe

	make multiline.exe
	./multiline.exe

	make multiline_alt.exe
	./multiline_alt.ext

format.exe: format.c
	clang $^ -o $@

format_alt.exe: format_alt.c
	clang $^ -o $@

url_generate.exe: url_generate.cpp
	clang++ $^ -o $@

url_generate_alt.exe: url_generate_alt.cpp
	clang++ $^ -o $@

multiline.exe: multiline.c
	clang $^ -o $@

multiline_alt.exe: multiline_alt.c
	clang $^ -o $@

clean:
	rm -rf *.exe
