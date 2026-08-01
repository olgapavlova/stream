#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static long parse_positive_long(const char *text, const char *name)
{
    char *end = NULL;
    errno = 0;

    long value = strtol(text, &end, 10);

    if (errno != 0 || end == text || *end != '\0' || value <= 0) {
        fprintf(stderr, "Invalid %s: %s\n", name, text);
        exit(EXIT_FAILURE);
    }

    return value;
}

static double parse_positive_double(const char *text, const char *name)
{
    char *end = NULL;
    errno = 0;

    double value = strtod(text, &end);

    if (errno != 0 || end == text || *end != '\0' || value <= 0.0) {
        fprintf(stderr, "Invalid %s: %s\n", name, text);
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
    if (argc != 7) {
        fprintf(
            stderr,
            "Usage:\n"
            "  %s COLUMNS ROWS PITCH PAD_DIAMETER HOLE_DIAMETER OUTPUT\n\n"
            "Example:\n"
            "  %s 30 20 2.54 1.8 1.0 ProtoGrid_30x20.kicad_mod\n",
            argv[0],
            argv[0]
        );

        return EXIT_FAILURE;
    }

    const long columns = parse_positive_long(argv[1], "column count");
    const long rows = parse_positive_long(argv[2], "row count");

    const double pitch =
        parse_positive_double(argv[3], "pitch");

    const double pad_diameter =
        parse_positive_double(argv[4], "pad diameter");

    const double hole_diameter =
        parse_positive_double(argv[5], "hole diameter");

    const char *output_path = argv[6];

    if (hole_diameter >= pad_diameter) {
        fprintf(
            stderr,
            "Hole diameter must be smaller than pad diameter.\n"
        );

        return EXIT_FAILURE;
    }

    FILE *file = fopen(output_path, "w");

    if (file == NULL) {
        fprintf(
            stderr,
            "Cannot open %s: %s\n",
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
     * Anchor footprint text outside the pad field.
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
            "Error writing %s: %s\n",
            output_path,
            strerror(errno)
        );

        return EXIT_FAILURE;
    }

    printf(
        "Created %s: %ld x %ld, %ld pads\n",
        output_path,
        columns,
        rows,
        columns * rows
    );

    return EXIT_SUCCESS;
}
