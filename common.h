#ifndef COMMON_H
#define COMMON_H

#include <QString>
#include <QDateTime>
#include <QVector>
#include <QMultiMap>
#include <QDebug>
#include <QFile>
#include <QtMath>

#define VERSION 1.1

#define EVENT_SECONDS_FOR_START -5
#define SECNODS_FOR_ALIGN_QSCD 10
#define NUM_FOR_QSCD_BLOCK 100
#define EVENT_DURATION 120
#define MAX_NUM_STATION 1000
#define KEEP_LARGE_DATA_DURATION 86400
#define MAX_SMALL_NUM_EEW 100

#define STA_LEN 10

#define SMALL_MAP_WIDTH     814
#define SMALL_MAP_HEIGHT    768

#define P_VEL 6.5
#define S_VEL 3.5

// _STATION
typedef struct _station
{
    char netSta[STA_LEN];
    float lat;
    float lon;
    int lmapX;
    int lmapY;
    int smapX;
    int smapY;
    int inUse;
    int maxPgaTime[5];
    float maxPga[5];
    int pgaTime;
    float pga[5];
} _STATION;

//_EEWINFO, _BINARY_EEWLIST_PACKET
enum nudMessageType {NEW, UPDATE, DELETE};
enum MessageCategory {LIVE, TEST};

typedef struct _eewinfo
{
    int eew_evid;
    int version;
    enum MessageCategory message_category;
    enum nudMessageType message_type;
    float magnitude;
    float latitude;
    float longitude;
    float depth;
    int origintime;
    int number_stations;
    int lmapX;
    int lmapY;
    int smapX;
    int smapY;
    char location[100];
    char lddate[12];
} _EEWINFO;


typedef struct _binary_small_eewlist_packet {
    int numEEW;
    _EEWINFO eewInfos[MAX_SMALL_NUM_EEW];
} _BINARY_SMALL_EEWLIST_PACKET;

typedef struct _binary_pga_packet
{
    int numStation;
    int dataTime;
    _STATION staList[MAX_NUM_STATION];
} _BINARY_PGA_PACKET;

static QDateTime convertKST(QDateTime utc)
{
    return utc.addSecs(9 * 3600);
}

static QDateTime convertUTC(QDateTime kst)
{
    return kst.addSecs(-(9 * 3600));
}

#endif // COMMON_H
