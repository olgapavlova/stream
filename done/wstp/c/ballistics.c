#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <wstp.h>

#define WOLFRAM_KERNEL \
    "/opt/Wolfram/WolframEngine/13.3/Executables/WolframKernel"

static void close_wstp(WSLINK link, WSENV environment) {
    if (link != NULL) {
        WSClose(link);
    }

    if (environment != NULL) {
        WSDeinitialize(environment);
    }
}

static void print_wstp_error(WSLINK link, const char *context) {
    fprintf(
        stderr,
        "%s: WSTP error %d: %s\n",
        context,
        WSError(link),
        WSErrorMessage(link)
    );
}

/*
 * Пропускает служебные пакеты и останавливается
 * на содержимом ReturnPacket.
 */
static int wait_for_return_packet(WSLINK link) {
    int packet;

    for (;;) {
        packet = WSNextPacket(link);

        if (packet == 0) {
            return 0;
        }

        if (packet == RETURNPKT) {
            return 1;
        }

        if (!WSNewPacket(link)) {
            return 0;
        }
    }
}

static int load_wolfram_file(
    WSLINK link,
    const char *filename
) {
    /*
     * Отправляем:
     *
     * EvaluatePacket[
     *     Get["/полный/путь/ballistics.wl"]
     * ]
     */
    if (!WSPutFunction(link, "EvaluatePacket", 1) ||
        !WSPutFunction(link, "Get", 1) ||
        !WSPutString(link, filename) ||
        !WSEndPacket(link)) {

        return 0;
    }

    if (!wait_for_return_packet(link)) {
        return 0;
    }

    /*
     * Get возвращает Null.
     * Содержимое ReturnPacket нам не нужно.
     */
    return WSNewPacket(link);
}

static int call_max_height_and_angle(
    WSLINK link,
    double velocity,
    double distance,
    double *height,
    double *angle
) {
    const char *head = NULL;
    int argument_count = 0;

    /*
     * Отправляем:
     *
     * EvaluatePacket[
     *     maxHeightAndAngle[velocity, distance]
     * ]
     */
    if (!WSPutFunction(link, "EvaluatePacket", 1) ||
        !WSPutFunction(link, "maxHeightAndAngle", 2) ||
        !WSPutReal64(link, velocity) ||
        !WSPutReal64(link, distance) ||
        !WSEndPacket(link)) {

        return 0;
    }

    if (!wait_for_return_packet(link)) {
        return 0;
    }

    /*
     * Ожидаемый ответ:
     *
     * List[height, angle]
     *
     * То есть в обычной записи:
     *
     * {height, angle}
     */
    if (!WSGetFunction(link, &head, &argument_count)) {
        return 0;
    }

    if (strcmp(head, "List") != 0 || argument_count != 2) {
        fprintf(
            stderr,
            "Unexpected result: %s with %d arguments\n",
            head,
            argument_count
        );

        WSReleaseSymbol(link, head);
        return 0;
    }

    WSReleaseSymbol(link, head);

    if (!WSGetReal64(link, height) ||
        !WSGetReal64(link, angle)) {

        return 0;
    }

    return 1;
}

static int parse_nonnegative_double(
    const char *text,
    const char *name,
    double *value
) {
    char *end = NULL;

    errno = 0;
    *value = strtod(text, &end);

    if (errno != 0 ||
        end == text ||
        *end != '\0') {

        fprintf(stderr, "Invalid %s: %s\n", name, text);
        return 0;
    }

    if (*value < 0.0) {
        fprintf(stderr, "%s must not be negative\n", name);
        return 0;
    }

    return 1;
}

int main(int argc, char **argv) {
    WSENV environment = NULL;
    WSLINK link = NULL;

    int error = WSEOK;

    double velocity;
    double distance;
    double height;
    double angle;

    char wolfram_file[PATH_MAX];

    char *link_arguments[] = {
        "wstp",
        "-linkmode",
        "launch",
        "-linkname",
        WOLFRAM_KERNEL " -wstp"
    };

    int link_argument_count =
        (int)(sizeof(link_arguments) /
              sizeof(link_arguments[0]));

    if (argc != 3) {
        fprintf(
            stderr,
            "Usage: %s VELOCITY DISTANCE\n"
            "Example: %s 50 100\n",
            argv[0],
            argv[0]
        );

        return EXIT_FAILURE;
    }

    if (!parse_nonnegative_double(
            argv[1],
            "velocity",
            &velocity)) {

        return EXIT_FAILURE;
    }

    if (velocity == 0.0) {
        fprintf(stderr, "Velocity must be greater than zero\n");
        return EXIT_FAILURE;
    }

    if (!parse_nonnegative_double(
            argv[2],
            "distance",
            &distance)) {

        return EXIT_FAILURE;
    }

    /*
     * Получаем абсолютный путь к ballistics.wl.
     * Файл должен находиться в текущем каталоге.
     */
    if (realpath("ballistics.wl", wolfram_file) == NULL) {
        perror("realpath ballistics.wl");
        return EXIT_FAILURE;
    }

    environment = WSInitialize(NULL);

    if (environment == NULL) {
        fprintf(stderr, "WSInitialize failed\n");
        return EXIT_FAILURE;
    }

    link = WSOpenArgcArgv(
        environment,
        link_argument_count,
        link_arguments,
        &error
    );

    if (link == NULL || error != WSEOK) {
        fprintf(
            stderr,
            "WSOpenArgcArgv failed: WSTP error %d\n",
            error
        );

        close_wstp(link, environment);
        return EXIT_FAILURE;
    }

    if (!WSActivate(link)) {
        print_wstp_error(link, "WSActivate");
        close_wstp(link, environment);
        return EXIT_FAILURE;
    }

    if (!load_wolfram_file(link, wolfram_file)) {
        print_wstp_error(link, "Loading ballistics.wl");
        close_wstp(link, environment);
        return EXIT_FAILURE;
    }

    if (!call_max_height_and_angle(
            link,
            velocity,
            distance,
            &height,
            &angle)) {

        print_wstp_error(
            link,
            "Calling maxHeightAndAngle"
        );

        close_wstp(link, environment);
        return EXIT_FAILURE;
    }

    printf("Velocity:       %8.2f m/s\n", velocity);
    printf("Distance:       %8.2f m\n", distance);
    printf("Maximum height: %8.2f m\n", height);
    printf("Launch angle:   %8.2f ˚\n", angle);

    close_wstp(link, environment);

    return EXIT_SUCCESS;
}
