ASSIGNMENT = sp20161662_proj

default: 
	cd src && $(MAKE) && cp a.out .. && $(MAKE) clean

clean:
	-rm a.out
	-rm -r .t
	-rm -r $(ASSIGNMENT)*
	-rm $(ASSIGNMENT)*.tar

assignment1:
	-rm -r $(ASSIGNMENT)1
	-rm -r .t
	mkdir .t
	cp README.md .t/README
	cp opcode.txt .t
	cp src/* .t
	cp docs/Report\ 1.docx .t/Document.doc
	mv .t/main.c .t/20161662.c
	echo 'MAINC = 20161662.c\n' | cat - .t/Makefile > .temp
	mv .temp .t/Makefile
	echo 'TARGET = 20161662.out' | cat - .t/Makefile > .temp
	mv .temp .t/Makefile
	mv .t $(ASSIGNMENT)1
	tar -cvf $(ASSIGNMENT)1.tar $(ASSIGNMENT)1
	rm -r $(ASSIGNMENT)1

assignment2:
	-rm -r $(ASSIGNMENT)2
	-rm -r .t
	mkdir .t
	cp README.md .t/README
	cp src/* .t
	cp docs/Report\ 2.docx .t/Document.doc
	mv .t/main.c .t/20161662.c
	echo 'MAINC = 20161662.c\n' | cat - .t/Makefile > .temp
	mv .temp .t/Makefile
	echo 'TARGET = 20161662.out' | cat - .t/Makefile > .temp
	mv .temp .t/Makefile
	mv .t $(ASSIGNMENT)2
	tar -cvf $(ASSIGNMENT)2.tar $(ASSIGNMENT)2
	rm -r $(ASSIGNMENT)2
	
.DEFAULT :
	cd src && $(MAKE) $@
