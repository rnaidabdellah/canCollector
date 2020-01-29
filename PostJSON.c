/* Use libcurl to POST JSON data.
Usage: PostJSON <name> <value>
curl-library mailing list thread:
'how do i post json to a https ?'
http://curl.haxx.se/mail/lib-2015-01/0049.html
Copyright (C) 2015 Jay Satiro <raysatiro@yahoo.com>
http://curl.haxx.se/docs/copyright.html
https://gist.github.com/jay/2a6c54cc6da442489561
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* http://curl.haxx.se/download.html */
#include <curl/curl.h>

/* http://sourceforge.net/projects/cjson/
cJSON is not thread safe, however that is not an issue in this example.
http://sourceforge.net/p/cjson/feature-requests/9/
*/
#include "cJSON.h"
#include "PostJSON.h"


#include "types.h"
/*#include "definitions.h"
#include "can.h"
#include "service.h"
#include "global.h"
#include "tasks.h"
#include "canConverter.h"*/
#include "appConfig.h"


#undef FALSE
#define FALSE 0
#undef TRUE
#define TRUE 1

#ifdef _WIN32
#undef snprintf
#define snprintf _snprintf
#endif



 void itoa(int n, char s[]);
 void reverse(char s[]);


char socname[] = "SOC";
char socvalue[] = "55";
char tmname[] = "localtime";
char tmvalue[] = "2020-01-22 13:23:55";



/* Post JSON data to a server.
name and value must be UTF-8 strings.
Returns TRUE on success, FALSE on failure.
*/
int PostJSON(const char *name, const char *value)
{
  int retcode = FALSE;
  cJSON *root = NULL, *item = NULL;
  char *json = NULL;
  CURL *curl = NULL;
  CURLcode res = CURLE_FAILED_INIT;
  char errbuf[CURL_ERROR_SIZE] = { 0, };
  struct curl_slist *headers = NULL;
  char agent[1024] = { 0, };

  if(!name) {
    fprintf(stderr, "Error: Missing name parameter.\n");
    goto cleanup;
  }

  if(!value) {
    fprintf(stderr, "Error: Missing value parameter.\n");
    goto cleanup;
  }

  root = cJSON_CreateObject();
  if(!root) {
    fprintf(stderr, "Error: cJSON_CreateObject failed.\n");
    goto cleanup;
  }

  cJSON_AddStringToObject(root, name, value);
  /* There is no return value for cJSON_AddStringToObject. To make sure it
  succeeded we try retrieving what we added. If you are adding a lot of values
  it would be much more efficient to write a malloc wrapper that sets a global
  fail flag if malloc fails; point cJSON to your wrapper via cJSON_InitHooks,
  and then test the fail flag after adding all your values.
  */
  item = cJSON_GetObjectItem(root, name);
  if(!item || !item->valuestring || strcmp(item->valuestring, value)) {
    fprintf(stderr, "Error: cJSON_AddStringToObject failed.\n");
    goto cleanup;
  }

  json = cJSON_PrintUnformatted(root);
  if(!json) {
    fprintf(stderr, "Error: cJSON_PrintUnformatted failed.\n");
    goto cleanup;
  }

  curl = curl_easy_init();
  if(!curl) {
    fprintf(stderr, "Error: curl_easy_init failed.\n");
    goto cleanup;
  }

  /* CURLOPT_CAINFO
  To verify SSL sites you may need to load a bundle of certificates.
  You can download the default bundle here:
  https://raw.githubusercontent.com/bagder/ca-bundle/master/ca-bundle.crt
  However your SSL backend might use a database in addition to or instead of
  the bundle.
  http://curl.haxx.se/docs/ssl-compared.html
  */
  /*curl_easy_setopt(curl, CURLOPT_CAINFO, "curl-ca-bundle.crt");

  snprintf(agent, sizeof agent, "libcurl/%s", curl_version_info(CURLVERSION_NOW)->version);
  agent[sizeof agent - 1] = 0;
  curl_easy_setopt(curl, CURLOPT_USERAGENT, agent);

  headers = curl_slist_append(headers, "Expect:");*/ //comment by rachid

  headers = curl_slist_append(headers, "Content-Type: application/json");
  headers = curl_slist_append(headers, "x-apikey: 532351f556101975e9a044367535f6156d83b");//added by rachid
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json);
  curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, -1L);

  /* This is a test server, it fakes a reply as if the json object were
     created */
  //curl_easy_setopt(curl, CURLOPT_URL, "http://jsonplaceholder.typicode.com/posts");//comment by rachid
  curl_easy_setopt(curl, CURLOPT_URL, "https://bmsg2db-6954.restdb.io/rest/g2lifetimedata");//Added by rachid

  //curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
  curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);
  curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errbuf);

  //Added by rachid for aloowing insecure connection
  curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);

  printf("\njson :: %s\n",json);

  res = curl_easy_perform(curl);
  if(res != CURLE_OK) {
    size_t len = strlen(errbuf);
    fprintf(stderr, "\nlibcurl: (%d) ", res);
    if(len)
      fprintf(stderr, "%s%s", errbuf, ((errbuf[len - 1] != '\n') ? "\n" : ""));
    fprintf(stderr, "%s\n\n", curl_easy_strerror(res));
    goto cleanup;
  }

  retcode = TRUE;
cleanup:
  curl_slist_free_all(headers);
  curl_easy_cleanup(curl);
  cJSON_Delete(root);
  free(json);
  return retcode;
}

/* Post JSON data to a server.
name and value must be UTF-8 strings.
Returns TRUE on success, FALSE on failure.
*/
int postJSONMSGOuf()
{
  int retcode = FALSE;
  cJSON *lroot = NULL, *item = NULL;
  char *ljson = NULL;
  CURL *lcurl = NULL;
  CURLcode res = CURLE_FAILED_INIT;
  char errbuf[CURL_ERROR_SIZE] = { 0, };
  struct curl_slist *headers = NULL;
  char agent[1024] = { 0, };
  int i;
  char snum[10];
  char tmpYes[] = "Yes";
  bbool isPrimary = FALSE;
  time_t now = time(NULL);

  lroot = cJSON_CreateObject();
  if(!lroot) {
    fprintf(stderr, "Error: cJSON_CreateObject failed.\n");
    goto cleanup;
  }

  strftime(tmvalue, 20, "%Y-%m-%d %H:%M:%S", localtime(&now));
  cJSON_AddStringToObject(lroot, tmname, tmvalue);
  item = cJSON_GetObjectItem(lroot, tmname);
  if(!item || !item->valuestring || strcmp(item->valuestring, tmvalue)) {
    fprintf(stderr, "Error: cJSON_AddStringToObject failed.\n");
    goto cleanup;
  }

  for(i=0;i<10;i++)
  {
    if(ALL_VAR_ARRAY[i].isSet)
    {
        isPrimary = (strcmp(tmpYes,ALL_VAR_ARRAY[i].primary)==0);
        if(isPrimary)
        {
            itoa(ALL_VAR_ARRAY[i].value, snum);
            cJSON_AddStringToObject(lroot, ALL_VAR_ARRAY[i].tag, snum);
            item = cJSON_GetObjectItem(lroot, ALL_VAR_ARRAY[i].tag);
            if(!item || !item->valuestring || strcmp(item->valuestring, snum)) {
                fprintf(stderr, "Error: cJSON_AddStringToObject failed.\n");
                goto cleanup;
            }
        }
    }
  }



  ljson = cJSON_PrintUnformatted(lroot);
  if(!ljson) {
    fprintf(stderr, "Error: cJSON_PrintUnformatted failed.\n");
    goto cleanup;
  }

  lcurl = curl_easy_init();
  if(!lcurl) {
    fprintf(stderr, "Error: curl_easy_init failed.\n");
    goto cleanup;
  }



  headers = curl_slist_append(headers, "Content-Type: application/json");

  //bmsg2db
  headers = curl_slist_append(headers, "x-apikey: 532351f556101975e9a044367535f6156d83b");//added by rachid
  curl_easy_setopt(lcurl, CURLOPT_POSTFIELDS, ljson);
  curl_easy_setopt(lcurl, CURLOPT_POSTFIELDSIZE, -1L);
  curl_easy_setopt(lcurl, CURLOPT_URL, "https://bmsg2db-6954.restdb.io/rest/g2lifetimedata");//Added by rachid

  /*
  //bms63
  headers = curl_slist_append(headers, "x-apikey: f55015bfbf548843ab36947644ebdfb7e0e6c");//added by rachid
  curl_easy_setopt(lcurl, CURLOPT_POSTFIELDS, ljson);
  curl_easy_setopt(lcurl, CURLOPT_POSTFIELDSIZE, -1L);
  curl_easy_setopt(lcurl, CURLOPT_URL, "https://bms063v00-44cb.restdb.io/rest/bms063voodm");//Added by rachid
*/

  //curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
  curl_easy_setopt(lcurl, CURLOPT_VERBOSE, 0L);
  curl_easy_setopt(lcurl, CURLOPT_ERRORBUFFER, errbuf);

  //Added by rachid for aloowing insecure connection
  curl_easy_setopt(lcurl, CURLOPT_SSL_VERIFYPEER, 0L);

  printf("\njson :: %s\n",ljson);

  res = curl_easy_perform(lcurl);
  if(res != CURLE_OK) {
    size_t len = strlen(errbuf);
    fprintf(stderr, "\nlibcurl: (%d) ", res);
    if(len)
      fprintf(stderr, "%s%s", errbuf, ((errbuf[len - 1] != '\n') ? "\n" : ""));
    fprintf(stderr, "%s\n\n", curl_easy_strerror(res));
    goto cleanup;
  }

  retcode = TRUE;
cleanup:
  curl_slist_free_all(headers);
  curl_easy_cleanup(lcurl);
  cJSON_Delete(lroot);
  free(ljson);
  return retcode;
}


/* Post JSON data to a server.
name and value must be UTF-8 strings.
Returns TRUE on success, FALSE on failure.
*/
int postJSONMSG()
{
  int retcode = FALSE;
  cJSON *lroot = NULL, *item = NULL;
  char *ljson = NULL;
  CURL *lcurl = NULL;
  CURLcode res = CURLE_FAILED_INIT;
  char errbuf[CURL_ERROR_SIZE] = { 0, };
  struct curl_slist *headers = NULL;
  char agent[1024] = { 0, };
  time_t now = time(NULL);
  int i;
  char snum[10];
  char tmpYes[] = "Yes";
  bbool isPrimary = FALSE;

  lroot = cJSON_CreateObject();
  if(!lroot) {
    fprintf(stderr, "Error: cJSON_CreateObject failed.\n");
    goto cleanup;
  }

  strftime(tmvalue, 20, "%Y-%m-%d %H:%M:%S", localtime(&now));
  cJSON_AddStringToObject(lroot, tmname, tmvalue);
  item = cJSON_GetObjectItem(lroot, tmname);
  if(!item || !item->valuestring || strcmp(item->valuestring, tmvalue)) {
    fprintf(stderr, "Error: cJSON_AddStringToObject failed.\n");
    goto cleanup;
  }

  for(i=0;i<10;i++)
  {
    if(ALL_VAR_ARRAY[i].isSet)
    {
        isPrimary = (strcmp(tmpYes,ALL_VAR_ARRAY[i].primary)==0);
        if(isPrimary)
        {
            itoa(ALL_VAR_ARRAY[i].value, snum);
            cJSON_AddStringToObject(lroot, ALL_VAR_ARRAY[i].tag, snum);
            item = cJSON_GetObjectItem(lroot, ALL_VAR_ARRAY[i].tag);
            if(!item || !item->valuestring || strcmp(item->valuestring, snum)) {
                fprintf(stderr, "Error: cJSON_AddStringToObject failed.\n");
                goto cleanup;
            }
        }
    }
  }

  ljson = cJSON_PrintUnformatted(lroot);
  if(!ljson) {
    fprintf(stderr, "Error: cJSON_PrintUnformatted failed.\n");
    goto cleanup;
  }

  lcurl = curl_easy_init();
  if(!lcurl) {
    fprintf(stderr, "Error: curl_easy_init failed.\n");
    goto cleanup;
  }

  //g2bms database
  /*headers = curl_slist_append(headers, "Content-Type: application/json");
  headers = curl_slist_append(headers, "x-apikey: 532351f556101975e9a044367535f6156d83b");
  curl_easy_setopt(lcurl, CURLOPT_HTTPHEADER, headers);
  curl_easy_setopt(lcurl, CURLOPT_POSTFIELDS, ljson);
  curl_easy_setopt(lcurl, CURLOPT_POSTFIELDSIZE, -1L);
  curl_easy_setopt(lcurl, CURLOPT_URL, "https://bmsg2db-6954.restdb.io/rest/g2lifetimedata");*/

  //bms63database
  headers = curl_slist_append(headers, "Content-Type: application/json");
  headers = curl_slist_append(headers, "x-apikey: f55015bfbf548843ab36947644ebdfb7e0e6c");//added by rachid
  curl_easy_setopt(lcurl, CURLOPT_HTTPHEADER, headers);
  curl_easy_setopt(lcurl, CURLOPT_POSTFIELDS, ljson);
  curl_easy_setopt(lcurl, CURLOPT_POSTFIELDSIZE, -1L);
  curl_easy_setopt(lcurl, CURLOPT_URL, "https://bms063v00-44cb.restdb.io/rest/bms063voodm");//Added by rachid

  //curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
  curl_easy_setopt(lcurl, CURLOPT_VERBOSE, 0L);
  curl_easy_setopt(lcurl, CURLOPT_ERRORBUFFER, errbuf);

  //Added by rachid for aloowing insecure connection
  curl_easy_setopt(lcurl, CURLOPT_SSL_VERIFYPEER, 0L);

  printf("\njson :: %s\n",ljson);

  res = curl_easy_perform(lcurl);
  if(res != CURLE_OK) {
    size_t len = strlen(errbuf);
    fprintf(stderr, "\nlibcurl: (%d) ", res);
    if(len)
      fprintf(stderr, "%s%s", errbuf, ((errbuf[len - 1] != '\n') ? "\n" : ""));
    fprintf(stderr, "%s\n\n", curl_easy_strerror(res));
    goto cleanup;
  }

  retcode = TRUE;
cleanup:
  curl_slist_free_all(headers);
  curl_easy_cleanup(lcurl);
  cJSON_Delete(lroot);
  free(ljson);
  return retcode;
}

/* Post JSON data to a server.
name and value must be UTF-8 strings.
Returns TRUE on success, FALSE on failure.
*/
int postJSONMSGWorking()
{
  int retcode = FALSE;
  cJSON *lroot = NULL, *item = NULL;
  char *ljson = NULL;
  CURL *lcurl = NULL;
  CURLcode res = CURLE_FAILED_INIT;
  char errbuf[CURL_ERROR_SIZE] = { 0, };
  struct curl_slist *headers = NULL;
  char agent[1024] = { 0, };

  lroot = cJSON_CreateObject();
  if(!lroot) {
    fprintf(stderr, "Error: cJSON_CreateObject failed.\n");
    goto cleanup;
  }

  cJSON_AddStringToObject(lroot, socname, socvalue);
  item = cJSON_GetObjectItem(lroot, socname);
  if(!item || !item->valuestring || strcmp(item->valuestring, socvalue)) {
    fprintf(stderr, "Error: cJSON_AddStringToObject failed.\n");
    goto cleanup;
  }

  cJSON_AddStringToObject(lroot, tmname, tmvalue);
  item = cJSON_GetObjectItem(lroot, tmname);
  if(!item || !item->valuestring || strcmp(item->valuestring, tmvalue)) {
    fprintf(stderr, "Error: cJSON_AddStringToObject failed.\n");
    goto cleanup;
  }

  ljson = cJSON_PrintUnformatted(lroot);
  if(!ljson) {
    fprintf(stderr, "Error: cJSON_PrintUnformatted failed.\n");
    goto cleanup;
  }

  lcurl = curl_easy_init();
  if(!lcurl) {
    fprintf(stderr, "Error: curl_easy_init failed.\n");
    goto cleanup;
  }


  headers = curl_slist_append(headers, "Content-Type: application/json");
  headers = curl_slist_append(headers, "x-apikey: 532351f556101975e9a044367535f6156d83b");//added by rachid
  curl_easy_setopt(lcurl, CURLOPT_HTTPHEADER, headers);

  curl_easy_setopt(lcurl, CURLOPT_POSTFIELDS, ljson);
  curl_easy_setopt(lcurl, CURLOPT_POSTFIELDSIZE, -1L);

  /* This is a test server, it fakes a reply as if the json object were
     created */
  //curl_easy_setopt(curl, CURLOPT_URL, "http://jsonplaceholder.typicode.com/posts");//comment by rachid
  curl_easy_setopt(lcurl, CURLOPT_URL, "https://bmsg2db-6954.restdb.io/rest/g2lifetimedata");//Added by rachid

  //curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
  curl_easy_setopt(lcurl, CURLOPT_VERBOSE, 0L);
  curl_easy_setopt(lcurl, CURLOPT_ERRORBUFFER, errbuf);

  //Added by rachid for aloowing insecure connection
  curl_easy_setopt(lcurl, CURLOPT_SSL_VERIFYPEER, 0L);

  printf("\njson :: %s\n",ljson);

  res = curl_easy_perform(lcurl);
  if(res != CURLE_OK) {
    size_t len = strlen(errbuf);
    fprintf(stderr, "\nlibcurl: (%d) ", res);
    if(len)
      fprintf(stderr, "%s%s", errbuf, ((errbuf[len - 1] != '\n') ? "\n" : ""));
    fprintf(stderr, "%s\n\n", curl_easy_strerror(res));
    goto cleanup;
  }

  retcode = TRUE;
cleanup:
  curl_slist_free_all(headers);
  curl_easy_cleanup(lcurl);
  cJSON_Delete(lroot);
  free(ljson);
  return retcode;
}


/* itoa:  convert n to characters in s */
 void itoa(int n, char s[])
 {
     int i, sign;

     if ((sign = n) < 0)  /* record sign */
         n = -n;          /* make n positive */
     i = 0;
     do {       /* generate digits in reverse order */
         s[i++] = n % 10 + '0';   /* get next digit */
     } while ((n /= 10) > 0);     /* delete it */
     if (sign < 0)
         s[i++] = '-';
     s[i] = '\0';
     reverse(s);
}

 /* reverse:  reverse string s in place */
 void reverse(char s[])
 {
     int i, j;
     char c;

     for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
         c = s[i];
         s[i] = s[j];
         s[j] = c;
     }
}



