/*
 * client_example1.c
 *
 * This example is intended to be used with server_example_basic_io or server_example_goose.
 */

#include "iec61850_client.h"

#include <stdlib.h>
#include <stdio.h>

#include "hal_thread.h"

void reportCallbackFunction(void *parameter, ClientReport report)
{
    MmsValue *dataSetValues = ClientReport_getDataSetValues(report);

    printf("received report for %s\n", ClientReport_getRcbReference(report));

    int i;
    for (i = 0; i < 4; i++)
    {
        ReasonForInclusion reason = ClientReport_getReasonForInclusion(report, i);

        if (reason != IEC61850_REASON_NOT_INCLUDED)
        {
            printf("  GGIO1.SPCSO%i.stVal: %i (included for reason %i)\n", i,
                   MmsValue_getBoolean(MmsValue_getElement(dataSetValues, i)), reason);
        }
    }
}

int main(int argc, char **argv)
{
    char *hostname;
    int tcpPort = 102;

    if (argc > 1)
        hostname = argv[1];
    else
        hostname = "localhost";

    if (argc > 2)
        tcpPort = atoi(argv[2]);

    IedClientError error;

    IedConnection con = IedConnection_create();

    IedConnection_connect(con, &error, hostname, tcpPort);

    if (error == IED_ERROR_OK)
    {
        LinkedList deviceList = IedConnection_getLogicalDeviceList(con, &error);
        LinkedList device = LinkedList_getNext(deviceList);

        // while (device != NULL)
        // {
        //     printf("LD: %s\n", (char *)device->data);

        //     LinkedList logicalNodes = IedConnection_getLogicalDeviceDirectory(con, &error,
        //                                                                       (char *)device->data);

        //     device = LinkedList_getNext(device);
        // }
        /* read an analog measurement value from server */
        while (true)
        {
            // MmsValue *value = IedConnection_readObject(con, &error, "simpleIOGenericIO/GGIO1.AnIn1.mag.f", IEC61850_FC_MX);
            MmsValue *value = IedConnection_readObject(con, &error, "simpleIOGenericIO/GGIO1.AnIn1.mag.f", IEC61850_FC_MX);
            if (value != NULL)
            {
                float fval = MmsValue_toFloat(value);
                printf("read float value: %f\n", fval);
                // MmsValue_delete(value);
            }
        }

    close_connection:

        IedConnection_close(con);
    }
    else
    {
        printf("Failed to connect to %s:%i\n", hostname, tcpPort);
    }

    IedConnection_destroy(con);
    return 0;
}
