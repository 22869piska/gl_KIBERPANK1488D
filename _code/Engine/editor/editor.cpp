#include "editor.hpp"

bool BIN_IO::Save(const void* data, size_t size, const char name[])
{
    FILE* f = fopen(name, "wb");//перезапись с нуля
    if (!f) { return false; }

    size_t written = fwrite(data, 1, size, f);
    fclose(f);

    return written == size;
}
bool BIN_IO::SaveAppend(const void* data, size_t size, const char name[])
{
    FILE* f = fopen(name, "ab");//дозапись в конец
    if (!f) { return false; }

    size_t written = fwrite(data, 1, size, f);
    fclose(f);

    return written == size;
}
bool BIN_IO::Load(void* data, size_t size, const char name[],unsigned int OFFSET)
{
    FILE* f = fopen(name, "rb");
    if (!f) {
        perror("LOAD ERROR (File not found?)");
        return false;
    }

    if (fseek(f, OFFSET, SEEK_SET) != 0) {
        perror("LOAD ERROR (fseek failed)");
        fclose(f);
        return false;
    }

    size_t read = fread(data, 1, size, f);
    fclose(f);

    return read == size;
}
//-----------------------------------------------------------------------//
glm::vec2 SnapToGrid(glm::vec2 worldPos)
{
    glm::vec2 snapped;
    // Округляем позицию  сетки
    snapped.x = std::round(worldPos.x / EDITOR::gridsize) * EDITOR::gridsize;
    snapped.y = std::round(worldPos.y / EDITOR::gridsize) * EDITOR::gridsize;
    return snapped;
}
void EDITOR::PreviewMode()
{

}
