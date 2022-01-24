#ifndef __DEVICEINFO_H__
#define __DEVICEINFO_H__

#ifdef __cplusplus
extern "C" {
#endif

char *getPackageID(void);

char *getDevID(void);

char *getDevBrand(void);

char *getDevModel(void);

int setProjToken(char * token);

char *getProjToken(void);

#ifdef __cplusplus
}
#endif

#endif
