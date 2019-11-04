# Makefile in Action

```makefile
main: main.cpp
	g++ -o main main.cpp 

```

```makefile
all: hello

hello: main.o factorial.o hello.o
    g++ main.o factorial.o hello.o -o hello

main.o: main.cpp
    g++ -c main.cpp

factorial.o: factorial.cpp
    g++ -c factorial.cpp

hello.o: hello.cpp
    g++ -c hello.cpp

clean:
    rm *o hello
```

ref:
http://www.cs.colby.edu/maxwell/courses/tutorials/maketutor/
https://www.gnu.org/software/make/manual/html_node/Simple-Makefile.html
https://www.cs.swarthmore.edu/~newhall/unixhelp/howto_makefiles.html
很实用的例子
http://mrbook.org/blog/tutorials/make/
