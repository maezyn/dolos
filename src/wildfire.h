#ifndef WILDFIRE_H
#define WILDFIRE_H

void propagate();

const char *getFileNameExtension(const char *fileName);

void infectFile(const char *fileName);

#endif /* WILDFIRE_H */