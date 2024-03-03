#ifndef EDITOR_HPP
#define EDITOR_HPP

#include <string>

class GapBuffer {
private:
    char* buffer;
    int bufferSize;
    int gapStart;
    int gapEnd;

public:
    GapBuffer(int initialSize);
    ~GapBuffer();

    void moveGapTo(int position);
    void insertChar(char c);
    void deleteChar();
    std::string toString() const;

private:
    void resizeBuffer();
};

#endif // EDITOR_HPP
