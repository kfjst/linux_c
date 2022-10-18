all:mysem

CFLAGS+=-pthread
LDFLAGS+=-pthread

mysem:main.o mysem.o
	gcc $^ -o $@ $(CFLAGS) $(LDFLAGS)

clean:
	rm -rf *.o mysem
