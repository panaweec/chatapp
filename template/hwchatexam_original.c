//
//  hwchatexam.c
//  Router_Client
//
//  Created by Pimpat on 9/10/2557 BE.
//  Copyright (c) 2557 Pimpat. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include <time.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "transport.h"

#define FUNCHANDLE_N 13

static void cleanStdin() { while (getchar()!='\n') ;}
int getIdFromConfig(TPTransportCTX *context, char *configFile, char *id);

// function call by tag (set in main 'setTagFuncCall(callHello, 1);' )
static unsigned int callHello(TPTransportMsg *message) {
    char *msg = malloc(message->bytesCount+1);
    memcpy(msg, message->msg, message->bytesCount);
    msg[message->bytesCount] = '\0';
    printf("tag1:%s\n", message->msg);
    free(msg);
    return 0;
}

static void _1ReadMessage(TPTransportCTX *context) {
    TPUserID userID;
    TPTransportMsg *message;
    printf("user_id:\n");
    printf("> ");
    memset(&userID, 0, sizeof(userID));
    scanf("%s", (char *)&userID);
    if (strcmp((char *)&userID, "NULL") != 0) {
        message = TPMsgGet(context, &userID, TP_MSG_TYPE_CHAT);
    }
    else {
        message = TPMsgGet(context, NULL, TP_MSG_TYPE_CHAT);
    }
    if (message != NULL) {
        message->msg[message->bytesCount] = '\0';
        printf("'%s'->'%s': '%s' @%lu\n", message->from, message->to, message->msg, message->timestamp);
        TPMsgFree(message);
    } else {
        printf("not found message\n");
    }
}
static void _2SendToClient(TPTransportCTX *context) {
    int ret;
    char msg[256];
    TPTransportMsg message;
    TPDeviceID target;
    printf("receiver_id:\n");
    printf("> ");
    memset(&target, 0, sizeof(target));
    scanf("%s", (char *)&target);
    printf("msg:\n");
    printf("> ");
    scanf("%s", msg);
    message.msg = msg;
    message.bytesCount = (int)strlen(msg);
    message.msgType = TP_MSG_TYPE_CHAT;
    ret = TPSendToClient(context, &target, &message);
    if (ret != 0) {
        fprintf(stderr, "Client sendToClient return (%d)\n", ret);
    }
}
static void _3SendToUser(TPTransportCTX *context) {
    int ret;
    char msg[256];
    TPTransportMsg message;
    TPUserID sender;
    TPUserID target;
    printf("sender_name:\n");
    printf("> ");
    memset(&sender, 0, sizeof(sender));
    scanf("%s", (char *)&sender);
    printf("receiver_name:\n");
    printf("> ");
    memset(&target, 0, sizeof(target));
    scanf("%s", (char *)&target);
    printf("msg:\n");
    printf("> ");
    scanf("%s", msg);
    message.msg = msg;
    message.bytesCount = (int)strlen(msg);
    message.msgType = TP_MSG_TYPE_CHAT;
    ret = TPSendToUser(context, &sender, &target, &message);
    if (ret != 0) {
        fprintf(stderr, "Client sendToUser return (%d)\n", ret);
    }
}
static void _4SendToGroup(TPTransportCTX *context) {
    int ret;
    char msg[256];
    TPTransportMsg message;
    TPUserID sender;
    TPGroupID groupRef;
    printf("sender:\n");
    printf("> ");
    memset(&sender, 0, sizeof(sender));
    scanf("%s", (char *)&sender);
    printf("group_id:\n");
    printf("> ");
    memset(&groupRef, 0, sizeof(groupRef));
    scanf("%s", (char *)&groupRef);
    printf("msg:\n");
    printf("> ");
    scanf("%s", msg);
    message.msg = msg;
    message.bytesCount = (int)strlen(msg);
    message.msgType = TP_MSG_TYPE_CHAT;
    ret = TPSendToGroup(context, &sender, &groupRef, &message);
    if (ret != 0) {
        fprintf(stderr, "Client sendToGroup return (%d)\n", ret);
    }
}
static void _5SendTagToClient(TPTransportCTX *context) {
    int ret;
    TPTransportMsg message;
    int tag;
    char msg[256];
    TPDeviceID target;
    printf("tag:\n");
    printf("> ");
    scanf("%u", &tag);
    printf("receiver_id:\n");
    printf("> ");
    memset(&target, 0, sizeof(target));
    scanf("%s", (char *)&target);
    printf("msg:\n");
    printf("> ");
    scanf("%s", msg);
    message.msg = msg;
    message.bytesCount = (int)strlen(msg);
    ret = TPSendTagToClient(context, &target, &message, tag);
    if (ret != 0) {
        fprintf(stderr, "Client sendTagToClient return (%d)\n", ret);
    }
}
static void _6SendTagToGroup(TPTransportCTX *context) {
    int ret;
    int tag;
    char msg[256];
    TPTransportMsg message;
    TPUserID sender;
    TPGroupID groupRef;
    printf("tag:\n");
    printf("> ");
    scanf("%d", &tag);
    printf("sender:\n");
    printf("> ");
    memset(&sender, 0, sizeof(sender));
    scanf("%s", (char *)&sender);
    printf("group_id:\n");
    printf("> ");
    memset(&groupRef, 0, sizeof(groupRef));
    scanf("%s", (char *)&groupRef);
    printf("msg:\n");
    printf("> ");
    scanf("%s", msg);
    message.msg = msg;
    message.bytesCount = (int)strlen(msg);
    ret = TPSendTagToGroup(context, &sender, &groupRef, &message, tag);
    if (ret != 0) {
        fprintf(stderr, "Client sendTagToGroup return (%d)\n", ret);
    }
}
static void _7ListChatHistory(TPTransportCTX *context) {
    int ret;
    char refID[16];
    char senderID[16];
    time_t timestamp = time(0);
    int limit = 20;
    TPTransportMsg **arrayMsg;
    printf("refID:\n> ");
    memset(refID, 0, sizeof(refID));
    scanf("%s", refID);
    printf("senderID:\n> ");
    memset(senderID, 0, sizeof(senderID));
    scanf("%s", senderID);
    if (strcmp(senderID, "NULL") == 0) {
        ret = TPMsgGetFromDBWithSender(context, refID, sizeof(refID), NULL, 0, timestamp, limit, &arrayMsg);
    }
    else {
        ret = TPMsgGetFromDBWithSender(context, refID, sizeof(refID), senderID, sizeof(senderID), timestamp, limit, &arrayMsg);
    }
    if (ret != -1) {
        int i;
        for (i=0; i<ret; i++) {
            printf("'%s'->'%s': '%s' @%lu\n", arrayMsg[i]->from, arrayMsg[i]->to, arrayMsg[i]->msg, arrayMsg[i]->timestamp);
            free(arrayMsg[i]);
        }
    }
    else {
        fprintf(stderr, "Client getChatFromDb return (%d)\n", ret);
    }
}
static void _8NewGroup(TPTransportCTX *context) {
    int ret;
    TPGroupID groupRef;
    printf("groupRef:\n");
    printf("> ");
    memset(&groupRef, 0, 16);
    scanf("%s", (char *)&groupRef);
    ret = TPGroupNew(context, &groupRef);
    if (ret != 0) {
        fprintf(stderr, "Client newTPGroupID return (%d)\n", ret);
    }
}
static void _9JoinGroup(TPTransportCTX *context) {
    int ret;
    TPUserID userID;
    TPGroupID groupRef;
    printf("userID:\n");
    printf("> ");
    memset(&userID, 0, sizeof(userID));
    scanf("%s", (char *)&userID);
    printf("group_id:\n");
    printf("> ");
    memset(&groupRef, 0, sizeof(groupRef));
    scanf("%s", (char *)&groupRef);
    ret = TPGroupJoin(context, &userID, &groupRef);
    if (ret != 0) {
        fprintf(stderr, "Client joinGroup return (%d)\n", ret);
    }
}
static void _10LeaveGroup(TPTransportCTX *context) {
    int ret;
    TPUserID userID;
    TPGroupID groupRef;
    printf("userID:\n");
    printf("> ");
    memset(&userID, 0, sizeof(userID));
    scanf("%s", (char *)&userID);
    printf("group_id:\n");
    printf("> ");
    memset(&groupRef, 0, sizeof(groupRef));
    scanf("%s", (char *)&groupRef);
    ret = TPGroupLeave(context, &userID, &groupRef);
    if (ret != 0) {
        fprintf(stderr, "Client leaveGroup return (%d)\n", ret);
    }
}
static void _11ListUser(TPTransportCTX *context) {
    int ret;
    TPArrayUser *arrayUser = NULL;
    ret = TPListAllUser(context, &arrayUser);
    if (ret == 0) {
        int i;
        for (i=0; i<arrayUser->userLen; i++) {
            printf("'%s': %s\n", arrayUser->users[i].userID, arrayUser->users[i].status==TP_STATUS_ONLINE? "online": "offline");
        }
    }
    else {
        fprintf(stderr, "Client listAllUser return (%d)\n", ret);
    }
    TPArrayUserFree(arrayUser);
}
static void _12ListGroup(TPTransportCTX *context) {
    int ret;
    TPGroupID *groupIDs = NULL;
    ret = TPListGroup(context, &groupIDs);
    printf("num group:%d\n", ret);
    int i;
    for (i=0; i<ret; i++) {
        printf("groupID: '%s'\n", groupIDs[i]);
    }
    free(groupIDs);
}
static void _13ListMemberInGroup(TPTransportCTX *context) {
    int ret;
    TPUserID userID;
    TPGroupID groupID;
    TPArrayUser *arrayUser = NULL;
    printf("userID:\n> ");
    memset(&userID, 0, sizeof(userID));
    scanf("%s", (char *)userID);
    printf("groupID:\n> ");
    memset(&groupID, 0, sizeof(groupID));
    scanf("%s", (char *)groupID);
    ret = TPListMemberInGroup(context, &userID, &groupID, &arrayUser);
    if (ret == 0) {
        printf("'%s': member %d\n", (char *)groupID, arrayUser->userLen);
        int i;
        for (i=0; i<arrayUser->userLen; i++) {
            printf("'%s': \n", arrayUser->users[i].userID);
        }
    }
}

static int callTransport() {
    int ret;
    
START:
    printf("Select Router or Client \n");
    printf("\t1: Router\n");
    printf("\t2: Client\n");
    printf("\t0: Exit\n");
    printf("> ");
    ret = -1;
    scanf("%d", &ret);
    cleanStdin();
    if (ret == 1) {
        TPRouterCTX *router = NULL;
        char configFile[256];
        
        printf("config file:\n");
        printf("> ");
        memset(configFile, 0, sizeof(configFile));
        scanf("%s", configFile);
        ret = TPRouterStartFromConfigFile(&router, configFile);
        if (ret != 0) {
            printf("startRouter return (%d)\n", ret);
            return ret;
        }
        printf("[----Router----]\n");
        do {
            printf("\t0: Exit\n");
            printf("> ");
            ret = -1;
            scanf("%d", &ret);
            cleanStdin();
        } while (ret != 0);
        ret = TPRouterStop(router);
        if (ret == 0) {
            printf("Router is stopped\n");
        }
    }
    else if (ret == 2) {
        void (*funcHandle[FUNCHANDLE_N+1])(TPTransportCTX *);
        funcHandle[1] = _1ReadMessage;
        funcHandle[2] = _2SendToClient;
        funcHandle[3] = _3SendToUser;
        funcHandle[4] = _4SendToGroup;
        funcHandle[5] = _5SendTagToClient;
        funcHandle[6] = _6SendTagToGroup;
        funcHandle[7] = _7ListChatHistory;
        funcHandle[8] = _8NewGroup;
        funcHandle[9] = _9JoinGroup;
        funcHandle[10] = _10LeaveGroup;
        funcHandle[11] = _11ListUser;
        funcHandle[12] = _12ListGroup;
        funcHandle[13] = _13ListMemberInGroup;
        
        TPTransportCTX *context = NULL;
        char configFile[256];
        TPDeviceID deviceID;
    CLIENT:
        printf("[----Client----]\n");
        printf("\t1: Start Client\n");
        printf("\t2: Login User\n");
        printf("\t3: Logout User\n");
        printf("\t0: StopClient\n");
        printf("> ");
        ret = -1;
        scanf("%d", &ret);
        cleanStdin();
        if (ret == 1) {
            printf("config file:\n");
            printf("> ");
            memset(configFile, 0, sizeof(configFile));
            scanf("%s", configFile);
            ret = TPTransportInitFromConfigFile(&context, configFile);
            if (ret == 0) {
                TPTagFuncCallSet(context, callHello, 1);
                getIdFromConfig(context, configFile, (char *)deviceID);
                
                char path[64];
                system("mkdir -p chat_history");
                sprintf(path, "./chat_history/%s.db", deviceID);
            }
            else {
                fprintf(stderr, "Client startClient return (%d)\n", ret);
            }
        }
        else if (ret == 2) {
            TPUserID userID;
            printf("userID:\n");
            printf("> ");
            memset(&userID, 0, 16);
            scanf("%s", (char *)&userID);
            ret = TPLoginUser(context, &userID);
            if (ret != 0) {
                fprintf(stderr, "Client loginUser return (%d)\n", ret);
                goto CLIENT;
            }
            
        LOGIN:
            printf("[%s]\n", (char *)&userID);
            printf("\t1: Read a message\n");
            printf("\t2: Send message to Client by DeviceID\n");
            printf("\t3: Send message to Client by Name\n");
            printf("\t4: Send message to Group\n");
            printf("\t5: Send tag to Client by DeviceID\n");
            printf("\t6: Send tag to Group\n");
            printf("\t7: List chat history\n");
            printf("\t8: New group\n");
            printf("\t9: Join group\n");
            printf("\t10: Leave group\n");
            printf("\t11: List user\n");
            printf("\t12: List group\n");
            printf("\t13: List member in group\n");
            printf("\t0: Back\n");
        INPUT:
            printf("> ");
            ret = -1;
            scanf("%d", &ret);
            cleanStdin();
            if (ret > 0 && ret <= FUNCHANDLE_N) {
                funcHandle[ret](context);
            }
            if (ret == 0) {
                goto CLIENT;
            }
            else if (ret == -1) {
                goto LOGIN;
            }
            goto INPUT;
        }
        else if (ret == 3) {
            TPUserID userID;
            printf("userID:\n");
            printf("> ");
            memset(&userID, 0, 16);
            scanf("%s", (char *)&userID);
            ret = TPLogoutUser(context, &userID);
            if (ret != 0) {
                fprintf(stderr, "Client logoutUser return (%d)\n", ret);
            }
        }
        else if (ret == 0) {
            ret = TPTransportClose(context);
            if (ret == 0) {
                printf("Client is stopped\n");
            }
            else {
                fprintf(stderr, "Client stopClient return (%d)\n", ret);
            }
            goto EXIT;
        }
        goto CLIENT;
    }
    else if (ret == 0) {
        goto EXIT;
    }
    else {
        goto START;
    }
EXIT:
    printf("Exit\n");
    
    return 0;
}

int main()
{
    return callTransport();
}

//--------------------------------------------------------------------------------------------------

int getIdFromConfig(TPTransportCTX *context, char *configFile, char *id) {
    TPConfigGet(context, TP_CONFIG_UUID, id, NULL);
    if (strcmp(id, "") == 0) {
        TPConfigGet(context, TP_CONFIG_ID, id, NULL);
    }
    return 0;
}
