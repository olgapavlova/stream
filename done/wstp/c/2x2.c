#include <stdio.h>
#include <stdlib.h>
#include <wstp.h>

#define WOLFRAM_KERNEL "/opt/Wolfram/WolframEngine/13.3/Executables/WolframKernel"

int main(void) {
    WSENV environment;
    WSLINK link;
    int error;
    int packet;
    int result;

    char *arguments[] = {
        "wstp",
        "-linkmode",
        "launch",
        "-linkname",
        WOLFRAM_KERNEL " -wstp"
    };

    int argument_count =
        sizeof(arguments) / sizeof(arguments[0]);

    environment = WSInitialize(NULL);

    if (environment == NULL) {
        fprintf(stderr, "Не удалось инициализировать WSTP\n");
        return EXIT_FAILURE;
    }

    link = WSOpenArgcArgv(
        environment,
        argument_count,
        arguments,
        &error
    );

    if (link == NULL) {
        fprintf(stderr,
                "Не удалось создать WSTP-соединение: ошибка %d\n",
                error);

        WSDeinitialize(environment);
        return EXIT_FAILURE;
    }

    if (!WSActivate(link)) {
        fprintf(stderr,
                "Не удалось активировать WSTP-соединение: %s\n",
                WSErrorMessage(link));

        WSClose(link);
        WSDeinitialize(environment);
        return EXIT_FAILURE;
    }

    /*
     * Отправляем выражение:
     *
     *     EvaluatePacket[Plus[2, 2]]
     */

    WSPutFunction(link, "EvaluatePacket", 1);
    WSPutFunction(link, "ToExpression", 1);
    WSPutString(link, "2+2");
    WSEndPacket(link);

// Это для случаев динамической компоновки выражения
#if 0
    WSPutFunction(link, "EvaluatePacket", 1);
    WSPutFunction(link, "Plus", 2);
    WSPutInteger32(link, 2);
    WSPutInteger32(link, 2);
    WSEndPacket(link);
#endif

    /*
     * Ядро может прислать служебные пакеты.
     * Пропускаем их, пока не получим ReturnPacket.
     */

    while ((packet = WSNextPacket(link)) != RETURNPKT) {
        if (packet == 0) {
            fprintf(stderr,
                    "Ошибка при чтении ответа: %s\n",
                    WSErrorMessage(link));

            WSClose(link);
            WSDeinitialize(environment);
            return EXIT_FAILURE;
        }

        WSNewPacket(link);
    }

    if (!WSGetInteger32(link, &result)) {
        fprintf(stderr,
                "Ответ не является целым числом: %s\n",
                WSErrorMessage(link));

        WSClose(link);
        WSDeinitialize(environment);
        return EXIT_FAILURE;
    }

    printf("Результат: %d\n", result);

    WSClose(link);
    WSDeinitialize(environment);

    return EXIT_SUCCESS;
}
