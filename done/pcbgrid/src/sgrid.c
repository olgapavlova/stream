#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PITCH 2.54
#define HOLE_DIAMETER 1.0
#define PAD_DIAMETER 1.8

static long parse_positive_long(const char *text, const char *name)
{
    char *end = NULL;
    errno = 0;

    long value = strtol(text, &end, 10);

    if (errno != 0 || end == text || *end != '\0' || value <= 0) {
        fprintf(stderr, "Ошибка в %s: %s\n", name, text);
        exit(EXIT_FAILURE);
    }

    return value;
}

static const char *filename_part(const char *path)
{
    const char *slash = strrchr(path, '/');
    return slash ? slash + 1 : path;
}

static void footprint_name(
    char *result,
    size_t result_size,
    const char *output_path
)
{
    const char *filename = filename_part(output_path);

    snprintf(result, result_size, "%s", filename);

    char *extension = strstr(result, ".kicad_mod");

    if (extension != NULL && extension[10] == '\0')
        *extension = '\0';
}

int main(int argc, char **argv)
{
    if (argc != 4) {
        fprintf(
            stderr,
            "Вызывать так:\n"
            "  %s COLUMNS ROWS OUTPUT\n\n"
            "Например:\n"
            "  %s 30 20 PcbGrid.pretty/PcbGrid_30x20.kicad_mod\n",
            argv[0],
            argv[0]
        );

        return EXIT_FAILURE;
    }

    const long columns = parse_positive_long(argv[1], "column count");
    const long rows = parse_positive_long(argv[2], "row count");

    const double pitch = PITCH;

    const double pad_diameter = PAD_DIAMETER;

    const double hole_diameter = HOLE_DIAMETER;

    const char *output_path = argv[3];

    FILE *file = fopen(output_path, "w");

    if (file == NULL) {
        fprintf(
            stderr,
            "Не могу открыть %s: %s\n",
            output_path,
            strerror(errno)
        );

        return EXIT_FAILURE;
    }

    char name[256];
    footprint_name(name, sizeof(name), output_path);

    fprintf(file,
        "(footprint \"%s\"\n"
        "  (version 20240108)\n"
        "  (generator \"protogrid-c\")\n"
        "  (layer \"F.Cu\")\n"
        "  (descr \"Generated %ld x %ld plated prototyping grid\")\n"
        "  (tags \"prototype grid plated through hole\")\n"
        "  (attr through_hole)\n",
        name,
        columns,
        rows
    );

    /*
     * Невидимые служебные надписи 
     */
    fprintf(file,
        "  (fp_text reference \"REF**\"\n"
        "    (at 0 -%.4f)\n"
        "    (layer \"F.SilkS\")\n"
        "    (effects\n"
        "      (font (size 1 1) (thickness 0.15))\n"
        "    )\n"
        "  )\n",
        pitch
    );

    fprintf(file,
        "  (fp_text value \"%s\"\n"
        "    (at 0 %.4f)\n"
        "    (layer \"F.Fab\")\n"
        "    (effects\n"
        "      (font (size 1 1) (thickness 0.15))\n"
        "    )\n"
        "  )\n",
        name,
        rows * pitch
    );

    long pad_number = 1;

    for (long row = 0; row < rows; ++row) {
        for (long column = 0; column < columns; ++column) {
            const double x = column * pitch;
            const double y = row * pitch;

            fprintf(file,
                "  (pad \"%ld\" thru_hole circle\n"
                "    (at %.6f %.6f)\n"
                "    (size %.6f %.6f)\n"
                "    (drill %.6f)\n"
                "    (layers \"*.Cu\" \"*.Mask\")\n"
                "  )\n",
                pad_number,
                x,
                y,
                pad_diameter,
                pad_diameter,
                hole_diameter
            );

            ++pad_number;
        }
    }

    fprintf(file, ")\n");

    if (fclose(file) != 0) {
        fprintf(
            stderr,
            "Ошибка записи %s: %s\n",
            output_path,
            strerror(errno)
        );

        return EXIT_FAILURE;
    }

    printf(
        "Получилось %s: %ld x %ld, %ld точек\n",
        output_path,
        columns,
        rows,
        columns * rows
    );

    return EXIT_SUCCESS;
}
