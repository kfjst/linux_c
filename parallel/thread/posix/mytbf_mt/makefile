all:mytbf

CFLAGS+=-pthread
LDFLAGS+=-pthread

mytbf:main.o mytbf.o
	gcc $^ -o $@ $(CFLAGS) $(LDFLAGS)


clean:
	rm -rf *.o mytbf
