/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 * Port of com.b3dgs.lionengine.Version.
 */
#ifndef LH_LE_VERSION_H
#define LH_LE_VERSION_H

typedef struct {
    int major;
    int minor;
    int micro;
} lh_version;

#define LH_VERSION(M,N,P) ((lh_version){ (M), (N), (P) })

#endif
