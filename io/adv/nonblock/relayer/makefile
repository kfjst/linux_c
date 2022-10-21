all:relayer

CFLAGS+=-pthread
LDFLAGS+=-pthread

relayer:main.o relayer.o
	gcc $^ -o $@ $(CFLAGS) $(LDFLAGS)

clean:
	rm -rf *.o relayer
