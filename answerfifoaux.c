#include "answerfifoaux.h"

#include <stdio.h>

void create_header_struct_a(int id, int ret_code, rep_header_t *t)
{
    t->account_id = id;
    t->ret_code = ret_code;
}

void create_balance_struct_a(int balance, rep_balance_t *create)
{
    create->balance = balance;
}

void create_transfer_struct_a(int balance, rep_transfer_t *create)
{
    create->balance = balance;
}

void create_shutdown_struct_a(int active, rep_shutdown_t *shutd)
{
    shutd->active_offices = active;
}

void create_tlv_reply_struct(tlv_reply_t *tlv, int opnumber,
                               rep_header_t *header,
                               rep_balance_t *balance,
                               rep_transfer_t *transfer,
                               rep_shutdown_t *shutdown)
{
    rep_value_t value;
    value.header = *header;

    if (balance != NULL)
        value.balance = *balance;
    if (transfer != NULL)
        value.transfer = *transfer;
    if (shutdown != NULL)
        value.shutdown = *shutdown;

    tlv->length = sizeof(value);
    tlv->type = opnumber;
    tlv->value = value;
}

tlv_reply_t join_structs_to_send_a(int opnumber)
//TODO: add struct with values from the operations to use on the right places and respective return values?
{
    rep_header_t header;
    rep_balance_t balance;
    rep_transfer_t transfer;
    rep_shutdown_t shutdown;
    tlv_reply_t tlv;

    create_header_struct_a(getpid(),0, &header);

    switch(opnumber)
    {
    case 1:
        create_balance_struct_a(0, &balance); //TODO: add real value
        create_tlv_reply_struct(&tlv, opnumber,&header, &balance, NULL,NULL);
        break;
    case 2:
    {
        create_transfer_struct_a(0,&transfer);
        create_tlv_reply_struct(&tlv, opnumber,&header, NULL, &transfer, NULL);
        break;
    }
    case 3:
    {
        create_shutdown_struct_a(0,&shutdown);
        create_tlv_reply_struct(&tlv, opnumber,&header, NULL, &transfer, NULL);
        break;
    }
    case 0:
        create_tlv_reply_struct(&tlv,opnumber, &header, NULL, NULL, NULL);
        break;
    }


    return tlv;
}
