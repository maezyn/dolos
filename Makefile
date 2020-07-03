OBJECTS = src/engine/wildfire.o

default: wildfire

wildfire: $(OBJECTS)
	gcc $(OBJECTS) -o $@

clean:
	-rm -f $(OBJECTS)
	-rm -f .wildfire_*
	-rm -f wildfire