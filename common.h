#ifndef COMMON_H
#define COMMON_H

#include <QString>
#include <QDateTime>
#include <QVector>
#include <QMultiMap>
#include <QDebug>
#include <QFile>
#include "math.h"

#define VERSION 0.1
#define NUM_FOR_QSCD_BLOCK 100
#define EVENT_SECONDS_FOR_START -5
#define EVENT_DURATION 120
#define SECNODS_FOR_ALIGN_QSCD 10
#define PI 3.14159265358979323846
#define DATA_DURATION 43200
#define MAX_NUM_STATION 1000
#define MAX_NUM_EVENT 10

#define STA_LEN 10
#define CHAN_LEN 5
#define NET_LEN 4
#define LOC_LEN 4

#define IMAGE_X_WIDTH 814
#define IMAGE_Y_HEIGHT 768

#define P_VEL 6.5
#define S_VEL 3.5

typedef struct _configure
{
    QString configFileName;

    QString homeDir;
    QString logDir;
    QString eventDir;

    int websocketPort;

    int sps;        // QSCD20 or QSCD100
    QString network;    // MPSS or KISS
    QString staType;
    QString processName;    // KISS20 or KISS100 or MPSS20_G or MPSS20_B or MPSS20_T

    QString db_ip, db_name, db_user, db_passwd;

    QString qscd_amq_ip, qscd_amq_port, qscd_amq_user, qscd_amq_passwd, qscd_amq_topic;
    QString eew_amq_ip, eew_amq_port, eew_amq_user, eew_amq_passwd, eew_amq_topic;
} _CONFIGURE;

typedef struct _station
{
    int index;
    char netsta[STA_LEN];
    char staType;
    float lat;
    float lon;
    int mapX;
    int mapY;
    int inUse;
    int lastPGATime;
    float lastPGA[5];
} _STATION;

typedef struct _qscd_for_multimap
{
    QString netsta;
    float pga[5];
} _QSCD_FOR_MULTIMAP;

typedef struct _event
{
    int evid;        // same with eew_evid
    int eventEpochStartTime;
    float lat;
    float lon;
    int mapX;
    int mapY;
    float mag;
} _EVENT;

/*
typedef struct _binary_packet
{
    _EVENT event;
    int numPGAsta;
    int dataTime;
    _STATION staList[MAX_NUM_STATION];
} _BINARY_PACKET;
*/

typedef struct _binary_packet
{
    int numEVENT;
    _EVENT eventlist[MAX_NUM_EVENT];
    int numPGAsta;
    int dataTime;
    _STATION staList[MAX_NUM_STATION];
} _BINARY_PACKET;

typedef struct _qscd_packet
{
    int crc32 ;         // 4byte CRC Number
    char QFlag ;        // 1byte Quality Flag
    char DType ;        // 1byte Data Type
    char Reserved ;        // 1byte Unused
    char SSSSS[5] ;     // 5byte Station Code
    int time ;          // 4byte Data Time(EpochTime)
    float ZWMin ;     // 4byte U-D Windowed Minimum
    float ZWMax ;     // 4byte U-D Windowed Maximum
    float ZWAvg ;     // 4byte U-D Windowed Average
    float NWMin ;     // 4byte N-S Windowed Minimum
    float NWMax ;     // 4byte N-S Windowed Maximum
    float NWAvg ;     // 4byte N-S Windowed Average
    float EWMin ;     // 4byte E-W Windowed Minimum
    float EWMax ;     // 4byte E-W Windowed Maximum
    float EWAvg ;     // 4byte E-W Windowed Average
    float ZTMin ;     // 4byte U-D True Minimum
    float ZTMax ;     // 4byte U-D True Maximum
    float NTMin ;     // 4byte N-S True Minimum
    float NTMax ;     // 4byte N-S True Maximum
    float ETMin ;     // 4byte E-W True Minimum
    float ETMax ;     // 4byte E-W True Maximum
    float ZMax ;      // 4byte E-W Maximum
    float NMax ;      // 4byte N-S Maximum
    float EMax ;      // 4byte U-D Maximum
    float HPGA ;        // 4byte Horizontal PGA
    float TPGA ;        // 4byte Total PGA
    float ZSI ;       // 4byte U-D SI
    float NSI ;       // 4byte E-W SI
    float ESI ;       // 4byte N-S SI
    float HSI ;       // 4byte Horizontal SI
    float Correl ;    // 4byte Correlation
    char CN1;           // 1byte Channel Number 1
    char CN2;           // 1byte Channel Number 2
    char LO[2];     // 2byte Unused
} _QSCD_PACKET ;


typedef struct _qscd100_packet
{
    int version;
    char msg_type;

    char sta[STA_LEN];
    char chan[CHAN_LEN];
    char net[NET_LEN];
    char loc[LOC_LEN];

    float lat;
    float lon;

    time_t time;

    _QSCD_PACKET qscd;
    char dummy[50];

} _QSCD100_PACKET;

enum nudMessageType {NEW, UPDATE, DELETE};
enum MessageCategory {LIVE, TEST};

static QString find_loc_program = "findLocC";

typedef struct _eewinfo
{
    char type;
    int eew_evid;
    int version;
    enum MessageCategory message_category;
    enum nudMessageType message_type;
    double magnitude;
    double magnitude_uncertainty;
    double latitude;
    double latitude_uncertainty;
    double longitude;
    double longitude_uncertainty;
    double depth;
    double depth_uncertainty;
    double origin_time;
    double origin_time_uncertainty;
    double number_stations;
    double number_triggers; // Seong
    int sent_flag;
    double sent_time;
    double percentsta;
    double misfit;
    int   status;
    QDate lddate;
} _EEWInfo ;

static QDateTime convertKST(QDateTime oriTime)
{
    oriTime = oriTime.addSecs(3600 * 9);
    return oriTime;
}

static QDateTime convertUTC(QDateTime oriTime)
{
    oriTime = oriTime.addSecs( - (3600 * 9) );
    return oriTime;
}

static void SwapFloat(float *data)
{
    char temp;

    union {
        char c[4];
    } dat;

    memcpy( &dat, data, sizeof(float) );
    temp     = dat.c[0];
    dat.c[0] = dat.c[3];
    dat.c[3] = temp;
    temp     = dat.c[1];
    dat.c[1] = dat.c[2];
    dat.c[2] = temp;
    memcpy( data, &dat, sizeof(float) );
    return;
}

static void SwapInt(int *data)
{
    char temp;

    union {
        char c[4];
    } dat;

    memcpy( &dat, data, sizeof(int) );
    temp     = dat.c[0];
    dat.c[0] = dat.c[3];
    dat.c[3] = temp;
    temp     = dat.c[1];
    dat.c[1] = dat.c[2];
    dat.c[2] = temp;
    memcpy( data, &dat, sizeof(int) );
    return;
}

static int geo_to_km(double lat1,double lon1,double lat2,double lon2,double* dist,double* azm)
{
    double a, b;
    double semi_major=a=6378.160;
    double semi_minor=b=6356.775;
    double torad, todeg;
    double aa, bb, cc, dd, top, bottom, lambda12, az, temp;
    double v1, v2;
    double fl, e, e2, eps, eps0;
    double b0, x2, y2, z2, z1, u1p, u2p, xdist;
    double lat1rad, lat2rad, lon1rad, lon2rad;
    double coslon1, sinlon1, coslon2, sinlon2;
    double coslat1, sinlat1, coslat2, sinlat2;
    double tanlat1, tanlat2, cosazm, sinazm;

    double c0, c2, c4, c6;

    double c00=1.0, c01=0.25, c02=-0.046875, c03=0.01953125;
    double c21=-0.125, c22=0.03125, c23=-0.014648438;
    double c42=-0.00390625, c43=0.0029296875;
    double c63=-0.0003255208;

    if( lat1 == lat2 && lon1 == lon2 ) {
        *azm = 0.0;
        *dist= 0.0;
        return(1);
    }

    torad = PI / 180.0;
    todeg = 1.0 / torad;
    fl = ( a - b ) / a;
    e2 = 2.0*fl - fl*fl;
    e  = sqrt(e2);
    eps = e2 / ( 1.0 - e2);

    temp=lat1;
    if(temp == 0.) temp=1.0e-08;
    lat1rad=torad*temp;
    lon1rad=torad*lon1;

    temp=lat2;
    if(temp == 0.) temp=1.0e-08;
    lat2rad=torad*temp;
    lon2rad=torad*lon2;

    coslon1 = cos(lon1rad);
    sinlon1 = sin(lon1rad);
    coslon2 = cos(lon2rad);
    sinlon2 = sin(lon2rad);
    tanlat1 = tan(lat1rad);
    tanlat2 = tan(lat2rad);
    sinlat1 = sin(lat1rad);
    coslat1 = cos(lat1rad);
    sinlat2 = sin(lat2rad);
    coslat2 = cos(lat2rad);

    v1 = a / sqrt( 1.0 - e2*sinlat1*sinlat1 );
    v2 = a / sqrt( 1.0 - e2*sinlat2*sinlat2 );
    aa = tanlat2 / ((1.0+eps)*tanlat1);
    bb = e2*(v1*coslat1)/(v2*coslat2);
    lambda12 = aa + bb;
    top = sinlon2*coslon1 - coslon2*sinlon1;
    bottom = lambda12*sinlat1-coslon2*coslon1*sinlat1-sinlon2*sinlon1*sinlat1;
    az = atan2(top,bottom)*todeg;
    if( az < 0.0 ) az = 360 + az;
    *azm = az;
    az = az * torad;
    cosazm = cos(az);
    sinazm = sin(az);

    if( lat2rad < 0.0 ) {
        temp = lat1rad;
        lat1rad = lat2rad;
        lat2rad = temp;
        temp = lon1rad;
        lon1rad = lon2rad;
        lon2rad = temp;

        coslon1 = cos(lon1rad);
        sinlon1 = sin(lon1rad);
        coslon2 = cos(lon2rad);
        sinlon2 = sin(lon2rad);
        tanlat1 = tan(lat1rad);
        tanlat2 = tan(lat2rad);
        sinlat1 = sin(lat1rad);
        coslat1 = cos(lat1rad);
        sinlat2 = sin(lat2rad);
        coslat2 = cos(lat2rad);

        v1 = a / sqrt( 1.0 - e2*sinlat1*sinlat1 );
        v2 = a / sqrt( 1.0 - e2*sinlat2*sinlat2 );

        aa = tanlat2 / ((1.0+eps)*tanlat1);
        bb = e2*(v1*coslat1)/(v2*coslat2);
        lambda12 = aa + bb;

        top = sinlon2*coslon1 - coslon2*sinlon1;
        bottom =lambda12*sinlat1-coslon2*coslon1*sinlat1-
            sinlon2*sinlon1*sinlat1;
        az = atan2(top,bottom);
        cosazm = cos(az);
        sinazm = sin(az);

    }

    eps0 = eps * ( coslat1*coslat1*cosazm*cosazm + sinlat1*sinlat1 );
    b0 = (v1/(1.0+eps0)) * sqrt(1.0+eps*coslat1*coslat1*cosazm*cosazm);

    x2 = v2*coslat2*(coslon2*coslon1+sinlon2*sinlon1);
    y2 = v2*coslat2*(sinlon2*coslon1-coslon2*sinlon1);
    z2 = v2*(1.0-e2)*sinlat2;
    z1 = v1*(1.0-e2)*sinlat1;

    c0 = c00 + c01*eps0 + c02*eps0*eps0 + c03*eps0*eps0*eps0;
    c2 =       c21*eps0 + c22*eps0*eps0 + c23*eps0*eps0*eps0;
    c4 =                  c42*eps0*eps0 + c43*eps0*eps0*eps0;
    c6 =                                  c63*eps0*eps0*eps0;

    bottom = cosazm*sqrt(1.0+eps0);
    u1p = atan2(tanlat1,bottom);

    top = v1*sinlat1+(1.0+eps0)*(z2-z1);
    bottom = (x2*cosazm-y2*sinlat1*sinazm)*sqrt(1.0+eps0);
    u2p = atan2(top,bottom);

    aa = c0*(u2p-u1p);
    bb = c2*(sin(2.0*u2p)-sin(2.0*u1p));
    cc = c4*(sin(4.0*u2p)-sin(4.0*u1p));
    dd = c6*(sin(6.0*u2p)-sin(6.0*u1p));

    xdist = fabs(b0*(aa+bb+cc+dd));
    *dist = xdist;
    return(1);
}

static double getDistance(double lat1, double lon1, double lat2, double lon2)
{
    double dist, azim;
    int rtn = geo_to_km(lat1, lon1, lat2, lon2, &dist, &azim);
    return dist;
}



#endif // COMMON_H
